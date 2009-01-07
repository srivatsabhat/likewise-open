#include "smbserver.h"

NTSTATUS
SmbProcessRenameFile(
    PSMB_REQUEST pSmbRequest
    )
{
    NTSTATUS ntStatus = 0;

    ntStatus = UnmarshallRenameFileRequest(pSmbRequest);
    BAIL_ON_NT_STATUS(ntStatus);

    ntStatus = SrvRenameFile(
                        hTreeObject
                        );
    BAIL_ON_NT_STATUS(ntStatus);
    
    
    ntStatus = MarshallRenameFileResponse(pSmbRequest);
    BAIL_ON_NT_STATUS(ntStatus);


    ntStatus = SmbSendReply(pSmbRequest);
    BAIL_ON_NT_STATUS(ntStatus);

error:

    return (ntStatus);
}


NTSTATUS
UnmarshallRenameFileRequest(
    PSMB_REQUEST pSmbRequest
    )
{
    NTSTATUS ntStatus = 0;

    return (ntStatus);
}


NTSTATUS
MarshallRenameFileResponse(
    PSMB_REQUEST pSmbRequest
    )
{
    NTSTATUS ntStatus = 0;

    return (ntStatus);

}
