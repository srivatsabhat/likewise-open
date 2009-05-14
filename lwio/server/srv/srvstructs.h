/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

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



/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        srvstructs.h
 *
 * Abstract:
 *
 *        Likewise IO (LWIO)
 *
 *        Server structures
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#ifndef __STRUCTS_H__
#define __STRUCTS_H__

typedef struct _LWIO_SRV_CONFIG
{
    ULONG ulNumReaders;
    ULONG ulNumWorkers;
    ULONG ulMaxNumWorkItemsInQueue;
    ULONG ulMaxNumPackets;

} LWIO_SRV_CONFIG, *PLWIO_SRV_CONFIG;

typedef struct _SRV_HOST_INFO
{
    LONG  refcount;

    pthread_rwlock_t  mutex;
    pthread_rwlock_t* pMutex;

    PSTR  pszHostname;
    PSTR  pszDomain;

} SRV_HOST_INFO, *PSRV_HOST_INFO;

typedef enum
{
    SHARE_SERVICE_DISK_SHARE = 0,
    SHARE_SERVICE_PRINTER,
    SHARE_SERVICE_COMM_DEVICE,
    SHARE_SERVICE_NAMED_PIPE,
    SHARE_SERVICE_ANY,
    SHARE_SERVICE_UNKNOWN
} SHARE_SERVICE;

typedef struct _SHARE_DB_INFO
{
    LONG refcount;

    pthread_rwlock_t  mutex;
    pthread_rwlock_t* pMutex;

    PWSTR pwszName;
    PWSTR pwszPath;
    PWSTR pwszComment;
    PWSTR pwszSID;

    SHARE_SERVICE service;

    BOOLEAN bMarkedForDeletion;

} SHARE_DB_INFO, *PSHARE_DB_INFO;


typedef struct _SRV_SHARE_ENTRY
{
    PSHARE_DB_INFO pInfo;

    struct _SRV_SHARE_ENTRY  *pNext;

} SRV_SHARE_ENTRY, *PSRV_SHARE_ENTRY;


typedef struct _LWIO_SRV_SHARE_DB_CONTEXT
{
    pthread_rwlock_t  mutex;
    pthread_rwlock_t* pMutex;

    PSRV_SHARE_ENTRY  pShareEntry;

    PWSTR             pwszFileSystemPrefix;
    PWSTR             pwszFileSystemRoot;
    PWSTR             pwszPipeSystemRoot;

} LWIO_SRV_SHARE_DB_CONTEXT, *PLWIO_SRV_SHARE_DB_CONTEXT;


typedef ULONG CCB_TYPE;

#define SRV_CCB_DEVICE 1

typedef struct _SRV_CCB
{
    LONG                    refCount;

    CCB_TYPE                CcbType;
    UNICODE_STRING          AbsolutePathName;
    ACCESS_MASK             DesiredAccess;
    LONG64                  AllocationSize;
    FILE_ATTRIBUTES         FileAttributes;
    FILE_SHARE_FLAGS        ShareAccess;
    FILE_CREATE_DISPOSITION CreateDisposition;
    FILE_CREATE_OPTIONS     CreateOptions;

    struct _SRV_CCB *       pNext;

} SRV_CCB, *PSRV_CCB;

typedef struct _SRV_IRP_CONTEXT
{
    PIRP             pIrp;
    IO_DEVICE_HANDLE targetDeviceHandle;
    UNICODE_STRING   rootPathName;
    UNICODE_STRING   relativePathName;
    UNICODE_STRING   absolutePathName;

} SRV_IRP_CONTEXT, *PSRV_IRP_CONTEXT;


typedef VOID (*PFN_PROD_CONS_QUEUE_FREE_ITEM)(PVOID pItem);

typedef struct _SMB_PROD_CONS_QUEUE
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    LWIO_QUEUE       queue;

    ULONG           ulNumMaxItems;
    ULONG           ulNumItems;

    PFN_PROD_CONS_QUEUE_FREE_ITEM pfnFreeItem;

    pthread_cond_t  event;
    pthread_cond_t* pEvent;

} SMB_PROD_CONS_QUEUE, *PSMB_PROD_CONS_QUEUE;

typedef struct _LWIO_SRV_SOCKET
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    int fd;

    struct sockaddr_in cliaddr;

} LWIO_SRV_SOCKET, *PLWIO_SRV_SOCKET;

typedef struct _SRV_PROPERTIES
{
    SMB_SECURITY_MODE preferredSecurityMode;
    BOOLEAN           bEncryptPasswords;
    BOOLEAN           bEnableSecuritySignatures;
    BOOLEAN           bRequireSecuritySignatures;
    USHORT            MaxMpxCount;
    USHORT            MaxNumberVCs;
    ULONG             MaxBufferSize;
    ULONG             MaxRawSize;
    ULONG             Capabilities;
    uuid_t            GUID;

} SRV_PROPERTIES, *PSRV_PROPERTIES;

typedef struct _SRV_CLIENT_PROPERITES
{

    USHORT MaxBufferSize;
    USHORT MaxMpxCount;
    USHORT VcNumber;
    ULONG  SessionKey;
    ULONG  Capabilities;
    PWSTR  pwszNativeOS;
    PWSTR  pwszNativeLanMan;
    PWSTR  pwszNativeDomain;

} SRV_CLIENT_PROPERTIES, *PSRV_CLIENT_PROPERTIES;

typedef struct _SRV_ID_ALLOCATOR
{
    LONG             refcount;

    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    PSMB_BIT_VECTOR  map;

} SRV_ID_ALLOCATOR, *PSRV_ID_ALLOCATOR;

typedef struct _LWIO_SRV_FILE
{
    pthread_rwlock_t        mutex;
    pthread_rwlock_t*       pMutex;

    LONG                    refcount;

    USHORT                  fid;

    IO_FILE_HANDLE          hFile;
    PIO_FILE_NAME           pFilename; // physical path on server
    PWSTR                   pwszFilename; // requested path
    ACCESS_MASK             desiredAccess;
    LONG64                  allocationSize;
    FILE_ATTRIBUTES         fileAttributes;
    FILE_SHARE_FLAGS        shareAccess;
    FILE_CREATE_DISPOSITION createDisposition;
    FILE_CREATE_OPTIONS     createOptions;

} LWIO_SRV_FILE, *PLWIO_SRV_FILE;

typedef struct _LWIO_SRV_TREE
{
    LONG                   refcount;

    pthread_rwlock_t  mutex;
    pthread_rwlock_t* pMutex;

    USHORT            tid;

    PSHARE_DB_INFO    pShareInfo;

    PLWRTL_RB_TREE    pFileCollection;

    USHORT            nextAvailableFid;

} LWIO_SRV_TREE, *PLWIO_SRV_TREE;

typedef struct _LWIO_SRV_SESSION
{
    LONG              refcount;

    pthread_rwlock_t   mutex;
    pthread_rwlock_t*  pMutex;

    USHORT            uid;

    PLWRTL_RB_TREE    pTreeCollection;

    HANDLE            hFinderRepository;

    USHORT            nextAvailableTid;

    PSTR              pszClientPrincipalName;

    PIO_CREATE_SECURITY_CONTEXT   pIoSecurityContext;

} LWIO_SRV_SESSION, *PLWIO_SRV_SESSION;

typedef struct _LWIO_SRV_CONNECTION
{
    LONG                refCount;

    pthread_rwlock_t     mutex;
    pthread_rwlock_t*    pMutex;

    LWIO_SRV_CONN_STATE  state;

    PLWIO_SRV_SOCKET     pSocket;

    SRV_PROPERTIES        serverProperties;
    SRV_CLIENT_PROPERTIES clientProperties;

    ULONG               ulSequence;

    // Invariant
    // Not owned
    HANDLE              hPacketAllocator;

    struct
    {
        BOOLEAN         bReadHeader;
        size_t          sNumBytesToRead;
        size_t          sOffset;
        PSMB_PACKET     pRequestPacket;

    } readerState;

    PBYTE               pSessionKey;
    ULONG               ulSessionKeyLength;

    PSRV_HOST_INFO      pHostinfo;
    PLWIO_SRV_SHARE_DB_CONTEXT pShareDbContext;

    HANDLE              hGssContext;
    HANDLE              hGssNegotiate;

    PLWRTL_RB_TREE      pSessionCollection;

    USHORT              nextAvailableUid;

} LWIO_SRV_CONNECTION, *PLWIO_SRV_CONNECTION;

typedef struct _LWIO_SRV_CONTEXT
{
    PLWIO_SRV_CONNECTION pConnection;

    PSMB_PACKET         pRequest;

    ULONG               ulRequestSequence;

} LWIO_SRV_CONTEXT, *PLWIO_SRV_CONTEXT;

typedef struct _LWIO_SRV_SOCKET_READER_CONTEXT
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    ULONG          readerId;

    BOOLEAN        bStop;
    BOOLEAN        bActive;

    PLWRTL_RB_TREE pConnections;
    ULONG          ulNumSockets;

    PSMB_PROD_CONS_QUEUE pWorkQueue;

    // pipe used to interrupt the reader
    int fd[2];

} LWIO_SRV_SOCKET_READER_CONTEXT, *PLWIO_SRV_SOCKET_READER_CONTEXT;

typedef struct _LWIO_SRV_SOCKET_READER
{
    pthread_t  reader;
    pthread_t* pReader;

    ULONG      readerId;

    LWIO_SRV_SOCKET_READER_CONTEXT context;

} LWIO_SRV_SOCKET_READER, *PLWIO_SRV_SOCKET_READER;

typedef struct _LWIO_SRV_WORKER_CONTEXT
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    BOOLEAN bStop;

    ULONG   workerId;

    // Invariant
    // not owned
    PSMB_PROD_CONS_QUEUE pWorkQueue;

} LWIO_SRV_WORKER_CONTEXT, *PLWIO_SRV_WORKER_CONTEXT;

typedef struct _LWIO_SRV_WORKER
{
    pthread_t  worker;
    pthread_t* pWorker;

    ULONG      workerId;

    LWIO_SRV_WORKER_CONTEXT context;

} LWIO_SRV_WORKER, *PLWIO_SRV_WORKER;

typedef struct _LWIO_SRV_LISTENER_CONTEXT
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    BOOLEAN bStop;

    SRV_PROPERTIES serverProperties;

    // Invariant
    // Not owned
    HANDLE                    hPacketAllocator;
    HANDLE                    hGssContext;
    PLWIO_SRV_SHARE_DB_CONTEXT pShareDbContext;
    PLWIO_SRV_SOCKET_READER    pReaderArray;
    ULONG                     ulNumReaders;

} LWIO_SRV_LISTENER_CONTEXT, *PLWIO_SRV_LISTENER_CONTEXT;

typedef struct _LWIO_SRV_LISTENER
{
    pthread_t  listener;
    pthread_t* pListener;

    LWIO_SRV_LISTENER_CONTEXT context;

} LWIO_SRV_LISTENER, *PLWIO_SRV_LISTENER;


typedef struct _LWIO_SRV_RUNTIME_GLOBALS
{
    pthread_mutex_t          mutex;
    pthread_mutex_t*         pMutex;

    LWIO_SRV_CONFIG           config;

    LWIO_SRV_SHARE_DB_CONTEXT shareDBContext;

    SMB_PROD_CONS_QUEUE      workQueue;

    PLWIO_SRV_SOCKET_READER   pReaderArray;
    ULONG                    ulNumReaders;

    PLWIO_SRV_WORKER          pWorkerArray;
    ULONG                    ulNumWorkers;

    LWIO_SRV_LISTENER         listener;

    PLWIO_PACKET_ALLOCATOR   hPacketAllocator;

    PSRV_CCB                 pCCBList;

} LWIO_SRV_RUNTIME_GLOBALS, *PLWIO_SRV_RUNTIME_GLOBALS;


#endif /* __STRUCTS_H__ */


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
