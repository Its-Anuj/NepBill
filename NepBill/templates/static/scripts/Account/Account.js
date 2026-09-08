import * as AccountTypes from "./AccountTypes.js";
import * as SideBar from "./SideBar.js";

async function GetAccountInfo(Account_Id) {
  const response = await fetch("/api/account/queryaccount", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: Account_Id,
    }),
  });
  const result = await response.json();
  console.log(result);

  let Info = new AccountTypes.AccountInfo(
    Account_Id,
    result.Roles,
    AccountTypes.CheckAccountIdType(Account_Id),
  );
  return Info;
}

/**
 * Loads an HTML snippet into a target DOM container
 * @param {string} targetId - The ID of the element where HTML will be inserted
 * @param {string} filePath - Path to the HTML component file
 * @param {Function} [onLoaded] - Optional callback after the HTML is inserted
 */
export async function LoadComponent(targetId, filePath, onLoaded) {
  try {
    const response = await fetch(filePath);
    if (!response.ok) {
      throw new Error(
        `Failed to load component from ${filePath}: ${response.status}`,
      );
    }

    const htmlContent = await response.text();
    const container = document.getElementById(targetId);

    if (container) {
      container.innerHTML = htmlContent;

      // Execute post-load logic (like active link highlights or events)
      if (typeof onLoaded === "function") {
        onLoaded(container);
      }
    }
  } catch (error) {
    console.error("[Component Loader Error]:", error);
  }
}

document.addEventListener("DOMContentLoaded", () => {
  // 1. Load the common sidebar component
  LoadComponent(
    "sidebar-container",
    "/Account/Components/SideBar.html",
    async (sidebarEl) => {
      console.log(sidebarEl);

      const urlParams = new URLSearchParams(window.location.search);
      const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL
      let UserInfo = await GetAccountInfo(userId);
      console.log(UserInfo);

      if (UserInfo) {
        SideBar.HandleSideBarPerRole(
          UserInfo.AccountId,
          UserInfo.Role,
          sidebarEl,
        );
      }
    },
  );
});
