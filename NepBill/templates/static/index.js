function openMenu() {
  document.getElementById("sideMenu").classList.add("open");
  document.getElementById("menuOverlay").style.display = "block";
}

function closeMenu() {
  document.getElementById("sideMenu").classList.remove("open");
  document.getElementById("menuOverlay").style.display = "none";
}

async function loadHeader() {
  const response = await fetch("/api/me");

  const user = await response.json();
  const headerRight = document.querySelector(".header-right");

  if (!headerRight) {
    console.warn("The .header-right section was not found on this page.");
    return;
  }

  if (user.loggedIn) {
    console.log("Logged in: ");
    console.log(user.PhoneNumber);
    headerRight.innerHTML = `
            <a href="/Account/Account.html" class="login-btn">
                Our Account
            </a>
        `;
  } else {
    headerRight.innerHTML = `
            <a href="/login.html" class="login-btn">
                Login
            </a>

            <a href="/Common/Contact.html" class="signup-btn">
                Contact Us
            </a>
        `;
  }
}

async function goToDashboard(event) {
  event.preventDefault();

  const response = await fetch("/api/me");

  if (!response.ok) {
    window.location.href = "/login.html";
    return;
  }

  const user = await response.json();
  if (user.loggedIn == false) {
    window.location.href = "/login.html";
    return;
  }

  switch (user.Role) {
    case "Admin":
      window.location.href = "/Admin/Dashboard.html";
      break;

    case "BusinessOwner":
      window.location.href = "/Account/Dashboard.html";
      break;

    case "Staff":
      window.location.href = "/staff/dashboard.html";
      break;

    default:
      window.location.href = "/";
      break;
  }
}

loadHeader();
