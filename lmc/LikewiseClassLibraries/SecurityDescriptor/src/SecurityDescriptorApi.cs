using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Likewise.LMC.SecurityDesriptor
{
    public class SecurityDescriptorApi
    {
        #region Interop Apis

        private const string LibADVAPIPath = "advapi32.dll";

        // This function uses the DACL to retrieve an array of explicit entries,
        // each of which contains information about individual ACEs within the
        // DACL.
        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        public static extern Int32 GetExplicitEntriesFromAcl(
        IntPtr pacl,
        ref UInt32 pcCountOfExplicitEntries,
        out IntPtr pListOfExplicitEntries);

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto)]
        public static extern uint GetNamedSecurityInfo(
            string pObjectName,
            SE_OBJECT_TYPE ObjectType,
            SECURITY_INFORMATION SecurityInfo,
            out IntPtr pSidOwner,
            out IntPtr pSidGroup,
            out IntPtr pDacl,
            out IntPtr pSacl,
            out IntPtr pSecurityDescriptor);

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto)]
        public static extern bool InitializeSecurityDescriptor(
            IntPtr pSecurityDescriptor,
            uint dwRevision
        );

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool LookupAccountSid(
            string lpSystemName,
            [MarshalAs(UnmanagedType.LPArray)] byte[] Sid,
            System.Text.StringBuilder lpName,
            ref uint cchName,
            System.Text.StringBuilder ReferencedDomainName,
            ref uint cchReferencedDomainName,
            out SID_NAME_USE peUse);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetAclInformation(IntPtr pAcl, ACL_SIZE_INFORMATION pAclInformation, uint nAclInformationLength, ACL_INFORMATION_CLASS dwAclInformationClass);

        [DllImport(LibADVAPIPath)]
        public static extern int GetAce(IntPtr aclPtr, int aceIndex, out IntPtr acePtr);

        [DllImport(LibADVAPIPath)]
        public static extern uint GetLengthSid(IntPtr pSid);

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool ConvertSidToStringSid(
            [MarshalAs(UnmanagedType.LPArray)] byte[] pSID,
            out IntPtr ptrSid);

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool ConvertStringSidToSid(
            [MarshalAs(UnmanagedType.LPWStr)] string sSID,
            out IntPtr ptrSid);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool OpenProcessToken(IntPtr ProcessHandle,
            UInt32 DesiredAccess, out IntPtr TokenHandle);

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool LookupPrivilegeValueW([MarshalAs(UnmanagedType.LPWStr)] string lpSystemName,
            [MarshalAs(UnmanagedType.LPWStr)] string lpName,
            out IntPtr lpLuid);

        // Use this signature if you want the previous state information returned
        [DllImport(LibADVAPIPath, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool AdjustTokenPrivileges(IntPtr TokenHandle,
           [MarshalAs(UnmanagedType.Bool)]bool DisableAllPrivileges,
           TOKEN_PRIVILEGES NewState,
           UInt32 BufferLengthInBytes,
           IntPtr PreviousState,
           UInt32 ReturnLengthInBytes);

        // Use this signature if you do not want the previous state
        [DllImport(LibADVAPIPath, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool AdjustTokenPrivileges(IntPtr TokenHandle,
           [MarshalAs(UnmanagedType.Bool)]bool DisableAllPrivileges,
           ref TOKEN_PRIVILEGES NewState,
           UInt32 Zero,
           IntPtr Null1,
           UInt32 Null2);

        [DllImport("kernel32.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetTokenInformation(
            IntPtr TokenHandle,
            TOKEN_INFORMATION_CLASS TokenInformationClass,
            ref IntPtr TokenInformation,
            uint TokenInformationLength,
            out uint ReturnLength);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetTokenInformation(
            IntPtr TokenHandle,
            TOKEN_INFORMATION_CLASS TokenInformationClass,
            TOKEN_PRIVILEGES TokenInformation,
            uint TokenInformationLength,
            out uint ReturnLength);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetTokenInformation(
            IntPtr TokenHandle,
            TOKEN_INFORMATION_CLASS TokenInformationClass,
            TOKEN_PRIVILEGES TokenInformation,
            uint TokenInformationLength);

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool BuildTrusteeWithSid(
             ref IntPtr pTrustee,
             IntPtr pSid);

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool BuildTrusteeWithName(
             ref IntPtr pTrustee,
             string pSid);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool LogonUser(
            String lpszUsername,
            String lpszDomain,
            String lpszPassword,
            int dwLogonType,
            int dwLogonProvider,
            ref IntPtr phToken
            );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetSecurityDescriptorDacl(
            IntPtr pSecurityDescriptor,
            out bool lpbDaclPresent,
            out IntPtr pDacl,
            out bool lpbDaclDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetSecurityDescriptorOwner(
            IntPtr pSecurityDescriptor,
            out IntPtr pOwner,
            out bool lpbOwnerDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetSecurityDescriptorGroup(
            IntPtr pSecurityDescriptor,
            out IntPtr pGroup,
            out bool lpbGroupDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetSecurityDescriptorControl(
            IntPtr pSecurityDescriptor,
            out SECURITY_DESCRIPTOR_CONTROL pControl,
            out uint lpdwRevision
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern byte GetSecurityDescriptorLength(
            IntPtr pSecurityDescriptor);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool AddAce(
            IntPtr pAcl,
            byte dwAceRevision,
            int dwStartingAceIndex,
            IntPtr pAceList,
            uint nAceListLength
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool DeleteAce(
            IntPtr pAcl,
            int dwAceIndex
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetSecurityDescriptorDacl(
            IntPtr pSecurityDescriptor,
            bool bDaclPresent,
            IntPtr pDacl,
            bool bDaclDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetSecurityDescriptorControl(
            IntPtr pSecurityDescriptor,
            uint ControlBitsOfInterest,
            uint ControlBitsToSet
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool GetSecurityDescriptorControl(
            IntPtr pSecurityDescriptor,
            out uint pControl,
            out uint lpdwRevision
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetSecurityDescriptorGroup(
            IntPtr pSecurityDescriptor,
            IntPtr pGroup,
            bool bGroupDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetSecurityDescriptorOwner(
            IntPtr pSecurityDescriptor,
            IntPtr pOwner,
            bool bOwnerDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetSecurityDescriptorOwner(
            IntPtr pSecurityDescriptor,
            IntPtr RMControl
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetSecurityDescriptorSacl(
            IntPtr pSecurityDescriptor,
            bool bSaclPresent,
            IntPtr pSacl,
            bool bSaclDefaulted
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern void FreeSid(
            IntPtr pSid
        );

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern uint SetEntriesInAcl(
        ulong cCountOfExplicitEntries,
        EXPLICIT_ACCESS[] pListOfExplicitEntries,
        LwACL OldAcl,
        ref LwACL NewAcl
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern uint SetNamedSecurityInfo(
            string pObjectName,
            SE_OBJECT_TYPE ObjectType,
            SECURITY_INFORMATION SecurityInfo,
            IntPtr psidOwner,
            IntPtr psidGroup,
            IntPtr pDacl,
            IntPtr pSacl
        );

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern uint BuildExplicitAccessWithName(
            out IntPtr pExplicitAccess,
            string pTrusteeName,
            uint AccessPermissions,
            ACCESS_MODE AccessMode,
            uint Inheritance
        );

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool MakeAbsoluteSD(
            IntPtr pSelfRelativeSD,
            out IntPtr pAbsoluteSD,
            ref uint lpdwAbsoluteSDSize,
            out IntPtr pDacl,
            ref uint lpdwDaclSize,
            out IntPtr pSacl,
            ref uint lpdwSaclSize,
            out IntPtr pOwner,
            ref uint lpdwOwnerSize,
            out IntPtr pPrimaryGroup,
            ref uint lpdwPrimaryGroupSize
        );

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool MakeSelfRelativeSD(
            IntPtr pAbsoluteSD,
            out IntPtr pSelfRelativeSD,
            ref uint lpdwBufferLength
        );

        [DllImport("Kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern uint LocalFree(
        IntPtr phMem
        );

        [DllImport("Kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern IntPtr GetCurrentThread();

        [DllImport("kernel32.dll")]
        public static extern IntPtr OpenThread(ThreadAccess dwDesiredAccess, bool bInheritHandle,
           uint dwThreadId);

        [DllImport("kernel32.dll")]
        public static extern uint GetCurrentProcessId();

        [DllImport("kernel32.dll")]
        public static extern uint GetCurrentThreadId();

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool SetThreadToken(
        IntPtr Thread,
        IntPtr Token
        );

        [DllImport(LibADVAPIPath, SetLastError = true, CharSet = CharSet.Auto)]
        public static extern bool CopySid(
        uint nDestinationSidLength,
        out IntPtr pDestinationSid,
        IntPtr pSourceSid
        );

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool OpenThreadToken(
            IntPtr ThreadHandle,
            uint DesiredAccess,
            bool OpenAsSelf,
            out IntPtr TokenHandle);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool InitializeAcl(IntPtr pAcl, int nAclLength, uint dwAclRevision);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool IsValidAcl(IntPtr pAcl);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool IsValidSecurityDescriptor(IntPtr pSecurityDescriptor);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool AddAccessAllowedAce(ref IntPtr pAcl, uint dwAceRevision, int AccessMask, IntPtr pSid);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool AddAccessAllowedAceEx(IntPtr pAcl, int dwAceRevision, byte AceFlags, int AccessMask, IntPtr pSid);

        [DllImport(LibADVAPIPath, SetLastError = true)]
        public static extern bool SetAclInformation(
            IntPtr pAcl,
            IntPtr pAclInformation,
            uint nAclInformationLength,
            ACL_INFORMATION_CLASS dwAclInformationClass);

        #region CSP (cryptographic service provider) Apis

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CryptAcquireContext(ref IntPtr hProv, string pszContainer,
        string pszProvider, uint dwProvType, uint dwFlags);

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CryptGenKey(IntPtr hProv, uint Algid, uint dwFlags, ref IntPtr phKey);

        [DllImport(LibADVAPIPath, CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool CryptDestroyKey(IntPtr phKey);

        #endregion

        #endregion

        #region Class Data

        #region  definitions for CryptAcquireContext
        // dwFlags
        public static UInt32 CRYPT_VERIFYCONTEXT = 0xF0000000;
        public static UInt32 CRYPT_NEWKEYSET = 0x00000008;
        public static UInt32 CRYPT_DELETEKEYSET = 0x00000010;
        public static UInt32 CRYPT_MACHINE_KEYSET = 0x00000020;
        public static UInt32 CRYPT_SILENT = 0x00000040;
        public static string MS_STRONG_PROV = "Microsoft Strong Cryptographic Provider";

        public static UInt32 PROV_RSA_FULL = 1;
        public static UInt32 PROV_RSA_SIG = 2;
        public static UInt32 PROV_DSS = 3;
        public static UInt32 PROV_FORTEZZA = 4;
        public static UInt32 PROV_MS_EXCHANGE = 5;
        public static UInt32 PROV_SSL = 6;
        public static UInt32 PROV_RSA_SCHANNEL = 12;
        public static UInt32 PROV_DSS_DH = 13;
        public static UInt32 PROV_EC_ECDSA_SIG = 14;
        public static UInt32 PROV_EC_ECNRA_SIG = 15;
        public static UInt32 PROV_EC_ECDSA_FULL = 16;
        public static UInt32 PROV_DH_SCHANNEL = 18;
        public static UInt32 PROV_SPYRUS_LYNKS = 20;
        public static UInt32 PROV_RNG = 21;
        public static UInt32 PROV_INTEL_SEC = 22;
        public static UInt32 PROV_REPLACE_OWF = 23;
        public static UInt32 PROV_RSA_AES = 24;

        public static UInt32 AT_KEYEXCHANGE = 1;
        public static UInt32 AT_SIGNATURE = 2;

        #endregion

        public static UInt32 SECURITY_DESCRIPTOR_REVISION = 1;

        //Use these for DesiredAccess
        public static UInt32 STANDARD_RIGHTS_REQUIRED = 0x000F0000;
        public static UInt32 STANDARD_RIGHTS_READ = 0x00020000;
        public static UInt32 TOKEN_ASSIGN_PRIMARY = 0x0001;
        public static UInt32 TOKEN_DUPLICATE = 0x0002;
        public static UInt32 TOKEN_IMPERSONATE = 0x0004;
        public static UInt32 TOKEN_QUERY = 0x0008;
        public static UInt32 TOKEN_QUERY_SOURCE = 0x0010;
        public static UInt32 TOKEN_ADJUST_PRIVILEGES = 0x0020;
        public static UInt32 TOKEN_ADJUST_GROUPS = 0x0040;
        public static UInt32 TOKEN_ADJUST_DEFAULT = 0x0080;
        public static UInt32 TOKEN_ADJUST_SESSIONID = 0x0100;
        public static UInt32 TOKEN_READ = (STANDARD_RIGHTS_READ | TOKEN_QUERY);
        public static UInt32 TOKEN_ALL_ACCESS = (STANDARD_RIGHTS_REQUIRED | TOKEN_ASSIGN_PRIMARY |
            TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_QUERY_SOURCE |
            TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_GROUPS | TOKEN_ADJUST_DEFAULT |
            TOKEN_ADJUST_SESSIONID);

        //Token privilege constants
        public const UInt32 SE_PRIVILEGE_ENABLED_BY_DEFAULT = 0x00000001;
        public const UInt32 SE_PRIVILEGE_ENABLED = 0x00000002;
        public const UInt32 SE_PRIVILEGE_REMOVED = 0x00000004;
        public const UInt32 SE_PRIVILEGE_USED_FOR_ACCESS = 0x80000000;
        public const UInt32 PRIVILEGE_SET_ALL_NECESSARY = 1;


        public const string SE_ASSIGNPRIMARYTOKEN_NAME = "SeAssignPrimaryTokenPrivilege";
        public const string SE_AUDIT_NAME = "SeAuditPrivilege";
        public const string SE_BACKUP_NAME = "SeBackupPrivilege";
        public const string SE_CHANGE_NOTIFY_NAME = "SeChangeNotifyPrivilege";
        public const string SE_CREATE_GLOBAL_NAME = "SeCreateGlobalPrivilege";
        public const string SE_CREATE_PAGEFILE_NAME = "SeCreatePagefilePrivilege";
        public const string SE_CREATE_PERMANENT_NAME = "SeCreatePermanentPrivilege";
        public const string SE_CREATE_SYMBOLIC_LINK_NAME = "SeCreateSymbolicLinkPrivilege";
        public const string SE_CREATE_TOKEN_NAME = "SeCreateTokenPrivilege";
        public const string SE_DEBUG_NAME = "SeDebugPrivilege";
        public const string SE_ENABLE_DELEGATION_NAME = "SeEnableDelegationPrivilege";
        public const string SE_IMPERSONATE_NAME = "SeImpersonatePrivilege";
        public const string SE_INC_BASE_PRIORITY_NAME = "SeIncreaseBasePriorityPrivilege";
        public const string SE_INCREASE_QUOTA_NAME = "SeIncreaseQuotaPrivilege";
        public const string SE_INC_WORKING_SET_NAME = "SeIncreaseWorkingSetPrivilege";
        public const string SE_LOAD_DRIVER_NAME = "SeLoadDriverPrivilege";
        public const string SE_LOCK_MEMORY_NAME = "SeLockMemoryPrivilege";
        public const string SE_MACHINE_ACCOUNT_NAME = "SeMachineAccountPrivilege";
        public const string SE_MANAGE_VOLUME_NAME = "SeManageVolumePrivilege";
        public const string SE_PROF_SINGLE_PROCESS_NAME = "SeProfileSingleProcessPrivilege";
        public const string SE_RELABEL_NAME = "SeRelabelPrivilege";
        public const string SE_REMOTE_SHUTDOWN_NAME = "SeRemoteShutdownPrivilege";
        public const string SE_RESTORE_NAME = "SeRestorePrivilege";
        public const string SE_SECURITY_NAME = "SeSecurityPrivilege";
        public const string SE_SHUTDOWN_NAME = "SeShutdownPrivilege";
        public const string SE_SYNC_AGENT_NAME = "SeSyncAgentPrivilege";
        public const string SE_SYSTEM_ENVIRONMENT_NAME = "SeSystemEnvironmentPrivilege";
        public const string SE_SYSTEM_PROFILE_NAME = "SeSystemProfilePrivilege";
        public const string SE_SYSTEMTIME_NAME = "SeSystemtimePrivilege";
        public const string SE_TAKE_OWNERSHIP_NAME = "SeTakeOwnershipPrivilege";
        public const string SE_TCB_NAME = "SeTcbPrivilege";
        public const string SE_TIME_ZONE_NAME = "SeTimeZonePrivilege";
        public const string SE_TRUSTED_CREDMAN_ACCESS_NAME = "SeTrustedCredManAccessPrivilege";
        public const string SE_UNDOCK_NAME = "SeUndockPrivilege";
        public const string SE_UNSOLICITED_INPUT_NAME = "SeUnsolicitedInputPrivilege";

        #endregion

        #region Structures

        [StructLayout(LayoutKind.Sequential)]
        public struct PRIVILEGE_SET
        {
            public long PrivilegeCount;
            public long Control;
            public LUID_AND_ATTRIBUTES[] Privileges;

        }

        [StructLayout(LayoutKind.Sequential)]
        public struct TOKEN_PRIVILEGES
        {
            public UInt32 PrivilegeCount;
            //[MarshalAs(UnmanagedType.ByValArray)]
            //public LUID_AND_ATTRIBUTES[] Privileges;
            public IntPtr Luid;
            public int Attributes;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public struct LwLUID
        {
            public Int32 LowPart;
            public Int32 HighPart;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        public struct LUID_AND_ATTRIBUTES
        {
            //public LwLUID Luid;
            public IntPtr Luid;
            public UInt32 Attributes;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto, Pack = 4)]
        public struct TRUSTEE
        {
            public IntPtr pMultipleTrustee; // must be null
            public UInt32 MultipleTrusteeOperation;
            public UInt32 TrusteeForm;
            public UInt32 TrusteeType;
            [MarshalAs(UnmanagedType.LPStr)]
            public IntPtr ptstrName;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SECURITY_DESCRIPTOR
        {
            public byte revision;
            public byte size;
            public short control;
            public IntPtr owner;
            public IntPtr group;
            public IntPtr sacl;
            public IntPtr dacl;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ACE
        {
            public uint AccessMask;
            public uint AceFlags;
            public uint AceType;
            [MarshalAs(UnmanagedType.LPStr)]
            public string GuidInheritedObjectType;
            [MarshalAs(UnmanagedType.LPStr)]
            public string GuidObjectType;
            [MarshalAs(UnmanagedType.LPStr)]
            public string Trustee;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct SID
        {
            byte Revision;
            byte SubAuthorityCount;
            SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
            //[MarshalAs(UnmanagedType.ByValArray, SizeConst = ANYSIZE_ARRAY)]
            IntPtr SubAuthority;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct LwACL
        {
            public byte AclRevision;
            public byte Sbz1; // Padding (should be 0)
            public short AclSize;
            public short AceCount;
            public short Sbz2; // Padding (should be 0)
        }

        [StructLayout(LayoutKind.Sequential, Size = 32)]
        public struct EXPLICIT_ACCESS
        {
            public UInt32 grfAccessPermissions;
            public UInt32 grfAccessMode;
            public UInt32 grfInheritance;
            public TRUSTEE Trustee;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ACL_REVISION_INFORMATION
        {
            public uint AclRevision;
        }

        [StructLayout(LayoutKind.Sequential)]
        public class ACL_SIZE_INFORMATION
        {
            public uint AceCount;
            public uint AclBytesInUse;
            public uint AclBytesFree;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct ACE_HEADER
        {
            public byte AceType;
            public byte AceFlags;
            public short AceSize;
        }

        [StructLayout(LayoutKind.Sequential, Size = 36)]
        public struct ACCESS_ALLOWED_ACE
        {
            public ACE_HEADER Header;
            public int Mask;
            public IntPtr SidStart;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SECURITY_ATTRIBUTES
        {
            public int nLength;
            public unsafe byte* lpSecurityDescriptor;
            public int bInheritHandle;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SID_IDENTIFIER_AUTHORITY
        {
            byte[] Value;
        }

        #endregion

        #region Enums

        public enum SECURITY_DESCRIPTOR_CONTROL : uint
        {
            SE_OWNER_DEFAULTED = 0x1, //1
            SE_GROUP_DEFAULTED = 0x2, //2
            SE_DACL_PRESENT = 0x4,//4
            SE_DACL_DEFAULTED = 0x8,//8
            SE_SACL_PRESENT = 0x10, //16
            SE_SACL_DEFAULTED = 0x20, //32
            SE_DACL_AUTO_INHERIT_REQ = 0x100, //256
            SE_SACL_AUTO_INHERIT_REQ = 0x200, //512
            SE_DACL_AUTO_INHERITED = 0x400, //1024
            SE_SACL_AUTO_INHERITED = 0x800, //2048
            SE_DACL_PROTECTED = 0x1000,//4096
            SE_SACL_PROTECTED = 0x2000, //8192
            SE_SELF_RELATIVE = 0x8000, //32768
        }

        public enum TRUSTEE_FORM : uint
        {
            TRUSTEE_IS_SID,
            TRUSTEE_IS_NAME,
            TRUSTEE_BAD_FORM,
            TRUSTEE_IS_OBJECTS_AND_SID,
            TRUSTEE_IS_OBJECTS_AND_NAME
        }

        public enum TRUSTEE_TYPE : uint
        {
            TRUSTEE_IS_UNKNOWN,
            TRUSTEE_IS_USER,
            TRUSTEE_IS_GROUP,
            TRUSTEE_IS_DOMAIN,
            TRUSTEE_IS_ALIAS,
            TRUSTEE_IS_WELL_KNOWN_GROUP,
            TRUSTEE_IS_DELETED,
            TRUSTEE_IS_INVALID,
            TRUSTEE_IS_COMPUTER
        }

        public enum MULTIPLE_TRUSTEE_OPERATION :uint
        {
            NO_MULTIPLE_TRUSTEE,
            TRUSTEE_IS_IMPERSONATE
        }

        public enum ACL_INFORMATION_CLASS
        {
            AclRevisionInformation = 1,
            AclSizeInformation
        }

        public enum ACCESS_MODE : uint
        {
            NOT_USED_ACCESS = 0,
            GRANT_ACCESS,
            SET_ACCESS,
            DENY_ACCESS,
            REVOKE_ACCESS,
            SET_AUDIT_SUCCESS,
            SET_AUDIT_FAILURE
        }

        public enum SID_NAME_USE
        {
            SidTypeUser = 1,
            SidTypeGroup,
            SidTypeDomain,
            SidTypeAlias,
            SidTypeWellKnownGroup,
            SidTypeDeletedAccount,
            SidTypeInvalid,
            SidTypeUnknown,
            SidTypeComputer
        }

        public enum SE_OBJECT_TYPE : uint
        {
            SE_UNKNOWN_OBJECT_TYPE = 0,
            SE_FILE_OBJECT,
            SE_SERVICE,
            SE_PRINTER,
            SE_REGISTRY_KEY,
            SE_LMSHARE,
            SE_KERNEL_OBJECT,
            SE_WINDOW_OBJECT,
            SE_DS_OBJECT,
            SE_DS_OBJECT_ALL,
            SE_PROVIDER_DEFINED_OBJECT,
            SE_WMIGUID_OBJECT,
            SE_REGISTRY_WOW64_32KEY,
            SE_DIR_OBJECT
        }

        [Flags]
        public enum SECURITY_INFORMATION : uint
        {
            OWNER_SECURITY_INFORMATION = 0x00000001,
            GROUP_SECURITY_INFORMATION = 0x00000002,
            DACL_SECURITY_INFORMATION = 0x00000004,
            SACL_SECURITY_INFORMATION = 0x00000008,
            UNPROTECTED_SACL_SECURITY_INFORMATION = 0x10000000,
            UNPROTECTED_DACL_SECURITY_INFORMATION = 0x20000000,
            PROTECTED_SACL_SECURITY_INFORMATION = 0x40000000,
            PROTECTED_DACL_SECURITY_INFORMATION = 0x80000000
        }

        public enum AccessTypes : uint
        {
            Allow = 0,
            Deny = 1,
            Audit = 2,
            Allow_Object = 5,
            Deny_Object = 6,
            Audit_Object = 7
        }

        public enum ACEFlags : uint
        {
            //Non-container child objects inherit the ACE as an effective ACE.
            OBJECT_INHERIT_ACE = 0x1,

            //The ACE has an effect on child namespaces as well as the current namespace.
            CONTAINER_INHERIT_ACE = 0x2,

            //The ACE applies only to the current namespace and immediate children .
            NO_PROPAGATE_INHERIT_ACE = 0x4,

            //The ACE applies only to child namespaces.
            INHERIT_ONLY_ACE = 0x8,

            //This is not set by clients, but is reported to clients when the source of an ACE is a parent namespace.
            INHERITED_ACE = 0x10,

            VALID_INHERIT_FLAGS = 0x1F,

            SUCCESSFUL_ACCESS = 0x40,

            FAILED_ACCESS = 0x80
        }

        public enum AccessRights : uint
        {
            //Enables the account and grants the user read permissions. This is a default access right for all users and corresponds to the
            //Enable Account permission on the Security tab of the WMI Control. For more information, see Setting Namespace Security with the WMI Control.
            WBEM_ENABLE = 0x1,

            //Allows the execution of methods. Providers can perform additional access checks.
            //This is a default access right for all users and corresponds to the Execute Methods permission on the Security tab of the WMI Control.
            WBEM_METHOD_EXECUTE = 0x2,

            //Allows a user account to write to classes in the WMI repository as well as instances.
            //A user cannot write to system classes. Only members of the Administrators group have this permission.
            //BEM_FULL_WRITE_REP corresponds to the Full Write permission on the Security tab of the WMI Control.
            WBEM_FULL_WRITE_REP = 0x4,

            //Allows you to write data to instances only, not classes.
            //A user cannot write classes to the WMI repository. Only members of the Administrators group have this right.
            //WBEM_PARTIAL_WRITE_REP corresponds to the Partial Write permission on the Security tab of the WMI Control.
            WBEM_PARTIAL_WRITE_REP = 0x8,

            //Allows writing classes and instances to providers.
            //Note that providers can do additional access checks when impersonating a user.
            //This is a default access right for all users and corresponds to the Provider Write permission on the Security tab of the WMI Control.
            WBEM_WRITE_PROVIDER = 0x10,

            //Allows a user account to remotely perform any operations allowed by the permissions described above.
            //Only members of the Administrators group have this right.
            //WBEM_REMOTE_ACCESS corresponds to the Remote Enable permission on the Security tab of the WMI Control.
            WBEM_REMOTE_ACCESS = 0x20,

            //Allows you to read the namespace security descriptor.
            //Only members of the Administrators group have this right.
            //READ_CONTROL corresponds to the Read Security permission on the Security tab of the WMI Control.
            READ_CONTROL = 0x20000,

            //Allows you to change the discretionary access control list (DACL) on the namespace.
            //Only members of the Administrators group have this right.
            //WRITE_DAC corresponds to the Edit Security permission on the Security tab of the WMI Control.
            WRITE_DAC = 0x40000
        }

        //Event Security Constants
        public enum EventSecurityConstants
        {
            //Specifies that the account can publish events to the instance of __EventFilter that defines the event filter for a permanent consumer.
            WBEM_RIGHT_PUBLISH = 0x80,

            //Specifies that a consumer can subscribe to the events delivered to a sink. Used in IWbemEventSink::SetSinkSecurity.
            WBEM_RIGHT_SUBSCRIBE = 0x40,

            //Event provider indicates that WMI checks the SECURITY_DESCRIPTOR property in each event (inherited from __Event),
            //and only sends events to consumers with the appropriate access permissions.
            WBEM_S_SUBJECT_TO_SDS = 0x43003
        }

        public enum TOKEN_INFORMATION_CLASS
        {

            /// <summary>
            /// The buffer receives a TOKEN_USER structure that contains the user account of the token.
            /// </summary>
            TokenUser = 1,

            /// <summary>
            /// The buffer receives a TOKEN_GROUPS structure that contains the group accounts associated with the token.
            /// </summary>
            TokenGroups,

            /// <summary>
            /// The buffer receives a TOKEN_PRIVILEGES structure that contains the privileges of the token.
            /// </summary>
            TokenPrivileges,

            /// <summary>
            /// The buffer receives a TOKEN_OWNER structure that contains the default owner security identifier (SID) for newly created objects.
            /// </summary>
            TokenOwner,

            /// <summary>
            /// The buffer receives a TOKEN_PRIMARY_GROUP structure that contains the default primary group SID for newly created objects.
            /// </summary>
            TokenPrimaryGroup,

            /// <summary>
            /// The buffer receives a TOKEN_DEFAULT_DACL structure that contains the default DACL for newly created objects.
            /// </summary>
            TokenDefaultDacl,

            /// <summary>
            /// The buffer receives a TOKEN_SOURCE structure that contains the source of the token. TOKEN_QUERY_SOURCE access is needed to retrieve this information.
            /// </summary>
            TokenSource,

            /// <summary>
            /// The buffer receives a TOKEN_TYPE value that indicates whether the token is a primary or impersonation token.
            /// </summary>
            TokenType,

            /// <summary>
            /// The buffer receives a SECURITY_IMPERSONATION_LEVEL value that indicates the impersonation level of the token. If the access token is not an impersonation token, the function fails.
            /// </summary>
            TokenImpersonationLevel,

            /// <summary>
            /// The buffer receives a TOKEN_STATISTICS structure that contains various token statistics.
            /// </summary>
            TokenStatistics,

            /// <summary>
            /// The buffer receives a TOKEN_GROUPS structure that contains the list of restricting SIDs in a restricted token.
            /// </summary>
            TokenRestrictedSids,

            /// <summary>
            /// The buffer receives a DWORD value that indicates the Terminal Services session identifier that is associated with the token.
            /// </summary>
            TokenSessionId,

            /// <summary>
            /// The buffer receives a TOKEN_GROUPS_AND_PRIVILEGES structure that contains the user SID, the group accounts, the restricted SIDs, and the authentication ID associated with the token.
            /// </summary>
            TokenGroupsAndPrivileges,

            /// <summary>
            /// Reserved.
            /// </summary>
            TokenSessionReference,

            /// <summary>
            /// The buffer receives a DWORD value that is nonzero if the token includes the SANDBOX_INERT flag.
            /// </summary>
            TokenSandBoxInert,

            /// <summary>
            /// Reserved.
            /// </summary>
            TokenAuditPolicy,

            /// <summary>
            /// The buffer receives a TOKEN_ORIGIN value.
            /// </summary>
            TokenOrigin,

            /// <summary>
            /// The buffer receives a TOKEN_ELEVATION_TYPE value that specifies the elevation level of the token.
            /// </summary>
            TokenElevationType,

            /// <summary>
            /// The buffer receives a TOKEN_LINKED_TOKEN structure that contains a handle to another token that is linked to this token.
            /// </summary>
            TokenLinkedToken,

            /// <summary>
            /// The buffer receives a TOKEN_ELEVATION structure that specifies whether the token is elevated.
            /// </summary>
            TokenElevation,

            /// <summary>
            /// The buffer receives a DWORD value that is nonzero if the token has ever been filtered.
            /// </summary>
            TokenHasRestrictions,

            /// <summary>
            /// The buffer receives a TOKEN_ACCESS_INFORMATION structure that specifies security information contained in the token.
            /// </summary>
            TokenAccessInformation,

            /// <summary>
            /// The buffer receives a DWORD value that is nonzero if virtualization is allowed for the token.
            /// </summary>
            TokenVirtualizationAllowed,

            /// <summary>
            /// The buffer receives a DWORD value that is nonzero if virtualization is enabled for the token.
            /// </summary>
            TokenVirtualizationEnabled,

            /// <summary>
            /// The buffer receives a TOKEN_MANDATORY_LABEL structure that specifies the token's integrity level.
            /// </summary>
            TokenIntegrityLevel,

            /// <summary>
            /// The buffer receives a DWORD value that is nonzero if the token has the UIAccess flag set.
            /// </summary>
            TokenUIAccess,

            /// <summary>
            /// The buffer receives a TOKEN_MANDATORY_POLICY structure that specifies the token's mandatory integrity policy.
            /// </summary>
            TokenMandatoryPolicy,

            /// <summary>
            /// The buffer receives the token's logon security identifier (SID).
            /// </summary>
            TokenLogonSid,

            /// <summary>
            /// The maximum value for this enumeration
            /// </summary>
            MaxTokenInfoClass

        }

        public enum LogonType : int
        {
            //'This logon type is intended for batch servers, where processes may be executing on behalf of a user without
            //'their direct intervention. This type is also for higher performance servers that process many plaintext
            //'authentication attempts at a time, such as mail or Web servers.
            //'The LogonUser function does not cache credentials for this logon type.
            LOGON32_LOGON_BATCH = 4,

            //'This logon type is intended for users who will be interactively using the computer, such as a user being logged on
            //'by a terminal server, remote shell, or similar process.
            //'This logon type has the additional expense of caching logon information for disconnected operations;
            //'therefore, it is inappropriate for some client/server applications,
            //'such as a mail server.
            LOGON32_LOGON_INTERACTIVE = 2,

            //'This logon type is intended for high performance servers to authenticate plaintext passwords.
            //'The LogonUser function does not cache credentials for this logon type.
            LOGON32_LOGON_NETWORK = 3,

            //'This logon type preserves the name and password in the authentication package, which allows the server to make
            //'connections to other network servers while impersonating the client. A server can accept plaintext credentials
            //'from a client, call LogonUser, verify that the user can access the system across the network, and still
            //'communicate with other servers.
            //'NOTE: Windows NT:  This value is not supported.
            LOGON32_LOGON_NETWORK_CLEARTEXT = 8,

            //'This logon type allows the caller to clone its current token and specify new credentials for outbound connections.
            //'The new logon session has the same local identifier but uses different credentials for other network connections.
            //'NOTE: This logon type is supported only by the LOGON32_PROVIDER_WINNT50 logon provider.
            //'NOTE: Windows NT:  This value is not supported.
            LOGON32_LOGON_NEW_CREDENTIALS = 9,

            //'Indicates a service-type logon. The account provided must have the service privilege enabled.
            LOGON32_LOGON_SERVICE = 5,

            //'This logon type is for GINA DLLs that log on users who will be interactively using the computer.
            //'This logon type can generate a unique audit record that shows when the workstation was unlocked.
            LOGON32_LOGON_UNLOCK = 7
        }

        public enum LogonProvider : int
        {
            /// <summary>
            /// Use the standard logon provider for the system.
            /// The default security provider is negotiate, unless you pass NULL for the domain name and the user name
            /// is not in UPN format. In this case, the default provider is NTLM.
            /// NOTE: Windows 2000/NT:   The default security provider is NTLM.
            /// </summary>
            LOGON32_PROVIDER_DEFAULT = 0,
            LOGON32_PROVIDER_WINNT50,
            LOGON32_PROVIDER_WINNT40
        }

        [Flags]
        public enum ThreadAccess : int
        {
            TERMINATE = (0x0001),
            SUSPEND_RESUME = (0x0002),
            GET_CONTEXT = (0x0008),
            SET_CONTEXT = (0x0010),
            SET_INFORMATION = (0x0020),
            QUERY_INFORMATION = (0x0040),
            SET_THREAD_TOKEN = (0x0080),
            IMPERSONATE = (0x0100),
            DIRECT_IMPERSONATION = (0x0200),
            ALL_ACCESS = TERMINATE | SUSPEND_RESUME | GET_CONTEXT | SET_CONTEXT |
                         SET_INFORMATION | QUERY_INFORMATION | SET_THREAD_TOKEN |
                         IMPERSONATE | DIRECT_IMPERSONATION
        }

        #endregion
    }

    /// <summary>
    /// Well-known SIDs identify generic groups and generic users.
    /// </summary>
    public class Well_Known_SID
    {

        #region SID strings for Well-Known SIDs

        //Account operators.  The corresponding RID is DOMAIN_ALIAS_RID_ACCOUNT_OPS.
        public static string SDDL_ACCOUNT_OPERATORS = "AO";

        //lias to grant permissions to accounts that use applications compatible with Windows NT 4.0 operating systems.
        //The corresponding RID is DOMAIN_ALIAS_RID_PREW2KCOMPACCESS.
        public static string SDDL_ALIAS_PREW2KCOMPACC = "RU";

        //Anonymous logon.  The corresponding RID is SECURITY_ANONYMOUS_LOGON_RID.
        public static string SDDL_ANONYMOUS = "AN";

        //Authenticated users.  The corresponding RID is SECURITY_AUTHENTICATED_USER_RID.
        public static string SDDL_AUTHENTICATED_USERS = "AU";

        //Built-in administrators.  The corresponding RID is DOMAIN_ALIAS_RID_ADMINS.
        public static string SDDL_BUILTIN_ADMINISTRATORS = "BA";

        //Built-in guests.  The corresponding RID is DOMAIN_ALIAS_RID_GUESTS.
        public static string SDDL_BUILTIN_GUESTS = "BG";

        //Backup operators.  The corresponding RID is DOMAIN_ALIAS_RID_BACKUP_OPS.
        public static string SDDL_BACKUP_OPERATORS = "BO";

        //Built-in users.  The corresponding RID is DOMAIN_ALIAS_RID_USERS.
        public static string SDDL_BUILTIN_USERS = "BU";

        //Certificate publishers.  The corresponding RID is DOMAIN_GROUP_RID_CERT_ADMINS.
        public static string SDDL_CERT_SERV_ADMINISTRATORS = "CA";

        //Creator group.  The corresponding RID is SECURITY_CREATOR_GROUP_RID.
        public static string SDDL_CREATOR_GROUP = "CG";

        //Creator owner.  The corresponding RID is SECURITY_CREATOR_OWNER_RID.
        public static string SDDL_CREATOR_OWNER = "CO";

        //Domain administrators.  The corresponding RID is DOMAIN_GROUP_RID_ADMINS.
        public static string SDDL_DOMAIN_ADMINISTRATORS = "DA";

        //Domain computers.  The corresponding RID is DOMAIN_GROUP_RID_COMPUTERS.s
        public static string SDDL_DOMAIN_COMPUTERS = "DC";

        //Domain controllers.  The corresponding RID is DOMAIN_GROUP_RID_CONTROLLERS.
        public static string SDDL_DOMAIN_DOMAIN_CONTROLLERS = "DD";

        //Domain guests.  The corresponding RID is DOMAIN_GROUP_RID_GUESTS.
        public static string SDDL_DOMAIN_GUESTS = "DG";

        //Domain users.  The corresponding RID is DOMAIN_GROUP_RID_USERS.
        public static string SDDL_DOMAIN_USERS = "DU";

        //Enterprise administrators.  The corresponding RID is DOMAIN_GROUP_RID_ENTERPRISE_ADMINS.
        public static string SDDL_ENTERPRISE_ADMINS = "EA";

        //Enterprise domain controllers.  The corresponding RID is SECURITY_SERVER_LOGON_RID.
        public static string SDDL_ENTERPRISE_DOMAIN_CONTROLLERS = "ED";

        //Everyone.  The corresponding RID is SECURITY_WORLD_RID.
        public static string SDDL_EVERYONE = "WD";

        //Group Policy administrators.  The corresponding RID is DOMAIN_GROUP_RID_POLICY_ADMINS.
        public static string SDDL_GROUP_POLICY_ADMINS = "PA";

        //Interactively logged-on user.  This is a group identifier added to the token of a process when it was logged on interactively.
        //The corresponding logon type is LOGON32_LOGON_INTERACTIVE. The corresponding RID is SECURITY_INTERACTIVE_RID.
        public static string SDDL_INTERACTIVE = "IU";

        //Local administrator.  The corresponding RID is DOMAIN_USER_RID_ADMIN.
        public static string SDDL_LOCAL_ADMIN = "LA";

        //Local guest.  The corresponding RID is DOMAIN_USER_RID_GUEST.
        public static string SDDL_LOCAL_GUEST = "LG";

        //Local service account.  The corresponding RID is SECURITY_LOCAL_SERVICE_RID.
        public static string SDDL_LOCAL_SERVICE = "LS";

        //Local system.  The corresponding RID is SECURITY_LOCAL_SYSTEM_RID.
        public static string SDDL_LOCAL_SYSTEM = "SY";

        //Network logon user.  This is a group identifier added to the token of a process when it was logged on across a network.
        //The corresponding logon type is LOGON32_LOGON_NETWORK.  The corresponding RID is SECURITY_NETWORK_RID.
        public static string SDDL_NETWORK = "NU";

        //Network configuration operators.  The corresponding RID is DOMAIN_ALIAS_RID_NETWORK_CONFIGURATION_OPS.
        public static string SDDL_NETWORK_CONFIGURATION_OPS = "NO";

        //Network service account.  The corresponding RID is SECURITY_NETWORK_SERVICE_RID.
        public static string SDDL_NETWORK_SERVICE = "NS";

        //Printer operators.  The corresponding RID is DOMAIN_ALIAS_RID_PRINT_OPS.
        public static string SDDL_PRINTER_OPERATORS = "PO";

        //Principal self.  The corresponding RID is SECURITY_PRINCIPAL_SELF_RID.
        public static string SDDL_PERSONAL_SELF = "PS";

        //Power users.  The corresponding RID is DOMAIN_ALIAS_RID_POWER_USERS.
        public static string SDDL_POWER_USERS = "PU";

        //RAS servers group.  The corresponding RID is DOMAIN_ALIAS_RID_RAS_SERVERS.
        public static string SDDL_RAS_SERVERS = "RS";

        //Terminal server users.  The corresponding RID is DOMAIN_ALIAS_RID_REMOTE_DESKTOP_USERS.
        public static string SDDL_REMOTE_DESKTOP = "RS";

        //Replicator.  The corresponding RID is DOMAIN_ALIAS_RID_REPLICATOR.
        public static string SDDL_REPLICATOR = "RE";

        //Restricted code.  This is a restricted token created using the CreateRestrictedToken() function.
        //The corresponding RID is SECURITY_RESTRICTED_CODE_RID.
        public static string SDDL_RESTRICTED_CODE = "RC";

        //Schema administrators.  The corresponding RID is DOMAIN_GROUP_RID_SCHEMA_ADMINS.
        public static string SDDL_SCHEMA_ADMINISTRATORS = "SA";

        //Server operators.  The corresponding RID is DOMAIN_ALIAS_RID_SYSTEM_OPS.
        public static string SDDL_SERVER_OPERATORS = "SO";

        //Service logon user.  This is a group identifier added to the token of a process when it was logged as a service.
        //The corresponding logon type is LOGON32_LOGON_SERVICE.  The corresponding RID is SECURITY_SERVICE_RID.
        public static string SDDL_SERVICE = "SU";

        #endregion
    }
}
