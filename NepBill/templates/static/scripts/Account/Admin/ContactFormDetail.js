import * as ContactEnums from "/Admin/ContactEnums.js";
import { LoadComponent } from "/Account.js";

function ContactStateOption(Value) {
  const Option = document.createElement("option");
  Option.value = Value;
  Option.textContent = Value;
  return Option;
}

async function FetchContactFormDetail(ContactFormId) {
  try {
    const response = await fetch("/api/admin/contactform/querydetails", {
      method: "POST",

      headers: {
        "Content-Type": "application/json",
      },

      body: JSON.stringify({
        UUID: ContactFormId,
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

    console.log("Data received:", data);

    const StateSelect = document.getElementById("val-state");
    StateSelect.value = data.State;
    let EmailElement = document.getElementById("val-email");
    EmailElement.value = data.Email;
    let PhoneElement = document.getElementById("val-phone");
    PhoneElement.value = data.PhoneNumber;
    let UUIDElement = document.getElementById("val-uuid");
    UUIDElement.value = ContactFormId;
    let CreatedAtElement = document.getElementById("val-created");
    CreatedAtElement.value = data.CreatedAt;
    let TypeElement = document.getElementById("val-type");
    TypeElement.value = data.Type;

    let BadgeStateElement = document.getElementById("badge-handled-state");
    BadgeStateElement.textContent = data.State;

    return data.Type;
  } catch (error) {
    console.error("Error fetching contact forms:", error);
  }
}

function HandleButtons(ContactFormId) {
  let SaveButton = document.getElementById("btn-save");
  SaveButton.addEventListener("click", (event) => OnSave(ContactFormId));

  let EditButton = document.getElementById("btn-edit");
  EditButton.addEventListener("click", (event) => OnEidt(ContactFormId));
}

async function OnSave(ContactFormId) {
  console.log("Save: ", ContactFormId);

  const StateSelect = document.getElementById("val-state");
  StateSelect.disabled = true;
  let EmailElement = document.getElementById("val-email");
  EmailElement.disabled = true;
  let SaveButton = document.getElementById("btn-save");
  SaveButton.disabled = true;
  let PhoneElement = document.getElementById("val-phone");
  PhoneElement.disabled = true;

  const DetailsResponse = await fetch("/api/admin/contactform/querydetails", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      UUID: ContactFormId,
    }),
  });
  const DetailResult = await DetailsResponse.json();

  console.log(DetailResult);
  // 1. Initialize the payload with the mandatory identifier
  let EditQuery = {
    UUID: ContactFormId,
  };

  // 2. Define a list of your form fields and their matching database keys
  const fields = [
    { key: "Email", id: "val-email" },
    { key: "PhoneNumber", id: "val-phone" },
    { key: "State", id: "val-state" },
  ];

  // 3. Loop through fields, extract .value, and compare
  fields.forEach((field) => {
    let element = document.getElementById(field.id);
    if (element) {
      let currentValue = element.value;

      // Only add to the payload if the user changed it
      if (DetailResult[field.key] !== currentValue) {
        EditQuery[field.key] = currentValue;
      }
    }
  });

  console.log("Edit: ", EditQuery);

  // 4. Send only the modified fields to the server
  const response = await fetch("/api/admin/contactform/editsave", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(EditQuery),
  });

  await FetchContactFormDetail(ContactFormId);
}

function OnEidt(ContactFormId) {
  console.log("Edit: ", ContactFormId);

  let EmailElement = document.getElementById("val-email");
  EmailElement.disabled = false;
  let PhoneElement = document.getElementById("val-phone");
  PhoneElement.disabled = false;
  let SaveButton = document.getElementById("btn-save");
  SaveButton.disabled = false;
  const StateSelect = document.getElementById("val-state");
  StateSelect.disabled = false;
}

/** 
@brief

@param {ContactEnums.ContactStates} Type 
**/

function HandleTypeSpecificAction(ContactFormId, Type) {
  switch (Type) {
    case value:
      break;

    default:
      break;
  }
}

function StateBadgeHandle(params) {
  // 2. Grab the dropdown element from the DOM
  const StateSelect = document.getElementById("val-state");
  StateSelect.appendChild(
    ContactStateOption(
      ContactEnums.ContactStateMapper.ToDisplayStr(
        ContactEnums.ContactStates.None,
      ),
    ),
  );
  StateSelect.appendChild(
    ContactStateOption(
      ContactEnums.ContactStateMapper.ToDisplayStr(
        ContactEnums.ContactStates.Handled,
      ),
    ),
  );
  StateSelect.appendChild(
    ContactStateOption(
      ContactEnums.ContactStateMapper.ToDisplayStr(
        ContactEnums.ContactStates.Processing,
      ),
    ),
  );
  StateSelect.appendChild(
    ContactStateOption(
      ContactEnums.ContactStateMapper.ToDisplayStr(
        ContactEnums.ContactStates.Failed,
      ),
    ),
  );
  StateSelect.disabled = true;
}

async function HandleRegisterAccountAction(ContactFormId, ActionTypeEl) {
  try {
    const Response = await fetch(
      "/api/admin/contactform/registeraction/query",
      {
        method: "POST",

        headers: {
          "Content-Type": "application/json",
        },

        body: JSON.stringify({
          UUID: ContactFormId,
        }),
      },
    );
    const Result = await Response.json();

    // 1. Check if the HTTP status code is successful
    if (!Response.ok) {
      throw new Error(`HTTP error! Status: ${Response.status}`);
    }

    console.log(Result);

    if (Result["Success"] == false) {
      throw new Error(`HTTP error! Status: ${Result["Error"]}`);
    }

    let busName = ActionTypeEl.querySelector("#reg-business-name");
    busName.value = Result["BusinessName"];

    let ownerName = ActionTypeEl.querySelector("#reg-owner-name");
    ownerName.value = Result["Ownername"];

    let panNum = ActionTypeEl.querySelector("#reg-pan-number");
    panNum.value = Result["PanNumber"];

    let vatNum = ActionTypeEl.querySelector("#reg-vat-number");
    vatNum.value = Result["VatNumber"];

    let countryEl = ActionTypeEl.querySelector("#reg-country");
    countryEl.value = Result["Country"];

    let addressEl = ActionTypeEl.querySelector("#reg-address");
    addressEl.value = Result["Address"];
    let PasswordHash = ActionTypeEl.querySelector("#reg-password-hash");
    PasswordHash.value = Result["PasswordHash"];

    let PanPdf = ActionTypeEl.querySelector("#img-pan-pic");
    let PanPdfHref = ActionTypeEl.querySelector("#img_href-pan-pic");

    // Ensure elements exist before manipulation to avoid runtime crash
    if (PanPdf && PanPdfHref) {
      // Retrieve the base64 string from the result payload using the string key
      let rawBase64 = Result["PanPdf"] ? Result["PanPdf"].trim() : "";

      // Check if the backend image string is missing, blank, or an encoded single-byte "0" (AA==)
      if (!rawBase64 || rawBase64 === "" || rawBase64 === "AA==") {
        // A clean, visible 100x100 grey square placeholder
        let greyPlaceholder =
          "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkAQMAAABKLAcXAAAABlBMVEUAAOfn5+fev4g7AAAAAnRSTlMAM8lDoAEAAAAKSURBVDjLY2AYGAAAAoAAAfb1vYwAAAAASUVORK5CYII=";

        PanPdf.src = greyPlaceholder;
        PanPdfHref.removeAttribute("href");
        PanPdfHref.style.display = "none"; // Hide overlay link
      } else {
        // Sanitize check: If backend already included the data URL header, strip it to prevent duplication
        if (rawBase64.startsWith("data:image")) {
          rawBase64 = rawBase64.replace(/^data:image\/[a-zA-Z]+;base64,/, "");
        }

        // Assemble final source with correct template literal syntax
        let cleanSrc = `data:image/jpeg;base64,${rawBase64}`;

        PanPdf.src = cleanSrc;
        PanPdfHref.href = cleanSrc;
        PanPdfHref.style.display = "flex"; // Show overlay link
      }

      // Safety verification printouts for your dev console
      console.log("Image element source set to:", PanPdf.src);
      console.log("Link anchor href set to:", PanPdfHref.href);
    } else {
      console.error(
        "DOM Elements could not be found within ActionTypeEl. Check your selector IDs.",
      );
    }

    let ApproveAccountButton = ActionTypeEl.querySelector(
      "#btn-allow-creation",
    );
    ApproveAccountButton.addEventListener("click", (event) =>
      OnRegisterAccountApprove(ContactFormId, ActionTypeEl),
    );
    ApproveAccountButton.disabled = true;

    if (Result["AccountAssociated"] == false) {
      ApproveAccountButton.disabled = false;
    }

    let VatPdf = ActionTypeEl.querySelector("#img-vat-cert");
    VatPdf.src = `data:image/jpeg;base64,${Result["VatPdf"]}`;
  } catch (error) {
    console.error("Error fetching contact forms:", error);
  }
}

async function OnRegisterAccountApprove(ContactFormId, ActionTypeEl) {
  console.log("OnRegisterAccountApprove: ", ContactFormId);

  const Response = await fetch(
    "/api/admin/contactform/registeraction/approve",
    {
      method: "POST",

      headers: {
        "Content-Type": "application/json",
      },

      body: JSON.stringify({
        UUID: ContactFormId,
      }),
    },
  );
  const Result = await Response.json();

  console.log(Result);

  if (Result["Success"] == true) {
    let ApproveAccountButton = ActionTypeEl.querySelector(
      "#btn-allow-creation",
    );
    ApproveAccountButton.disabled = true;
  }
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const ContactFormId = urlParams.get("uuid"); // Retrieves the 'id' parameter from the URL

  if (ContactFormId) {
    console.log("Contact Form for: ", ContactFormId);
  }

  StateBadgeHandle();
  let Type = await FetchContactFormDetail(ContactFormId);
  console.log(
    Type ==
      ContactEnums.ContactStateMapper.ToDisplayStr(
        ContactEnums.ContactSubjectType.RegisterAccount,
      ),
  );
  HandleButtons(ContactFormId);

  // 1. Load the common sidebar component
  LoadComponent(
    "Type-Specific-Container",
    "/Account/Components/Admin/ContactFormTypeFunctions.html",
    async (ActionTypeEl) => {
      console.log(Type);
      console.log(
        ContactEnums.ContactSubjectTypeMapper.ToDisplayStr(
          ContactEnums.ContactSubjectType.RegisterAccount,
        ),
      );

      const TypeAction = ActionTypeEl.querySelectorAll("#RegisterAccountView");
      console.log("TypeAction");

      // Grab the first element from the list
      if (TypeAction[0]) {
        TypeAction[0].style.display = "block";
      }

      HandleRegisterAccountAction(ContactFormId, ActionTypeEl);
    },
  );
});
