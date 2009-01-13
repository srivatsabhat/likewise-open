#include "includes.h"

DWORD
EVTParseDays(
    PCSTR  pszTimeInterval,
    PDWORD pdwTimeInterval
    )
{
    DWORD  dwError = 0;
    PSTR   pszTimeIntervalLocal = 0;
    DWORD  dwTimeIntervalLocalLen = 0;
    BOOLEAN  bConvert = FALSE;
    PSTR   pszUnitCode = NULL;

    EVTStripWhitespace(pszTimeIntervalLocal, TRUE, TRUE);

    dwError = EVTAllocateString(
                    pszTimeInterval,
                    &pszTimeIntervalLocal
                    );
    BAIL_ON_EVT_ERROR(dwError);

    dwTimeIntervalLocalLen = strlen(pszTimeIntervalLocal);

    pszUnitCode = pszTimeIntervalLocal + dwTimeIntervalLocalLen - 1;

    if (isdigit((int)(*pszUnitCode)))
    {
        bConvert = TRUE;
    }
    else if(*pszUnitCode == 'd' || *pszUnitCode == 'D')
    {
        bConvert = TRUE;
    }

    if(bConvert) {
        EVTStripWhitespace(pszTimeIntervalLocal, TRUE, TRUE);
	*pdwTimeInterval = (DWORD) atoi(pszTimeIntervalLocal);
    }
    else
    {
	*pdwTimeInterval = 0;
        dwError = EVT_ERROR_INVALID_PARAMETER;
    }

cleanup:

    EVT_SAFE_FREE_STRING(pszTimeIntervalLocal);

    return dwError;

error:

    goto cleanup;
}

DWORD
EVTParseDiskUsage(
    PCSTR  pszDiskUsage,
    PDWORD pdwDiskUsage
    )
{
    DWORD  dwError = 0;
    PSTR   pszDiskUsageLocal = 0;
    DWORD  dwLen = 0;
    DWORD  dwUnitMultiplier = 0;
    PSTR   pszUnitCode = NULL;

    EVTStripWhitespace(pszDiskUsageLocal, TRUE, TRUE);

    dwError = EVTAllocateString(
                    pszDiskUsage,
                    &pszDiskUsageLocal
                    );
    BAIL_ON_EVT_ERROR(dwError);

    dwLen = strlen(pszDiskUsageLocal);

    pszUnitCode = pszDiskUsageLocal + dwLen - 1;

    if (isdigit((int)(*pszUnitCode)))
    {
        dwUnitMultiplier = 1;
    }
    else if(*pszUnitCode == 'k' || *pszUnitCode == 'K')
    {
        dwUnitMultiplier = EVT_BYTES_IN_KB;
    }
    else if(*pszUnitCode == 'm' || *pszUnitCode == 'M')
    {
        dwUnitMultiplier = EVT_BYTES_IN_MB;
    }
    else if(*pszUnitCode == 'g' || *pszUnitCode == 'G')
    {
        dwUnitMultiplier = EVT_BYTES_IN_GB;
    }
    else
    {
        dwError = EVT_ERROR_INVALID_PARAMETER;
        BAIL_ON_EVT_ERROR(dwError);
    }

    *pszUnitCode = ' ';

    EVTStripWhitespace(pszDiskUsageLocal, TRUE, TRUE);

    *pdwDiskUsage = (DWORD) atoi(pszDiskUsageLocal) * dwUnitMultiplier;

cleanup:

    EVT_SAFE_FREE_STRING(pszDiskUsageLocal);

    return dwError;

error:

    goto cleanup;
}

DWORD
EVTParseMaxEntries(
    PCSTR  pszMaxEntries,
    PDWORD pdwMaxEntries
    )
{
    DWORD  dwError = 0;
    PSTR   pszMaxEntriesLocal = 0;
    DWORD  dwLen = 0;
    DWORD  dwUnitMultiplier = 0;
    PSTR   pszUnitCode = NULL;

    EVTStripWhitespace(pszMaxEntriesLocal, TRUE, TRUE);

    dwError = EVTAllocateString(
                    pszMaxEntries,
                    &pszMaxEntriesLocal
                    );
    BAIL_ON_EVT_ERROR(dwError);

    dwLen = strlen(pszMaxEntriesLocal);

    pszUnitCode = pszMaxEntriesLocal + dwLen - 1;

    if (isdigit((int)(*pszUnitCode)))
    {
        dwUnitMultiplier = 1;
    }
    else if(*pszUnitCode == 'k' || *pszUnitCode == 'K')
    {
        dwUnitMultiplier = EVT_RECS_IN_K;
    }
    else if(*pszUnitCode == 'm' || *pszUnitCode == 'M')
    {
        dwUnitMultiplier = EVT_RECS_IN_M;
    }
    else
    {
        dwError = EVT_ERROR_INVALID_PARAMETER;
        BAIL_ON_EVT_ERROR(dwError);
    }

    *pszUnitCode = ' ';

    EVTStripWhitespace(pszMaxEntriesLocal, TRUE, TRUE);

    *pdwMaxEntries = (DWORD) atoi(pszMaxEntriesLocal) * dwUnitMultiplier;

cleanup:

    EVT_SAFE_FREE_STRING(pszMaxEntriesLocal);

    return dwError;

error:

    goto cleanup;
}
