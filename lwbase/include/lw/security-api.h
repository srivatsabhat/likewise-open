#ifndef __LWBASE_SECURITY_API_H__
#define __LWBASE_SECURITY_API_H__

#include "security-types.h"
#include <lw/ntstatus.h>
#include <lw/rtlstring.h>

//
// SID Functions
//

NTSTATUS
RtlInitializeSid(
    OUT PSID Sid,
    IN PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    IN UCHAR SubAuthorityCount
    );
///<
/// Initialize a SID buffer.
///
/// Initialize SID buffer of sufficiently large size with
/// the relevant identifier authority and sub-authority count.
/// Note that the sub-authority values are not initialized.
///
/// @param[out] Sid - SID buffer
/// @param[in] IdentifierAuthority - Identifier authority
/// @param[in] SubAuthorityCount - Count of sub-authories.
///
/// @return NTSTATUS
///   @arg STATUS_SUCCESS on success
///   @arg STATUS_INVALID_PARAMETER if sub-authority count is invalid.
///

ULONG
RtlLengthRequiredSid(
    IN ULONG SubAuthorityCount
    );
///<
/// Get length required for a SID.
///
/// Get number of bytes required to store a SID with the specified
/// number of sub-authorities.
///
/// @param[in] SubAuthorityCount - Count of sub-authorities.
///
/// @return Number of bytes required to store the SID.
///   @arg STATUS_SUCCESS on success
///   @arg STATUS_INVALID_PARAMETER if sub-authority count is invalid.
///

ULONG
RtlLengthSid(
    IN PSID Sid
    );
///<
/// Get length of a SID.
///
/// Get number of bytes required to store the specified SID.
///
/// @param[in] SID - A SID
///
/// @return Number of bytes required to store the SID.
///

BOOLEAN
RtlValidSid(
    IN PSID Sid
    );

BOOLEAN
RtlEqualSid(
    IN PSID Sid1,
    IN PSID Sid2
    );

BOOLEAN
RtlEqualPrefixSid(
    IN PSID Sid1,
    IN PSID Sid2
    );

BOOLEAN
RtlIsPrefixSid(
    IN PSID Prefix,
    IN PSID Sid
    );

#if 0
PSID_IDENTIFIER_AUTHORITY
RtlIdentifierAuthoritySid(
    IN PSID Sid
    );

PULONG
RtlGetSubAuthorityPointerSid(
    IN PSID Sid,
    IN ULONG SubAuthorityIndex
    );

PUCHAR
RtlGetSubAuthorityCountSid(
    IN PSID Sid
    );
#endif

NTSTATUS
RtlCopySid(
    IN ULONG DestinationSidLength,
    OUT PSID DestinationSid,
    IN PSID SourceSid
    );

//
// ACL Functions
//

NTSTATUS
RtlCreateAcl(
    OUT PACL Acl,
    IN ULONG AclLength,
    IN ULONG AclRevision
    );

NTSTATUS
RtlAddAce(
    IN OUT PACL Acl,
    IN ULONG AceRevision,
    IN ULONG StartingAceIndex,
    IN PVOID AceList,
    IN ULONG AceListLength
    );

NTSTATUS
RtlDeleteAce(
    IN OUT PACL Acl,
    IN ULONG AceIndex
    );

NTSTATUS
RtlGetAce(
    IN PACL Acl,
    IN ULONG AceIndex,
    OUT PVOID* Ace
    );

NTSTATUS
RtlAddAccessAllowedAceEx(
    IN PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    );

NTSTATUS
RtlAddAccessDeniedAceEx(
    IN PACL Acl,
    IN ULONG AceRevision,
    IN ULONG AceFlags,
    IN ACCESS_MASK AccessMask,
    IN PSID Sid
    );

//
// SD Functions
//

NTSTATUS
RtlCreateSecurityDescriptorAbsolute(
    OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN ULONG Revision
    );

NTSTATUS
RtlCreateSecurityDescriptorRelative(
    OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN ULONG Revision
    );

BOOLEAN
RtlValidSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor
    );

BOOLEAN
RtlValidRelativeSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_RELATIVE SecurityDescriptor,
    IN ULONG SecurityDescriptorLength,
    IN SECURITY_INFORMATION RequiredInformation
    );

ULONG
RtlLengthSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor
    );

NTSTATUS
RtlGetSecurityDescriptorControl(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    OUT OPTIONAL PSECURITY_DESCRIPTOR_CONTROL Control,
    OUT OPTIONAL PUCHAR Revision
    );

NTSTATUS
RtlSetSecurityDescriptorControl(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN SECURITY_DESCRIPTOR_CONTROL BitsToChange,
    IN SECURITY_DESCRIPTOR_CONTROL BitsToSet
    );

#if 0
NTSTATUS
RtlGetSecurityDescriptorRmControl(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN UCHAR BitsToChange,
    IN UCHAR BitsToSet
    );

NTSTATUS
RtlSetSecurityDescriptorRmControl(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN UCHAR BitsToChange,
    IN UCHAR BitsToSet
    );
#endif

// ISSUE-For complete-ness, would need a way to set RM bits.

NTSTATUS
RtlGetOwnerSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    OUT PSID* Owner,
    OUT PBOOLEAN IsOwnerDefaulted
    );

NTSTATUS
RtlGetGroupSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    OUT PSID* Group,
    OUT PBOOLEAN IsGroupDefaulted
    );

NTSTATUS
RtlGetSaclSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    OUT PBOOLEAN IsSaclPresent,
    OUT PACL* Sacl,
    OUT PBOOLEAN IsSaclDefaulted
    );

NTSTATUS
RtlGetDaclSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    OUT PBOOLEAN IsDaclPresent,
    OUT PACL* Dacl,
    OUT PBOOLEAN IsDaclDefaulted
    );

NTSTATUS
RtlSetOwnerSecurityDescriptor(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN PSID Owner,
    IN BOOLEAN IsOwnerDefaulted
    );

NTSTATUS
RtlSetGroupSecurityDescriptor(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN OPTIONAL PSID Group,
    IN BOOLEAN IsGroupDefaulted
    );

NTSTATUS
RtlSetSaclSecurityDescriptor(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN BOOLEAN IsSaclPresent,
    IN OPTIONAL PACL Sacl,
    IN OPTIONAL BOOLEAN IsSaclDefaulted
    );

NTSTATUS
RtlSetDaclSecurityDescriptor(
    IN OUT PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN BOOLEAN IsDaclPresent,
    IN OPTIONAL PACL Dacl,
    IN OPTIONAL BOOLEAN IsDaclDefaulted
    );

NTSTATUS
RtlAbsoluteToSelfRelativeSD(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE AbsoluteSecurityDescriptor,
    OUT OPTIONAL PSECURITY_DESCRIPTOR_RELATIVE SelfRelativeSecurityDescriptor,
    IN OUT PULONG BufferLength
    );

NTSTATUS
RtlSelfRelativeToAbsoluteSD(
    IN PSECURITY_DESCRIPTOR_RELATIVE SelfRelativeSecurityDescriptor,
    OUT OPTIONAL PSECURITY_DESCRIPTOR_ABSOLUTE AbsoluteSecurityDescriptor,
    IN OUT PULONG AbsoluteSecurityDescriptorSize,
    OUT OPTIONAL PACL Dacl,
    IN OUT PULONG DaclSize,
    OUT OPTIONAL PACL Sacl,
    IN OUT PULONG SaclSize,
    OUT OPTIONAL PSID Owner,
    IN OUT PULONG OwnerSize,
    OUT OPTIONAL PSID PrimaryGroup,
    IN OUT PULONG PrimaryGroupSize
    );

//
// ACCESS_MASK Functions
//

VOID
RtlMapGenericMask(
    IN OUT PACCESS_MASK AccessMask,
    IN PGENERIC_MAPPING GenericMapping
    );

//
// Access Token Functions
//

// TODO-Do we need an AuthenticationId LUID?
// (Note: Multiple tokens can belong to a single AuthenticationId.  The TokenId
// is what is unique.)
// TODO-Support SID_ATTRIBUTES (SID_AND_ATTRIBUTES) for user/groups.
// (However, note that user cannot have any attributes).
// TODO-Support TOKEN_PRIVILEGES?
// TODO-Support TOKEN_SOURCE?

NTSTATUS
RtlCreateAccessToken(
    OUT PACCESS_TOKEN* AccessToken,
    IN PTOKEN_USER User,
    IN PTOKEN_GROUPS Groups,
#if 0
    IN PTOKEN_PRIVILEGES Privileges
#endif
    IN PTOKEN_OWNER Owner,
    IN PTOKEN_PRIMARY_GROUP PrimaryGroup,
    IN PTOKEN_DEFAULT_DACL DefaultDacl,
#if 0
    IN PTOKEN_SOURCE Source,
#endif
    IN OPTIONAL PTOKEN_UNIX Unix
    );

// NEW
VOID
RtlReleaseAccessToken(
    IN OUT PACCESS_TOKEN* AccessToken
    );

NTSTATUS
RtlQueryAccessTokenInformation(
    IN PACCESS_TOKEN AccessToken,
    IN TOKEN_INFORMATION_CLASS TokenInformationClass,
    OUT OPTIONAL PVOID TokenInformation,
    IN ULONG TokenInformationLength,
    OUT PULONG ReturnedLength
    );

NTSTATUS
RtlQueryAccessTokenUnixInformation(
    IN PACCESS_TOKEN AccessToken,
    OUT PTOKEN_UNIX TokenInformation
    );
///<
/// Query access token for Unix-specific information.
///
/// @param[in] AccessToken - Token to query.
/// @param[out] TokenInformation - Returns Unix token information,
///     if the token contains it.
///
/// @return NTSTATUS
///   @arg STATUS_SUCCESS on success
///   @arg STATUS_INVALID_PARAMETER
///   @arg STATUS_NOT_FOUND - If token does not have Unix information.
///

BOOLEAN
RtlAccessCheck(
    IN PSECURITY_DESCRIPTOR_ABSOLUTE SecurityDescriptor,
    IN PACCESS_TOKEN AccessToken,
    IN ACCESS_MASK DesiredAccess,
    IN ACCESS_MASK PreviouslyGrantedAccess,
#if 0
    OUT OPTIONAL PPRIVILEGE_SET* Privileges,
#endif
    IN PGENERIC_MAPPING GenericMapping,
    OUT PACCESS_MASK GrantedAccess,
    OUT PNTSTATUS AccessStatus
    );

//
// SID <-> String Conversion Functions
//

NTSTATUS
RtlAllocateUnicodeStringFromSid(
    OUT PUNICODE_STRING StringSid,
    IN PSID Sid
    );

NTSTATUS
RtlAllocateAnsiStringFromSid(
    OUT PANSI_STRING StringSid,
    IN PSID Sid
    );

NTSTATUS
RtlAllocateWC16StringFromSid(
    OUT PWSTR* StringSid,
    IN PSID Sid
    );

NTSTATUS
RtlAllocateCStringFromSid(
    OUT PSTR* StringSid,
    IN PSID Sid
    );

NTSTATUS
RtlAllocateSidFromUnicodeString(
    OUT PSID* Sid,
    IN PUNICODE_STRING StringSid
    );

NTSTATUS
RtlAllocateSidFromAnsiString(
    OUT PSID* Sid,
    IN PANSI_STRING StringSid
    );

NTSTATUS
RtlAllocateSidFromWC16String(
    OUT PSID* Sid,
    IN PCWSTR StringSid
    );

NTSTATUS
RtlAllocateSidFromCString(
    OUT PSID* Sid,
    IN PCSTR StringSid
    );

//
// Well-Known SID Functions
//

NTSTATUS
RtlCreateWellKnownSid(
    IN WELL_KNOWN_SID_TYPE WellKnownSidType,
    IN OPTIONAL PSID DomainOrComputerSid,
    OUT PSID* Sid,
    IN OUT PULONG SidSize
    );

#endif /* __LWBASE_SECURITY_API_H__ */
