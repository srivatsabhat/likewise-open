/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

/*
 * Copyright Likewise Software    2004-2009
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        samr_security.c
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Server Interface
 *
 *        Samr server security descriptor functions
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


static
DWORD
SamrSrvCreateServerDacl(
    PACL *ppDacl
    );


static
DWORD
SamrSrvCreateDacl(
    PACL *ppDacl,
    PACCESS_LIST pList
    );


static
NTSTATUS
SamrSrvCreateLocalUserDacl(
    PSID   pDomainSid,
    DWORD  dwRid,
    DWORD  dwObjectClass,
    PACL  *ppDacl
    );


static
NTSTATUS
SamrSrvCreateLocalGroupDacl(
    PSID   pDomainSid,
    DWORD  dwRid,
    DWORD  dwObjectClass,
    PACL  *ppDacl
    );


DWORD
SamrSrvInitServerSecurityDescriptor(
    PSECURITY_DESCRIPTOR_ABSOLUTE *ppSecDesc
    )
{
    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PSECURITY_DESCRIPTOR_ABSOLUTE pSecDesc = NULL;
    PSID pOwnerSid = NULL;
    PSID pGroupSid = NULL;
    PACL pDacl = NULL;

    BAIL_ON_INVALID_PTR(ppSecDesc);

    dwError = LwAllocateMemory(SECURITY_DESCRIPTOR_ABSOLUTE_MIN_SIZE,
                               OUT_PPVOID(&pSecDesc));
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlCreateSecurityDescriptorAbsolute(
                                    pSecDesc,
                                    SECURITY_DESCRIPTOR_REVISION);
    BAIL_ON_NT_STATUS(ntStatus);

    dwError = LwAllocateWellKnownSid(WinLocalSystemSid,
                                   NULL,
                                   &pOwnerSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetOwnerSecurityDescriptor(
                                    pSecDesc,
                                    pOwnerSid,
                                    FALSE);
    BAIL_ON_NT_STATUS(ntStatus);

    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pGroupSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetGroupSecurityDescriptor(
                                    pSecDesc,
                                    pGroupSid,
                                    FALSE);
    BAIL_ON_NT_STATUS(ntStatus);

    dwError = SamrSrvCreateServerDacl(&pDacl);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetDaclSecurityDescriptor(
                                    pSecDesc,
                                    TRUE,
                                    pDacl,
                                    FALSE);

    *ppSecDesc = pSecDesc;

cleanup:
    if (dwError == ERROR_SUCCESS &&
        ntStatus != STATUS_SUCCESS)
    {
        dwError = LwNtStatusToWin32Error(ntStatus);
    }

    return dwError;

error:
    LW_SAFE_FREE_MEMORY(pSecDesc);

    *ppSecDesc = NULL;
    goto cleanup;
}


static
DWORD
SamrSrvCreateServerDacl(
    PACL *ppDacl
    )
{
    ACCESS_MASK SystemAccessMask = STANDARD_RIGHTS_REQUIRED |
                                   SAMR_ACCESS_CONNECT_TO_SERVER |
                                   SAMR_ACCESS_SHUTDOWN_SERVER |
                                   SAMR_ACCESS_INITIALIZE_SERVER |
                                   SAMR_ACCESS_CREATE_DOMAIN |
                                   SAMR_ACCESS_ENUM_DOMAINS |
                                   SAMR_ACCESS_OPEN_DOMAIN;

    ACCESS_MASK AdminAccessMask = STANDARD_RIGHTS_REQUIRED |
                                  SAMR_ACCESS_CONNECT_TO_SERVER |
                                  SAMR_ACCESS_SHUTDOWN_SERVER |
                                  SAMR_ACCESS_INITIALIZE_SERVER |
                                  SAMR_ACCESS_ENUM_DOMAINS |
                                  SAMR_ACCESS_OPEN_DOMAIN;

    ACCESS_MASK AuthenticatedAccessMask = STANDARD_RIGHTS_READ |
                                          SAMR_ACCESS_CONNECT_TO_SERVER |
                                          SAMR_ACCESS_ENUM_DOMAINS |
                                          SAMR_ACCESS_OPEN_DOMAIN;

    ACCESS_MASK WorldAccessMask = STANDARD_RIGHTS_READ;

    DWORD dwError = ERROR_SUCCESS;
    DWORD dwSystemSidLen = 0;
    DWORD dwBuiltinAdminsSidLen = 0;
    DWORD dwAuthenticatedSidLen = 0;
    DWORD dwWorldSidLen = 0;
    PSID pSystemSid = NULL;
    PSID pBuiltinAdminsSid = NULL;
    PSID pAuthenticatedSid = NULL;
    PSID pWorldSid = NULL;
    PACL pDacl = NULL;

    ACCESS_LIST AccessList[] = {
        {
            .ppSid        = &pSystemSid,
            .AccessMask   = SystemAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pBuiltinAdminsSid,
            .AccessMask   = AdminAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pAuthenticatedSid,
            .AccessMask   = AuthenticatedAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pWorldSid,
            .AccessMask   = WorldAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = NULL,
            .AccessMask   = 0,
            .ulAccessType = 0
        }
    };

    /* create local system sid */
    dwError = LwAllocateWellKnownSid(WinLocalSystemSid,
                                   NULL,
                                   &pSystemSid,
                                   &dwSystemSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    /* create administrators sid */
    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pBuiltinAdminsSid,
                                   &dwBuiltinAdminsSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    /* create authenticated users sid */
    dwError = LwAllocateWellKnownSid(WinAuthenticatedUserSid,
                                   NULL,
                                   &pAuthenticatedSid,
                                   &dwAuthenticatedSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    /* create world (everyone) sid */
    dwError = LwAllocateWellKnownSid(WinWorldSid,
                                   NULL,
                                   &pWorldSid,
                                   &dwWorldSidLen);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = SamrSrvCreateDacl(&pDacl,
                                AccessList);
    BAIL_ON_LSA_ERROR(dwError);

    *ppDacl = pDacl;

cleanup:
    LW_SAFE_FREE_MEMORY(pSystemSid);
    LW_SAFE_FREE_MEMORY(pBuiltinAdminsSid);
    LW_SAFE_FREE_MEMORY(pAuthenticatedSid);
    LW_SAFE_FREE_MEMORY(pWorldSid);

    return dwError;

error:
    *ppDacl = NULL;

    goto cleanup;
}


static
DWORD
SamrSrvCreateDacl(
    PACL *ppDacl,
    PACCESS_LIST pList
    )
{
    DWORD dwError = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD dwDaclSize = 0;
    PACL pDacl = NULL;
    DWORD i = 0;
    ULONG ulSidSize = 0;

    dwDaclSize += ACL_HEADER_SIZE;

    for (i = 0; pList[i].ppSid && (*pList[i].ppSid); i++)
    {
        ulSidSize = RtlLengthSid(*(pList[i].ppSid));

        if (pList[i].ulAccessType == ACCESS_ALLOWED_ACE_TYPE)
        {
            dwDaclSize += ulSidSize + sizeof(ACCESS_ALLOWED_ACE);
        }
        else if (pList[i].ulAccessType == ACCESS_DENIED_ACE_TYPE)
        {
            dwDaclSize += ulSidSize + sizeof(ACCESS_DENIED_ACE);
        }
    }

    dwError = LwAllocateMemory(dwDaclSize,
                               OUT_PPVOID(&pDacl));
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlCreateAcl(pDacl, dwDaclSize, ACL_REVISION);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    for (i = 0; pList[i].ppSid && (*pList[i].ppSid); i++)
    {
        if (pList[i].ulAccessType == ACCESS_ALLOWED_ACE_TYPE)
        {
            ntStatus = RtlAddAccessAllowedAceEx(pDacl,
                                                ACL_REVISION,
                                                0,
                                                pList[i].AccessMask,
                                                *(pList[i].ppSid));
        }
        else if (pList[i].ulAccessType == ACCESS_DENIED_ACE_TYPE)
        {
            ntStatus = RtlAddAccessDeniedAceEx(pDacl,
                                               ACL_REVISION,
                                               0,
                                               pList[i].AccessMask,
                                               *(pList[i].ppSid));
        }

        BAIL_ON_NTSTATUS_ERROR(ntStatus);
    }

    *ppDacl = pDacl;

cleanup:
    if (dwError == ERROR_SUCCESS &&
        ntStatus != STATUS_SUCCESS)
    {
        dwError = LwNtStatusToWin32Error(ntStatus);
    }

    return dwError;

error:
    LW_SAFE_FREE_MEMORY(pDacl);
    *ppDacl = NULL;

    goto cleanup;
}


DWORD
SamrSrvDestroyServerSecurityDescriptor(
    PSECURITY_DESCRIPTOR_ABSOLUTE *ppSecDesc
    )
{
    DWORD dwError = ERROR_SUCCESS;

    SamrSrvFreeSecurityDescriptor(ppSecDesc);
    return dwError;
}


NTSTATUS
SamrSrvCreateNewAccountSecurityDescriptor(
    PSID                           pDomainSid,
    DWORD                          dwRid,
    DWORD                          dwObjectClass,
    PSECURITY_DESCRIPTOR_ABSOLUTE *ppSecDesc
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD dwError = ERROR_SUCCESS;
    PSECURITY_DESCRIPTOR_ABSOLUTE pSecDesc = NULL;
    PSID pAdminSid = NULL;
    DWORD dwAdminSidSize = 0;
    PSID pBuiltinAdminsSid = NULL;
    DWORD dwBuiltinAdminsSidSize = 0;
    PACL pDacl = NULL;

    BAIL_ON_INVALID_PTR(pDomainSid);
    BAIL_ON_INVALID_PTR(ppSecDesc);
    BAIL_ON_INVALID_PARAMETER(dwRid > DOMAIN_USER_RID_MAX);
    BAIL_ON_INVALID_PARAMETER(dwObjectClass == DIR_OBJECT_CLASS_USER ||
                              dwObjectClass == DIR_OBJECT_CLASS_LOCAL_GROUP);

    dwError = LwAllocateMemory(SECURITY_DESCRIPTOR_ABSOLUTE_MIN_SIZE,
                               OUT_PPVOID(&pSecDesc));
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlCreateSecurityDescriptorAbsolute(
                                    pSecDesc,
                                    SECURITY_DESCRIPTOR_REVISION);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    /*
     * Set owner (Administrator)
     */
    dwError = LwAllocateWellKnownSid(WinAccountAdministratorSid,
                                   pDomainSid,
                                   &pAdminSid,
                                   &dwAdminSidSize);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetOwnerSecurityDescriptor(pSecDesc,
                                             pAdminSid,
                                             FALSE);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    /*
     * Set group (BUILTIN\Administrators)
     */
    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pBuiltinAdminsSid,
                                   &dwBuiltinAdminsSidSize);
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlSetGroupSecurityDescriptor(pSecDesc,
                                             pBuiltinAdminsSid,
                                             FALSE);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    /*
     * Create default DACL
     */
    if (dwObjectClass == DIR_OBJECT_CLASS_USER)
    {
        ntStatus = SamrSrvCreateLocalUserDacl(pDomainSid,
                                              dwRid,
                                              dwObjectClass,
                                              &pDacl);
    }
    else if (dwObjectClass == DIR_OBJECT_CLASS_LOCAL_GROUP)
    {
        ntStatus = SamrSrvCreateLocalGroupDacl(pDomainSid,
                                               dwRid,
                                               dwObjectClass,
                                               &pDacl);
    }
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    ntStatus = RtlSetDaclSecurityDescriptor(
                                    pSecDesc,
                                    TRUE,
                                    pDacl,
                                    FALSE);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    *ppSecDesc = pSecDesc;

cleanup:
    if (ntStatus == STATUS_SUCCESS &&
        dwError != ERROR_SUCCESS)
    {
        ntStatus = LwWin32ErrorToNtStatus(dwError);
    }

    return ntStatus;

error:
    SamrSrvFreeSecurityDescriptor(&pSecDesc);

    goto cleanup;
}


static
NTSTATUS
SamrSrvCreateLocalUserDacl(
    PSID   pDomainSid,
    DWORD  dwRid,
    DWORD  dwObjectClass,
    PACL  *ppDacl
    )
{
    ACCESS_MASK AdminAccessMask = STANDARD_RIGHTS_ALL |
                                  USER_ACCESS_GET_NAME_ETC |
                                  USER_ACCESS_GET_LOCALE |
                                  USER_ACCESS_SET_LOC_COM |
                                  USER_ACCESS_GET_LOGONINFO |
                                  USER_ACCESS_GET_ATTRIBUTES |
                                  USER_ACCESS_SET_ATTRIBUTES |
                                  USER_ACCESS_CHANGE_PASSWORD |
                                  USER_ACCESS_SET_PASSWORD |
                                  USER_ACCESS_GET_GROUPS |
                                  USER_ACCESS_GET_GROUP_MEMBERSHIP |
                                  USER_ACCESS_CHANGE_GROUP_MEMBERSHIP;

    ACCESS_MASK AccountAccessMask = STANDARD_RIGHTS_READ |
                                    USER_ACCESS_GET_NAME_ETC |
                                    USER_ACCESS_GET_LOCALE |
                                    USER_ACCESS_GET_LOGONINFO |
                                    USER_ACCESS_GET_ATTRIBUTES |
                                    USER_ACCESS_CHANGE_PASSWORD |
                                    USER_ACCESS_GET_GROUPS |
                                    USER_ACCESS_GET_GROUP_MEMBERSHIP;

    ACCESS_MASK AllAccessMask = STANDARD_RIGHTS_READ |
                                USER_ACCESS_GET_NAME_ETC |
                                USER_ACCESS_GET_LOCALE |
                                USER_ACCESS_GET_LOGONINFO |
                                USER_ACCESS_GET_ATTRIBUTES |
                                USER_ACCESS_CHANGE_PASSWORD |
                                USER_ACCESS_GET_GROUPS |
                                USER_ACCESS_GET_GROUP_MEMBERSHIP;

    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdminSid = NULL;
    PSID pBuiltinAdminsSid = NULL;
    PSID pAccountSid = NULL;
    DWORD dwAccountSidSize = 0;
    PSID pWorldSid = NULL;
    PACL pDacl = NULL;

    ACCESS_LIST AccessList[] = {
        {
            .ppSid        = &pAdminSid,
            .AccessMask   = AdminAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pBuiltinAdminsSid,
            .AccessMask   = AdminAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pAccountSid,
            .AccessMask   = AccountAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pWorldSid,
            .AccessMask   = AllAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = NULL,
            .AccessMask   = 0,
            .ulAccessType = 0
        }
    };

    dwError = LwAllocateWellKnownSid(WinAccountAdministratorSid,
                                   pDomainSid,
                                   &pAdminSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pBuiltinAdminsSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwAccountSidSize = RtlLengthRequiredSid(
                                   pDomainSid->SubAuthorityCount + 1);
    dwError = LwAllocateMemory(dwAccountSidSize,
                               OUT_PPVOID(&pAccountSid));
    BAIL_ON_LSA_ERROR(dwError);

    ntStatus = RtlCopySid(dwAccountSidSize,
                          pAccountSid,
                          pDomainSid);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    ntStatus = RtlAppendRidSid(dwAccountSidSize,
                               pAccountSid,
                               dwRid);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    dwError = LwAllocateWellKnownSid(WinWorldSid,
                                   NULL,
                                   &pWorldSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = SamrSrvCreateDacl(&pDacl,
                                AccessList);
    BAIL_ON_LSA_ERROR(dwError);

    *ppDacl = pDacl;

cleanup:
    LW_SAFE_FREE_MEMORY(pAdminSid);
    LW_SAFE_FREE_MEMORY(pBuiltinAdminsSid);
    LW_SAFE_FREE_MEMORY(pAccountSid);
    LW_SAFE_FREE_MEMORY(pWorldSid);

    if (ntStatus == STATUS_SUCCESS &&
        dwError != ERROR_SUCCESS)
    {
        ntStatus = LwWin32ErrorToNtStatus(dwError);
    }

    return ntStatus;

error:
    if (ppDacl)
    {
        *ppDacl = NULL;
    }

    goto cleanup;
}


static
NTSTATUS
SamrSrvCreateLocalGroupDacl(
    PSID   pDomainSid,
    DWORD  dwRid,
    DWORD  dwObjectClass,
    PACL  *ppDacl
    )
{
    ACCESS_MASK AdminAccessMask = STANDARD_RIGHTS_ALL |
                                  ALIAS_ACCESS_ADD_MEMBER |
                                  ALIAS_ACCESS_REMOVE_MEMBER |
                                  ALIAS_ACCESS_GET_MEMBERS |
                                  ALIAS_ACCESS_LOOKUP_INFO |
                                  ALIAS_ACCESS_SET_INFO;

    ACCESS_MASK AllAccessMask = STANDARD_RIGHTS_READ |
                                ALIAS_ACCESS_GET_MEMBERS |
                                ALIAS_ACCESS_LOOKUP_INFO;

    DWORD dwError = ERROR_SUCCESS;
    PSID pAdminSid = NULL;
    PSID pBuiltinAdminsSid = NULL;
    PSID pWorldSid = NULL;
    PACL pDacl = NULL;

    ACCESS_LIST AccessList[] = {
        {
            .ppSid        = &pAdminSid,
            .AccessMask   = AdminAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pBuiltinAdminsSid,
            .AccessMask   = AdminAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = &pWorldSid,
            .AccessMask   = AllAccessMask,
            .ulAccessType = ACCESS_ALLOWED_ACE_TYPE
        },
        {
            .ppSid        = NULL,
            .AccessMask   = 0,
            .ulAccessType = 0
        }
    };

    dwError = LwAllocateWellKnownSid(WinAccountAdministratorSid,
                                   pDomainSid,
                                   &pAdminSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LwAllocateWellKnownSid(WinBuiltinAdministratorsSid,
                                   NULL,
                                   &pBuiltinAdminsSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = LwAllocateWellKnownSid(WinWorldSid,
                                   NULL,
                                   &pWorldSid,
                                   NULL);
    BAIL_ON_LSA_ERROR(dwError);

    dwError = SamrSrvCreateDacl(&pDacl,
                                AccessList);
    BAIL_ON_LSA_ERROR(dwError);

    *ppDacl = pDacl;

cleanup:
    LW_SAFE_FREE_MEMORY(pAdminSid);
    LW_SAFE_FREE_MEMORY(pBuiltinAdminsSid);
    LW_SAFE_FREE_MEMORY(pWorldSid);

    return dwError;

error:
    if (ppDacl)
    {
        *ppDacl = NULL;
    }

    goto cleanup;
}


VOID
SamrSrvFreeSecurityDescriptor(
    PSECURITY_DESCRIPTOR_ABSOLUTE *ppSecDesc
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PSECURITY_DESCRIPTOR_ABSOLUTE pSecDesc = NULL;
    PSID pOwnerSid = NULL;
    BOOLEAN bOwnerDefaulted = FALSE;
    PSID pPrimaryGroupSid = NULL;
    BOOLEAN bPrimaryGroupDefaulted = FALSE;
    PACL pDacl = NULL;
    BOOLEAN bDaclPresent = FALSE;
    BOOLEAN bDaclDefaulted = FALSE;
    PACL pSacl = NULL;
    BOOLEAN bSaclPresent = FALSE;
    BOOLEAN bSaclDefaulted = FALSE;

    if (ppSecDesc == NULL ||
        *ppSecDesc == NULL)
    {
        return;
    }

    pSecDesc = *ppSecDesc;

    ntStatus = RtlGetOwnerSecurityDescriptor(pSecDesc,
                                             &pOwnerSid,
                                             &bOwnerDefaulted);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    ntStatus = RtlGetGroupSecurityDescriptor(pSecDesc,
                                             &pPrimaryGroupSid,
                                             &bPrimaryGroupDefaulted);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    ntStatus = RtlGetDaclSecurityDescriptor(pSecDesc,
                                            &bDaclPresent,
                                            &pDacl,
                                            &bDaclDefaulted);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    ntStatus = RtlGetSaclSecurityDescriptor(pSecDesc,
                                            &bSaclPresent,
                                            &pSacl,
                                            &bSaclDefaulted);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

error:
    LW_SAFE_FREE_MEMORY(pOwnerSid);
    LW_SAFE_FREE_MEMORY(pPrimaryGroupSid);

    if (bDaclPresent)
    {
        LW_SAFE_FREE_MEMORY(pDacl);
    }

    if (bSaclPresent)
    {
        LW_SAFE_FREE_MEMORY(pSacl);
    }

    LW_SAFE_FREE_MEMORY(pSecDesc);
    *ppSecDesc = NULL;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
