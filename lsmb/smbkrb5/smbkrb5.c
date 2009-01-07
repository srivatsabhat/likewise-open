#include "includes.h"

static
DWORD
SMBKrb5GetTGTFromKeytab(
    PCSTR  pszUserName,
    PCSTR  pszPassword,
    PCSTR  pszCachePath,
    PDWORD pdwGoodUntilTime
    );

static
DWORD
SMBKrb5DestroyCache(
    PCSTR pszCachePath
    );

static
DWORD
SMBGetServerCanonicalName(
    PCSTR pszServerName,
    PSTR* ppszNormal
    );

static
DWORD
SMBGetServerDomain(
    PCSTR pszServerName,
    PSTR* ppszDomain
    );

static
void
smb_display_status(
    PCSTR     pszId,
    OM_uint32 maj_stat,
    OM_uint32 min_stat
    );

static
void
smb_display_status_1(
    PCSTR     pszId,
    OM_uint32 code,
    int       type
    );

DWORD
SMBKrb5Init(
    PCSTR pszHostname,
    PCSTR pszDomain
    )
{
    DWORD dwError = 0;
    PSTR  pszUsername = NULL;

    if (!gbKrb5Initialized)
    {
        DWORD i = 0;
        DWORD j = 0;
        DWORD dwLength = strlen(pszHostname) + strlen(pszDomain) + 3;

        dwError = SMBAllocateMemory(
                        dwLength,
                        (PVOID*)&pszUsername);
        BAIL_ON_SMB_ERROR(dwError);

        for (i = 0; i < strlen(pszHostname); i++)
        {
            pszUsername[i] = toupper(pszHostname[i]);
        }
        pszUsername[i++] = '$';
        pszUsername[i++] = '@';
        for (; j  < strlen(pszDomain); j++)
        {
             pszUsername[i++] = toupper(pszDomain[j]);
        }

        dwError = SMBKrb5GetTGTFromKeytab(
                        pszUsername,
                        NULL,
                        gpszKrb5CachePath,
                        NULL);
        BAIL_ON_SMB_ERROR(dwError);

        dwError = SMBKrb5SetDefaultCachePath(
                        gpszKrb5CachePath,
                        NULL);
        BAIL_ON_SMB_ERROR(dwError);

        gbKrb5Initialized = TRUE;
    }

cleanup:

    SMB_SAFE_FREE_STRING(pszUsername);

    return dwError;

error:

    goto cleanup;
}

DWORD
SMBKrb5Shutdown(
    VOID
    )
{
    DWORD dwError = 0;

    if (gbKrb5Initialized)
    {
        dwError = SMBKrb5DestroyCache(gpszKrb5CachePath);
        BAIL_ON_SMB_ERROR(dwError);

        gbKrb5Initialized = FALSE;
    }

error:

    return dwError;
}

static
DWORD
SMBKrb5GetTGTFromKeytab(
    PCSTR  pszUserName,
    PCSTR  pszPassword,
    PCSTR  pszCachePath,
    PDWORD pdwGoodUntilTime
    )
{
    DWORD dwError = 0;
    krb5_error_code ret = 0;
    krb5_context ctx = NULL;
    krb5_creds creds = { 0 };
    krb5_ccache cc = NULL;
    krb5_keytab keytab = 0;
    krb5_principal client_principal = NULL;
    krb5_get_init_creds_opt opts;

    ret = krb5_init_context(&ctx);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    ret = krb5_parse_name(ctx, pszUserName, &client_principal);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    /* use krb5_cc_resolve to get an alternate cache */
    ret = krb5_cc_resolve(ctx, pszCachePath, &cc);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    ret = krb5_kt_default(ctx, &keytab);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    krb5_get_init_creds_opt_init(&opts);
    krb5_get_init_creds_opt_set_forwardable(&opts, TRUE);

    ret = krb5_get_init_creds_keytab(
                    ctx,
                    &creds,
                    client_principal,
                    keytab,
                    0,    /* start time     */
                    NULL, /* in_tkt_service */
                    &opts  /* options        */
                    );
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    ret = krb5_cc_initialize(ctx, cc, client_principal);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    ret = krb5_cc_store_cred(ctx, cc, &creds);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    if (pdwGoodUntilTime)
    {
        *pdwGoodUntilTime = creds.times.endtime;
    }

error:

    if (creds.client == client_principal) {
        creds.client = NULL;
    }

    if (ctx) {
        if (client_principal) {
            krb5_free_principal(ctx, client_principal);
        }

        if (keytab) {
            krb5_kt_close(ctx, keytab);
        }

        if (cc) {
            krb5_cc_close(ctx, cc);
        }

        krb5_free_cred_contents(ctx, &creds);

        krb5_free_context(ctx);
    }

    return(dwError);
}

DWORD
SMBKrb5SetDefaultCachePath(
    PCSTR pszCachePath,
    PSTR* ppszOrigCachePath
    )
{
    DWORD dwError       = 0;
    DWORD dwMajorStatus = 0;
    DWORD dwMinorStatus = 0;
    PSTR  pszOrigCachePath = NULL;

    // Set the default for gss
    dwMajorStatus = gss_krb5_ccache_name(
                            (OM_uint32 *)&dwMinorStatus,
                            pszCachePath,
                            (ppszOrigCachePath) ? (const char**)&pszOrigCachePath : NULL);
    BAIL_ON_SEC_ERROR(dwMajorStatus);

    if (ppszOrigCachePath) {
        if (!IsNullOrEmptyString(pszOrigCachePath)) {
            dwError = SMBAllocateString(pszOrigCachePath, ppszOrigCachePath);
            BAIL_ON_SMB_ERROR(dwError);
        } else {
            *ppszOrigCachePath = NULL;
        }
    }

    SMB_LOG_DEBUG("Cache path set to [%s]", SMB_SAFE_LOG_STRING(pszCachePath));

cleanup:

    return dwError;

sec_error:
error:

    if (ppszOrigCachePath) {
        *ppszOrigCachePath = NULL;
    }

    goto cleanup;
}

static
DWORD
SMBKrb5DestroyCache(
    PCSTR pszCachePath
    )
{
    DWORD dwError = 0;
    krb5_error_code ret = 0;
    krb5_context ctx = NULL;
    krb5_ccache cc = NULL;

    ret = krb5_init_context(&ctx);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    /* use krb5_cc_resolve to get an alternate cache */
    ret = krb5_cc_resolve(ctx, pszCachePath, &cc);
    BAIL_ON_SMB_KRB_ERROR(ctx, ret);

    ret = krb5_cc_destroy(ctx, cc);
    if (ret != 0) {
        if (ret != KRB5_FCC_NOFILE) {
            BAIL_ON_SMB_KRB_ERROR(ctx, ret);
        } else {
            ret = 0;
        }
    }

error:

    if (ctx)
    {
       krb5_free_context(ctx);
    }

    return(dwError);
}

DWORD
SMBGSSContextBuild(
    PCSTR     pszServerName,
    PHANDLE   phSMBGSSContext
    )
{
    DWORD dwError = 0;
    DWORD dwMajorStatus = 0;
    DWORD dwMinorStatus = 0;
    PSMB_GSS_SEC_CONTEXT pContext = NULL;
    PSTR pszDomainName = NULL;

    gss_buffer_desc input_name = {0};

    SMB_LOG_DEBUG("Build GSS Context for server [%s]", SMB_SAFE_LOG_STRING(pszServerName));

    dwError = SMBGetServerDomain(pszServerName, &pszDomainName);
    BAIL_ON_SMB_ERROR(dwError);

    SMBStrToUpper(pszDomainName);

    dwError = SMBAllocateMemory(
                    sizeof(SMB_GSS_SEC_CONTEXT),
                    (PVOID*)&pContext);
    BAIL_ON_SMB_ERROR(dwError);

    dwError = SMBAllocateStringPrintf(
                    &pContext->pszTargetName,
                    "cifs/%s@%s",
                    pszServerName,
                    pszDomainName);
    BAIL_ON_SMB_ERROR(dwError);

    pContext->state = SMB_GSS_SEC_CONTEXT_STATE_INITIAL;

    input_name.value = pContext->pszTargetName;
    input_name.length = strlen(pContext->pszTargetName) + 1;

    dwMajorStatus = gss_import_name(
                        (OM_uint32 *)&dwMinorStatus,
                        &input_name,
                        (gss_OID) gss_nt_krb5_name,
                        &pContext->target_name);

    smb_display_status("gss_import_name", dwMajorStatus, dwMinorStatus);

    BAIL_ON_SEC_ERROR(dwMajorStatus);

    dwError = SMBAllocateMemory(
                    sizeof(CtxtHandle),
                    (PVOID*)&pContext->pGSSContext);
    BAIL_ON_SMB_ERROR(dwError);

    *pContext->pGSSContext = GSS_C_NO_CONTEXT;

    *phSMBGSSContext = (HANDLE)pContext;

cleanup:

    SMB_SAFE_FREE_STRING(pszDomainName);

    return dwError;

sec_error:

    dwError = SMB_ERROR_GSS;

error:

    *phSMBGSSContext = NULL;

    if (pContext)
    {
        SMBGSSContextFree(pContext);
    }

    goto cleanup;
}

BOOLEAN
SMBGSSContextNegotiateComplete(
    HANDLE hSMBGSSContext
    )
{
    PSMB_GSS_SEC_CONTEXT pContext = (PSMB_GSS_SEC_CONTEXT)hSMBGSSContext;

    return (pContext->state == SMB_GSS_SEC_CONTEXT_STATE_COMPLETE);
}

DWORD
SMBGSSContextNegotiate(
    HANDLE hSMBGSSContext,
    PBYTE  pSecurityInputBlob,
    DWORD  dwSecurityInputBlobLen,
    PBYTE* ppSecurityBlob,
    PDWORD pdwSecurityBlobLength
    )
{
    DWORD dwError = 0;
    DWORD dwMajorStatus = 0;
    DWORD dwMinorStatus = 0;
    PSMB_GSS_SEC_CONTEXT pContext = (PSMB_GSS_SEC_CONTEXT)hSMBGSSContext;
    gss_buffer_desc input_desc = {0};
    gss_buffer_desc output_desc = {0};
    DWORD ret_flags = 0;
    PBYTE pSecurityBlob = NULL;
    DWORD dwSecurityBlobLength = 0;

    static gss_OID_desc gss_spnego_mech_oid_desc =
      {6, (void *)"\x2b\x06\x01\x05\x05\x02"};

    SMB_LOG_DEBUG("Negotiate GSS Context for target [%s]", pContext->pszTargetName);

    if (pContext->state == SMB_GSS_SEC_CONTEXT_STATE_COMPLETE)
    {
        goto cleanup;
    }

    input_desc.value = pSecurityInputBlob;
    input_desc.length = dwSecurityInputBlobLen;

    dwMajorStatus = gss_init_sec_context(
                        (OM_uint32 *)&dwMinorStatus,
                        NULL,
                        pContext->pGSSContext,
                        pContext->target_name,
                        &gss_spnego_mech_oid_desc,
                        GSS_C_REPLAY_FLAG | GSS_C_MUTUAL_FLAG |
                        GSS_C_SEQUENCE_FLAG | GSS_C_CONF_FLAG |
                        GSS_C_INTEG_FLAG,
                        0,
                        NULL,
                        &input_desc,
                        NULL,
                        &output_desc,
                        &ret_flags,
                        NULL);

    smb_display_status("gss_init_sec_context", dwMajorStatus, dwMinorStatus);

    BAIL_ON_SEC_ERROR(dwMajorStatus);

    switch (dwMajorStatus)
    {
        case GSS_S_CONTINUE_NEEDED:

            pContext->state = SMB_GSS_SEC_CONTEXT_STATE_NEGOTIATE;

            break;

        case GSS_S_COMPLETE:

            pContext->state = SMB_GSS_SEC_CONTEXT_STATE_COMPLETE;

            break;

        default:

            dwError = SMB_ERROR_GSS;
            BAIL_ON_SMB_ERROR(dwError);

            break;
    }

    if (output_desc.length)
    {
        dwError = SMBAllocateMemory(
                        output_desc.length,
                        (PVOID*)&pSecurityBlob);
        BAIL_ON_SMB_ERROR(dwError);

        memcpy(pSecurityBlob, output_desc.value, output_desc.length);

        dwSecurityBlobLength = output_desc.length;
    }

    *ppSecurityBlob = pSecurityBlob;
    *pdwSecurityBlobLength = dwSecurityBlobLength;

cleanup:

    gss_release_buffer(&dwMinorStatus, &output_desc);

    return dwError;

sec_error:

    dwError = SMB_ERROR_GSS;

error:

    *ppSecurityBlob = NULL;
    *pdwSecurityBlobLength = 0;

    SMB_SAFE_FREE_MEMORY(pSecurityBlob);

    goto cleanup;
}

DWORD
SMBGSSContextGetSessionKey(
    HANDLE hSMBGSSContext,
    PBYTE* ppSessionKey,
    PDWORD pdwSessionKeyLength
    )
{
    DWORD dwError = 0;
    DWORD dwMinorStatus = 0;
    PSMB_GSS_SEC_CONTEXT pContext = (PSMB_GSS_SEC_CONTEXT)hSMBGSSContext;
    gss_name_t initiatorName = {0};
    gss_name_t acceptorName = {0};
    gss_buffer_desc sessionKey = GSS_C_EMPTY_BUFFER;
    PBYTE pSessionKey = NULL;

    dwError = gss_inquire_context2(
                    &dwMinorStatus,
                    *pContext->pGSSContext,
                    &initiatorName,
                    &acceptorName,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    &sessionKey);
    smb_display_status("gss_inquire_context2", dwError, dwMinorStatus);
    BAIL_ON_SMB_ERROR(dwError);

    assert(sessionKey.length > 0);

    dwError = SMBAllocateMemory(
                    sessionKey.length * sizeof(BYTE),
                    (PVOID*)&pSessionKey);
    BAIL_ON_SMB_ERROR(dwError);

    memcpy(pSessionKey, sessionKey.value, sessionKey.length);

    *ppSessionKey = pSessionKey;
    *pdwSessionKeyLength = sessionKey.length;

cleanup:

    gss_release_name(&dwMinorStatus, &initiatorName);
    gss_release_name(&dwMinorStatus, &acceptorName);
    gss_release_buffer(&dwMinorStatus, &sessionKey);

    return dwError;

error:

    *ppSessionKey = NULL;
    *pdwSessionKeyLength = 0;

    SMB_SAFE_FREE_MEMORY(pSessionKey);

    goto cleanup;
}

VOID
SMBGSSContextFree(
    HANDLE hSMBGSSContext
    )
{
    DWORD dwMinorStatus = 0;
    PSMB_GSS_SEC_CONTEXT pContext = (PSMB_GSS_SEC_CONTEXT)hSMBGSSContext;

    if (pContext)
    {
        gss_release_name(&dwMinorStatus, &pContext->target_name);

        if (pContext->pGSSContext &&
            (*pContext->pGSSContext != GSS_C_NO_CONTEXT))
        {
            gss_delete_sec_context(
                            &dwMinorStatus,
                            pContext->pGSSContext,
                            GSS_C_NO_BUFFER);

            SMBFreeMemory(pContext->pGSSContext);
        }

        SMB_SAFE_FREE_STRING(pContext->pszTargetName);

        SMBFreeMemory(pContext);
    }
}

static
DWORD
SMBGetServerCanonicalName(
    PCSTR pszServerName,
    PSTR* ppszNormal
    )
{
    DWORD dwError = 0;
    struct addrinfo hints;
    struct addrinfo* pAddrInfo = NULL;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_CANONNAME;

    if (getaddrinfo(pszServerName, NULL, &hints, &pAddrInfo))
    {
        dwError = SMB_ERROR_HOST_NOT_FOUND;
        BAIL_ON_SMB_ERROR(dwError);
    }

    dwError = SMBAllocateString(pAddrInfo->ai_canonname, ppszNormal);
    BAIL_ON_SMB_ERROR(dwError);

error:

    if (pAddrInfo)
    {
        freeaddrinfo(pAddrInfo);
    }

    return dwError;
}

static
DWORD
SMBGetServerDomain(
    PCSTR pszServerName,
    PSTR* ppszDomain
    )
{
    DWORD dwError = 0;
    char* pDot = NULL;
    PSTR pszDomain = NULL;
    PSTR pszNormal = NULL;

    pDot = strchr(pszServerName, '.');

    if (pDot)
    {
        dwError = SMBAllocateString(pszServerName, &pszNormal);
        BAIL_ON_SMB_ERROR(dwError);
    }
    else
    {
        dwError = SMBGetServerCanonicalName(pszServerName, &pszNormal);
        BAIL_ON_SMB_ERROR(dwError);
    }

    dwError = SMBAllocateString(strchr(pszNormal, '.') + 1, &pszDomain);
    BAIL_ON_SMB_ERROR(dwError);

    *ppszDomain = pszDomain;

cleanup:

    SMB_SAFE_FREE_MEMORY(pszNormal);

    return dwError;

error:

    *ppszDomain = NULL;
    SMB_SAFE_FREE_MEMORY(pszDomain);

    goto cleanup;
}


static
void
smb_display_status(
    PCSTR     pszId,
    OM_uint32 maj_stat,
    OM_uint32 min_stat
    )
{
     smb_display_status_1(pszId, maj_stat, GSS_C_GSS_CODE);
     smb_display_status_1(pszId, min_stat, GSS_C_MECH_CODE);
}

static
void
smb_display_status_1(
    PCSTR     pszId,
    OM_uint32 code,
    int       type
    )
{
    OM_uint32 maj_stat, min_stat;
    gss_buffer_desc msg;
    OM_uint32 msg_ctx;

    if ( code == 0 )
    {
        return;
    }

    msg_ctx = 0;
    while (1)
    {
        maj_stat = gss_display_status(&min_stat, code,
                                      type, GSS_C_NULL_OID,
                                      &msg_ctx, &msg);

        switch(code)
        {
#ifdef WIN32
            case SEC_E_OK:
            case SEC_I_CONTINUE_NEEDED:
#else
            case GSS_S_COMPLETE:
            case GSS_S_CONTINUE_NEEDED:
#endif
                SMB_LOG_VERBOSE("GSS-API error calling %s: %d (%s)\n",
                        pszId, code,
                        (char *)msg.value);
                break;

            default:

                SMB_LOG_ERROR("GSS-API error calling %s: %d (%s)\n",
                        pszId, code,
                        (char *)msg.value);
        }

        (void) gss_release_buffer(&min_stat, &msg);

        if (!msg_ctx)
            break;
    }
}
