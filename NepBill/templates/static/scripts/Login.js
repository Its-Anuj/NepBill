async function SendOtp() {
  console.log("sendOtp() called");

  const phone = document.getElementById("PhoneNumber").value;

  const password = document.getElementById("Password").value;

  console.log("Phone:", phone);
  console.log("Password:", password);

  const response = await fetch("/api/send-otp", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      phone: phone,
      password: password,
    }),
  });

  console.log("Response status:", response.status);

  const result = await response.json();

  console.log("Server response:", result);

  alert(result.message);
}

async function SubmitLogin() {
  const Account_Id = document.getElementById("account_id").value;

  const password = document.getElementById("password").value;
  console.log("Kigub");

  const response = await fetch("/api/login", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: Account_Id,
      Password: password
    }),
  });

  const result = await response.json();
  console.log(result);

}
