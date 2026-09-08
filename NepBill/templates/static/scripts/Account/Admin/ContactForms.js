import * as AccountType from "/AccountTypes.js";

class ContactFormsInfo {
  constructor(Forms) {
    this.Forms = Forms;
  }
}

async function FetchContactForms(AccountId) {
  try {
    console.log(AccountId);
    const response = await fetch("/api/admin/contactform/query", {
      method: "POST",

      headers: {
        "Content-Type": "application/json",
      },

      body: JSON.stringify({
        AccountContactFormId: AccountId
      }),
    });

    // 1. Check if the HTTP status code is successful
    if (!response.ok) {
      throw new Error(`HTTP error! Status: ${response.status}`);
    }

    // 2. Get the raw text first to see if it is empty
    const responseText = await response.text();

    // 3. Parse only if text exists, otherwise return an empty object/array
    const data = responseText ? JSON.parse(responseText) : [];

    console.log("Data received:", data.Forms);
    return new ContactFormsInfo(data.Forms);
  } catch (error) {
    console.error("Error fetching contact forms:", error);
  }
}

function PopulateForms(Forms) {
  const body = document.getElementById("tableBody");

  body.innerHTML = " ";

  Forms.Forms.forEach((f) => {
    body.innerHTML += `
<tr>

<td>${f.UUID}</td>
<td>${f.PhoneNumber}</td>

<td>${f.Type}</td>

<td>${f.CreatedAt}</td>

<td>${f.State}</td>

<td>

<button class="view-btn" data-uuid=${f.UUID}>View Details</button>
</button>

</td>

</tr>
`;
  });

  const ViewDetailsLinks = document.querySelectorAll(".view-btn");
  console.log(ViewDetailsLinks);

  // Iterate through the matching link elements
  ViewDetailsLinks.forEach((DetailLink) => {
    DetailLink.addEventListener("click", (event) => GoToDetails(event));
  });
}

export function GoToDetails(Event) {
  const uuid = Event.currentTarget.dataset.uuid;
  const urlParams = new URLSearchParams(window.location.search);
  const AccountId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL

  window.location.href = `/Admin/ContactFormDetail.html?id=${AccountId}&uuid=${uuid}`;
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const AccountId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL

  if (AccountId) {
    let Forms = await FetchContactForms(AccountId);

    PopulateForms(Forms);
  }
});
