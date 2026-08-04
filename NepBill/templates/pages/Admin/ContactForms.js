async function queryForms() {
  const handledValue = document.getElementById("handled").value;

  let handled = null;

  if (handledValue === "true") handled = true;
  else if (handledValue === "false") handled = false;
  else if (handledValue === "None") handled = -1;

  const query = {
    phone: document.getElementById("phone").value,
    subject: document.getElementById("subject").value,
    handled: handled,
    After: document.getElementById("after").value,
    Before: document.getElementById("before").value,
    limit: parseInt(document.getElementById("limit").value),
  };

  const response = await fetch("/api/admin/contactforms/query", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify(query),
  });

  const forms = await response.json();

  console.log(forms.Count);
  if (forms.Count != 0) populate(forms);
  else {
    // Fixed Line 33
    const body = document.getElementById("tableBody");
    body.innerHTML = " ";
  }
}

function populate(forms) {
  const body = document.getElementById("tableBody");

  body.innerHTML = " ";
  console.log(forms);

  forms.Forms.forEach((f) => {
    body.innerHTML += `
<tr>

<td>${f.PhoneNumber}</td>

<td>${f.Type}</td>

<td>${f.CreatedAt}</td>

<td>${f.Handled}</td>

<td>

<button class="view-btn" onclick="goToDetails('${f.UUID}')">View Details</button>
</button>

</td>

</tr>
`;
  });
}

function goToDetails(UUID) {
  // Encodes characters like '+' to make them URL-safe
  const safePhone = encodeURIComponent(UUID);

  // Navigates to the new page with the parameter attached
  window.location.href = `/Admin/FormDetails.html?UUID=${UUID}`;
}

queryForms();
