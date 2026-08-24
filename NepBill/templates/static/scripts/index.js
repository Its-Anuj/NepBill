async function CheckLogin() {
  const response = await fetch("/api/checklogin");
  const Header = document.getElementById("AccountHeaderSection");

  if (!Header) {
    console.warn("The .header-right section was not found on this page.");
    return;
  }

  const User = await response.json();

  console.log(User);

  if (User.LoggedIn) {
    console.log("Logged in: ");
    console.log(User.AccountId);
      Header.innerHTML = `
              <a href="/Account/Account.html?id=${User.AccountId}" class="btn-primary">
                  Our Account
              </a>
        `;
  } else {
    Header.innerHTML = `
            <a href="/Login.html" class="btn-login">Login</a>
            <a href="/ContactUs.html" class="btn-primary">Contact Us</a>
        `;
  }
}

CheckLogin();
