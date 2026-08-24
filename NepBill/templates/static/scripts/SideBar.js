import * as AccountTypes from "./AccountTypes.js";

function navigateToAdminPage(event, path) {
  event.preventDefault();

  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id");

  if (userId) {
    window.location.href = `${path}?id=${userId}`;
  } else {
    window.location.href = "/login.html";
  }
}

// A clean, scalable configuration object
const ROLE_NAVIGATION_MAP = {
  [AccountTypes.AccountRoles.Admin]: [
    { elementId: "sidebar-admin-dashboard", path: "/Admin/Dashboard.html" },
    {
      elementId: "sidebar-admin-contactforms",
      path: "/Admin/ContactForms.html",
    },
  ],
  [AccountTypes.AccountRoles.BusinessOwner]: [
    { elementId: "sidebar-owner-profile", path: "/Owner/Profile.html" },
    { elementId: "sidebar-owner-billing", path: "/Owner/Billing.html" },
  ],
  [AccountTypes.AccountRoles.BusinessManager]: [
    { elementId: "sidebar-manager-tasks", path: "/Manager/Tasks.html" },
  ],
};

const Role_SideBar_Map = {
  [AccountTypes.AccountRoles.Admin]: [{ UsageId: "sidebar-view-Admin" }],
  [AccountTypes.AccountRoles.BusinessOwner]: [{ UsageId: "sidebar-view-BusinessOwner" }],
};

/**
 * Handle which sidebar part to show and handles its connection per role
 * @param {string} AccountId - AccountId
 * @param {AccountTypes.AccountRoles} Role - The role of the account associated with the Id
 */
export function HandleSideBarPerRole(AccountId, Role, sidebarEl) {
  console.log("A ", Role_SideBar_Map[Role]);
  let RoleSpecificQuerier = "#" + Role_SideBar_Map[Role][0].UsageId;
  console.log(RoleSpecificQuerier);
  // 3. Apply role-based visibility if needed
  const adminGroup = sidebarEl.querySelectorAll(RoleSpecificQuerier);
  console.log(adminGroup);

  // Grab the first element from the list
  if (adminGroup[0]) {
    adminGroup[0].style.display = "block";
  }

  HandleConnectionsPerRole(AccountId, Role);

  let LogoutElement = document.getElementById("LogoutButton");
  if(LogoutElement)
  {
    LogoutElement.addEventListener("click", (event) =>
        Logout(),
      );
  }
}

/**
 * Handle the connection of buttons in the sidebar per role
 * @param {string} AccountId - AccountId
 * @param {AccountTypes.AccountRoles} Role - The role of the account associated with the Id
 */
export function HandleConnectionsPerRole(AccountId, Role) {
  // Get the navigation links array for the current user's role
  const configurations = ROLE_NAVIGATION_MAP[Role] || [];

  // Loop through the configs and dynamically bind event listeners
  configurations.forEach(({ elementId, path }) => {
    const linkElement = document.getElementById(elementId);
    if (linkElement) {
      linkElement.addEventListener("click", (e) =>
        navigateToAdminPage(e, path),
      );
    }
  });
}

export async function Logout() {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL

  console.log("Logout: ", userId);

  const response = await fetch("/api/account/logout", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: userId,
    }),
  });

  const result = await response.json();
  console.log("Logout: ", result);

  if (result.Success) {
    window.location.href = "/";
  }
}
