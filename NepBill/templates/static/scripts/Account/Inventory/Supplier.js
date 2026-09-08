async function AddSupplier(UserId) {
  const NameElement = document.getElementById("supplier_input_name");
  const PanElement = document.getElementById("supplier_input_pan");
  const PhoneElement = document.getElementById("supplier_input_phone");

  const nameError = document.getElementById("error_name");
  const panError = document.getElementById("error_pan");
  const phoneError = document.getElementById("error_phone");

  let valid = true;

  if (!NameElement.value?.trim()) {
    nameError.textContent = "Name is required!";
    valid = false;
  } else {
    nameError.textContent = "";
  }

  if (!PanElement.value?.trim()) {
    panError.textContent = "PAN is required!";
    valid = false;
  } else {
    panError.textContent = "";
  }

  if (!PhoneElement.value?.trim()) {
    phoneError.textContent = "Phone is required!";
    valid = false;
  } else {
    phoneError.textContent = "";
  }

  console.log({
    AccountId: UserId,
    Name: NameElement.value,
    PanNumber: PanElement.value,
    PhoneNumber: PhoneElement.value,
  });

  if (!valid) return; // stop before submitting

  const response = await fetch("/api/business/supplier/add", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: UserId,
      Name: NameElement.value,
      PanNumber: PanElement.value,
      PhoneNumber: PhoneElement.value,
    }),
  });

  const result = await response.json();
  console.log(result);
  // ... proceed with the actual add logic
}

async function QuerySuppliers(UserId) {
  const response = await fetch("/api/business/supplier/query", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({
      AccountId: UserId,
    }),
  });

  const result = await response.json();
  console.log(result);
  const tableContainer = document.getElementById("SupplierTable");

  if (result.State && result.Count > 0) {
    let tableHtml = `
      <table class="data-table">
        <thead>
          <tr>
            <th>Name</th>
            <th>PAN Number</th>
            <th>Phone Number</th>
            <th>Opening Balance</th>
          </tr>
        </thead>
        <tbody>
    `;

    result.Supplier.forEach((s) => {
      tableHtml += `
        <tr>
          <td>${s.Name || "-"}</td>
          <td>${s.PanNumber || "-"}</td>
          <td>${s.PhoneNumber || "-"}</td>
          <td>NPR ${s.OpeningBalance ?? 0}</td>
        </tr>
      `;
    });

    tableHtml += `</tbody></table>`;
    tableContainer.innerHTML = tableHtml;
  } else {
    tableContainer.innerHTML = `<div class="empty-state">No suppliers found.</div>`;
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL
  console.log(userId);

  let AddSupplierElement = document.getElementById("add_supplier_button");
  AddSupplierElement.addEventListener("click", (event) => AddSupplier(userId));

  QuerySuppliers(userId);
});
