// 1. Define the enum structure
export const ContactStates = {
  None: "None",
  Processing: "Processing",
  Handled: "Handled",
  Failed: "Failed",
};

// 1. Define the enum structure
export const ContactSubjectType = {
  RegisterAccount: "RegisterAccount",
  Issue: "Issue",
  Help: "Help",
  Billing: "Billing",
  Other: "Other",
};

export const ContactSubjectTypeMapper = {
  // Parse incoming data (e.g., from your Crow backend API) safely
  FromStr(str) {
    return Object.values(ContactSubjectType).includes(str) ? str : ContactSubjectType.Other;
  },

  // Format the enum value for UI presentation
  ToDisplayStr(type) {
    const names = {
      [ContactSubjectType.RegisterAccount]: "RegisterAccount",
      [ContactSubjectType.Issue]: "Report an Issue",
      [ContactSubjectType.Help]: "General Help",
      [ContactSubjectType.Billing]: "Billing Inquiry",
      [ContactSubjectType.Other]: "Other",
    };
    return names[type] || "Unknown";
  },
};

export const ContactStateMapper = {
  // Parse incoming data (e.g., from your Crow backend API) safely
  FromStr(str) {
    return Object.values(ContactStates).includes(str) ? str : ContactStates.None;
  },

  // Format the enum value for UI presentation
  ToDisplayStr(state) {
    const names = {
      [ContactStates.None]: "None",
      [ContactStates.Processing]: "Processing",
      [ContactStates.Handled]: "Handled",
      [ContactStates.Failed]: "Failed",
    };
    return names[state] || "Unknown";
  },
};
