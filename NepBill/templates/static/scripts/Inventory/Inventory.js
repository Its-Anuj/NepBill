async function SupplierIncomingResults(UserId) {
  const response = await fetch("/api/account/queryaccount", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: Account_Id,
    }),
  });
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL

  // Future check roles etc
});
