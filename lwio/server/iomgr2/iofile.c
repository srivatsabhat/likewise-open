/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

#include "iop.h"

VOID
IopFileObjectLock(
    IN PIO_FILE_OBJECT pFileObject
    )
{
    LwRtlLockMutex(&pFileObject->Mutex);
}

VOID
IopFileObjectUnlock(
    IN PIO_FILE_OBJECT pFileObject
    )
{
    LwRtlUnlockMutex(&pFileObject->Mutex);
}

VOID
IopFileObjectReference(
    IN PIO_FILE_OBJECT pFileObject
    )
{
    LONG count = InterlockedIncrement(&pFileObject->ReferenceCount);
    LWIO_ASSERT(count > 1);
}

VOID
IopFileObjectDereference(
    IN OUT PIO_FILE_OBJECT* ppFileObject
    )
{
    PIO_FILE_OBJECT pFileObject = *ppFileObject;

    if (pFileObject)
    {
        // Note that we do not have to acquire the device
        // lock here since it is impossible to resolve
        // a file object from a device.
        LONG count = InterlockedDecrement(&pFileObject->ReferenceCount);
        LWIO_ASSERT(count >= 0);
        if (0 == count)
        {
            IopFileObjectFree(&pFileObject);
        }
        *ppFileObject = NULL;
    }
}

NTSTATUS
IopFileObjectAllocate(
    OUT PIO_FILE_OBJECT* ppFileObject,
    IN PIO_DEVICE_OBJECT pDevice,
    IN PIO_FILE_NAME FileName
    )
{
    NTSTATUS status = 0;
    int EE = 0;
    PIO_FILE_OBJECT pFileObject = NULL;

    status = IO_ALLOCATE(&pFileObject, IO_FILE_OBJECT, sizeof(*pFileObject));
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    pFileObject->ReferenceCount = 1;
    pFileObject->pDevice = pDevice;

    LwListInit(&pFileObject->IrpList);

    if (FileName->RootFileHandle)
    {
        SetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RELATIVE);
    }

    if (!LwRtlCStringIsNullOrEmpty(FileName->FileName))
    {
        status = LwRtlUnicodeStringAllocateFromWC16String(
                        &pFileObject->FileName,
                        FileName->FileName);
        GOTO_CLEANUP_ON_STATUS_EE(status, EE);
    }

    status = LwRtlInitializeMutex(&pFileObject->Mutex, TRUE);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    status = LwRtlInitializeConditionVariable(&pFileObject->Rundown.Condition);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    IopDeviceLock(pDevice);
    LwListInsertTail(&pDevice->FileObjectsList, &pFileObject->DeviceLinks);
    IopDeviceUnlock(pDevice);

cleanup:
    if (status)
    {
        IopFileObjectDereference(&pFileObject);
    }

    *ppFileObject = pFileObject;

    IO_LOG_LEAVE_ON_STATUS_EE(status, EE);
    return status;
}

VOID
IopFileObjectFree(
    IN OUT PIO_FILE_OBJECT* ppFileObject
    )
{
    PIO_FILE_OBJECT pFileObject = *ppFileObject;

    if (pFileObject)
    {
        LWIO_ASSERT(LwListIsEmpty(&pFileObject->IrpList));

        IopDeviceLock(pFileObject->pDevice);
        LwListRemove(&pFileObject->DeviceLinks);
        IopDeviceUnlock(pFileObject->pDevice);

        LwRtlCleanupConditionVariable(&pFileObject->Rundown.Condition);
        LwRtlCleanupMutex(&pFileObject->Mutex);

        LwRtlUnicodeStringFree(&pFileObject->FileName);

        IoMemoryFree(pFileObject);
        *ppFileObject = NULL;
    }
}

NTSTATUS
IoFileSetContext(
    IN IO_FILE_HANDLE FileHandle,
    IN PVOID FileContext
    )
{
    NTSTATUS status = 0;
    int EE = 0;

    if (FileHandle->pContext)
    {
        assert(FALSE);
        status = STATUS_UNSUCCESSFUL;
        GOTO_CLEANUP_ON_STATUS_EE(status, EE);
    }

    FileHandle->pContext = FileContext;

cleanup:
    IO_LOG_LEAVE_ON_STATUS_EE(status, EE);
    return status;
}

PVOID
IoFileGetContext(
    IN IO_FILE_HANDLE FileHandle
    )
{
    return FileHandle->pContext;    
}

static
NTSTATUS
IopContinueAsyncCloseFile(
    IN PIO_FILE_OBJECT FileHandle,
    IN OPTIONAL PIO_ASYNC_COMPLETE_CALLBACK Callback,
    IN OPTIONAL PVOID CallbackContext,
    IN OPTIONAL PIO_STATUS_BLOCK IoStatusBlock
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    int EE = 0;
    PIRP pIrp = NULL;
    IO_ASYNC_CONTROL_BLOCK asyncControlBlock = { 0 };
    PIO_ASYNC_CONTROL_BLOCK useAsyncControlBlock = NULL;

    status = IopIrpCreate(&pIrp, IRP_TYPE_CLOSE, FileHandle);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

    if (Callback)
    {
        asyncControlBlock.Callback = Callback;
        asyncControlBlock.CallbackContext = CallbackContext;
        useAsyncControlBlock = &asyncControlBlock;
    }

    status = IopIrpDispatch(
                    pIrp,
                    useAsyncControlBlock,
                    IoStatusBlock,
                    NULL);
    if (STATUS_PENDING == status)
    {
        IoDereferenceAsyncCancelContext(&asyncControlBlock.AsyncCancelContext);
    }
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    IopIrpDereference(&pIrp);

    if (!useAsyncControlBlock && IoStatusBlock && (STATUS_PENDING != status))
    {
        IO_STATUS_BLOCK ioStatusBlock = { 0 };

        ioStatusBlock.Status = status;
        *IoStatusBlock = ioStatusBlock;
    }

    IO_LOG_LEAVE_ON_STATUS_EE(status, EE);

    return status;
}

NTSTATUS
IopFileObjectRundown(
    IN PIO_FILE_OBJECT pFileObject,
    IN OPTIONAL PIO_ASYNC_COMPLETE_CALLBACK Callback,
    IN OPTIONAL PVOID CallbackContext,
    IN OPTIONAL PIO_STATUS_BLOCK IoStatusBlock
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    int EE = 0;
    BOOLEAN isLocked = FALSE;
    IO_STATUS_BLOCK ioStatusBlock = { 0 };

    IopFileObjectLock(pFileObject);
    isLocked = TRUE;

    if (IsSetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RUNDOWN))
    {
        LWIO_LOG_ERROR("Attempt to rundown multiple times");
        status = STATUS_FILE_CLOSED;
        GOTO_CLEANUP_EE(EE);
    }

    SetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RUNDOWN);

    IopFileObjectUnlock(pFileObject);
    isLocked = FALSE;

    // Cancel everything now that rundown flag is set.
    IopIrpCancelFileObject(pFileObject);

    // Now check whether we need to wait for rundown.
    IopFileObjectLock(pFileObject);
    isLocked = TRUE;

    if (0 != pFileObject->DispatchedIrpCount)
    {
        // Need to wait

        if (!Callback)
        {
            // Wait inline for synchronous case

            LwRtlWaitConditionVariable(
                    &pFileObject->Rundown.Condition,
                    &pFileObject->Mutex,
                    NULL);
            LWIO_ASSERT(0 == pFileObject->DispatchedIrpCount);
        }
        else
        {
            // Set up rundown callback for async case

            pFileObject->Rundown.Callback = Callback;
            pFileObject->Rundown.CallbackContext = CallbackContext;
            pFileObject->Rundown.pIoStatusBlock = IoStatusBlock;

            status = STATUS_PENDING;
            GOTO_CLEANUP_EE(EE);
        }
    }

    IopFileObjectUnlock(pFileObject);
    isLocked = FALSE;

    // We can now continue closing.

    status = IopContinueAsyncCloseFile(
                    pFileObject,
                    Callback,
                    CallbackContext,
                    IoStatusBlock);
    GOTO_CLEANUP_ON_STATUS_EE(status, EE);

cleanup:
    if (isLocked)
    {
        IopFileObjectUnlock(pFileObject);
    }

    if (status && (STATUS_PENDING != status))
    {
        ioStatusBlock.Status = status;
    }

    if ((STATUS_PENDING != status) && IoStatusBlock)
    {
        *IoStatusBlock = ioStatusBlock;
    }

    // TODO-Perhaps do not ASSERT here because memory allocation
    // might have caused code not to issue close to the driver.
    LWIO_ASSERT((STATUS_SUCCESS == status) || (STATUS_PENDING == status));

    IO_LOG_LEAVE_ON_STATUS_EE(status, EE);
    return status;
}


static
VOID
IopFileObjectCompleteRundown(
    IN PIO_FILE_OBJECT pFileObject
    )
{
    // The file object lock should not be held.

    // Safe because this bit is never reset.
    LWIO_ASSERT(IsSetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RUNDOWN));

    // Is this safe?  In theory, the only IRP that could
    // change this is the close IRP, but that should wait
    // until this function calls or signals.
    //
    // FIXME - This needs to be revisited and verify the invariant
    LWIO_ASSERT(pFileObject->DispatchedIrpCount <= 1);

    if (pFileObject->Rundown.Callback)
    {
        IopContinueAsyncCloseFile(
                pFileObject,
                pFileObject->Rundown.Callback,
                pFileObject->Rundown.CallbackContext,
                pFileObject->Rundown.pIoStatusBlock);
    }
    else
    {
        LwRtlSignalConditionVariable(&pFileObject->Rundown.Condition);
    }
}

NTSTATUS
IopFileObjectAddDispatched(
    IN PIO_FILE_OBJECT pFileObject,
    IN IRP_TYPE Type
    )
{
    NTSTATUS status = STATUS_SUCCESS;

    IopFileObjectLock(pFileObject);

    if ((Type != IRP_TYPE_CLOSE) &&
        IsSetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RUNDOWN))
    {
        status = STATUS_CANCELLED;
    }
    else
    {
        status = STATUS_SUCCESS;

        pFileObject->DispatchedIrpCount++;
        LWIO_ASSERT(pFileObject->DispatchedIrpCount >= 1);
    }

    IopFileObjectUnlock(pFileObject);

    return status;
}

VOID
IopFileObjectRemoveDispatched(
    IN PIO_FILE_OBJECT pFileObject,
    IN IRP_TYPE Type
    )
{
    BOOLEAN needCompleteRundown = FALSE;

    IopFileObjectLock(pFileObject);

    pFileObject->DispatchedIrpCount--;
    LWIO_ASSERT(pFileObject->DispatchedIrpCount >= 0);

    if ((Type != IRP_TYPE_CLOSE) &&
        IsSetFlag(pFileObject->Flags, FILE_OBJECT_FLAG_RUNDOWN) &&
        (0 == pFileObject->DispatchedIrpCount))
    {
        needCompleteRundown = TRUE;
    }

    IopFileObjectUnlock(pFileObject);

    if (needCompleteRundown)
    {
        IopFileObjectCompleteRundown(pFileObject);
    }
}
