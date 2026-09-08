// 1. Define the enum structure
export const AccountRoles = {
  None: "None",
  BusinessOwner: "BusinessOwner",
  BusinessManager: "BusinessManager",
  BusinessReceptionist: "BusinessReceptionist",
  BusinessAccountant: "BusinessAccountant",
  BusinessInventorManager: "BusinessInventorManager",
  BusinessStaff: "BusinessStaff",
  BusinessKitchen: "BusinessKitchen",
  Admin: "Admin",
};

export const RoleMapper = {
  // Parse incoming data (e.g., from your Crow backend API) safely
  FromStr(str) {
    return Object.values(AccountRoles).includes(str) ? str : AccountRoles.None;
  },

  // Format the enum value for UI presentation
  ToDisplayStr(role) {
    const names = {
      [AccountRoles.None]: "None",
      [AccountRoles.BusinessOwner]: "Business Owner",
      [AccountRoles.BusinessManager]: "Business Manager",
      [AccountRoles.BusinessReceptionist]: "Receptionist",
      [AccountRoles.BusinessAccountant]: "Accountant",
      [AccountRoles.BusinessInventorManager]: "Inventory Manager",
      [AccountRoles.BusinessStaff]: "Staff",
      [AccountRoles.BusinessKitchen]: "Kitchen",
      [AccountRoles.Admin]: "Admin",
    };
    return names[role] || "Unknown";
  },
};

export const AccountIdType = {
  None: 0,
  PhoneNumber: 1,
  Email: 2,
};

export const AccountIdTypeMapper = {
  // 1. Convert an Integer Value to its String Name
  ToString(value) {
    const names = {
      [AccountIdType.None]: "None",
      [AccountIdType.PhoneNumber]: "Phone Number",
      [AccountIdType.Email]: "Email Address",
    };
    // Fallback to "Unknown" if the integer isn't in the enum
    return names[value] || "Unknown";
  },

  // 2. Convert a String Name into the Integer Value
  FromStr(str) {
    if (!str) return AccountIdType.None;

    // Clean up input string (remove spaces, lowercase everything for safety)
    const cleanStr = str.replace(/\s+/g, "").toLowerCase();

    const lookup = {
      none: AccountIdType.None,
      phonenumber: AccountIdType.PhoneNumber,
      email: AccountIdType.Email,
      emailaddress: AccountIdType.Email, // extra alias for convenience
    };

    // Return the matching int, or default to 0 (None) if not found
    return lookup[cleanStr] !== undefined
      ? lookup[cleanStr]
      : AccountIdType.None;
  },

  // 3. Clean validation for safe runtime/backend parsing
  FromInt(value) {
    const parsed = parseInt(value, 10);
    return Object.values(AccountIdType).includes(parsed)
      ? parsed
      : AccountIdType.None;
  },
};

export class AccountInfo {
  /**
   * @param {string} accountId
   * @param {string} role
   * @param {number} type
   */
  constructor(AccountId, Role, Type) {
    this.AccountId = AccountId;
    this.Role = RoleMapper.FromStr(Role);
    this.Type = AccountIdTypeMapper.FromInt(Type);
  }

  display() {
    return `${this.AccountId} has role ${RoleMapper.ToDisplayStr(this.Role)} of type ${AccountIdTypeMapper.ToString(this.Type)}`;
  }
}

export function CheckAccountIdType(Id) {
  if (typeof Id != "string") {
    return AccountIdType.None;
  }

  let IsGmail = false;
  if (Id.includes("@") == true && Id.includes(".com") == true) {
    IsGmail = true;
  }

  if (IsGmail == true) {
    return AccountIdType.Email;
  }
  if (IsGmail == false) {
    return AccountIdType.PhoneNumber;
  }
}
