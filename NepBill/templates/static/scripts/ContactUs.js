function handleIssueChange(selectedValue) {
  // Hide all dynamic section wrappers
  const dynamicGroups = document.querySelectorAll(".dynamic-group");
  dynamicGroups.forEach((group) => {
    group.style.display = "none";
  });

  // Show the selected dynamic group
  if (selectedValue) {
    const activeGroup = document.getElementById(`fields-${selectedValue}`);
    if (activeGroup) {
      activeGroup.style.display = "block";
    }
  }
}

async function SubmitContact(event) {
  // Prevent default HTML form navigation
  if (event) event.preventDefault();

  const issueType = document.getElementById("issue_type")?.value;
  if (!issueType) {
    alert("Please select an issue/topic category.");
    return;
  }

  // Create FormData container for text + binary files
  const formData = new FormData();

  // 1. Common Fields
  formData.append("phone", document.getElementById("phone")?.value || "");
  formData.append("email", document.getElementById("email")?.value || "");
  formData.append("issue_type", issueType);
  formData.append("message", document.getElementById("message")?.value || "");

  console.log(issueType);
  // 2. Dynamic Category-Specific Fields
  switch (issueType) {
    case "RegisterAccount": {
      formData.append(
        "business_name",
        document.getElementById("reg_business_name")?.value || "",
      );
      formData.append(
        "owner_name",
        document.getElementById("reg_owner_name")?.value || "",
      );
      formData.append(
        "pan_number",
        document.getElementById("reg_pan")?.value || "",
      );
      formData.append(
        "vat_number",
        document.getElementById("reg_vat")?.value || "",
      );
      formData.append(
        "business_address",
        document.getElementById("reg_address")?.value || "",
      );
      formData.append(
        "business_continent",
        document.getElementById("reg_continent")?.value || "",
      );
      formData.append(
        "business_country",
        document.getElementById("reg_country")?.value || "",
      );
      formData.append(
        "account_password",
        document.getElementById("reg_password")?.value || "",
      );

      // File Uploads
      const panFile = document.getElementById("reg_pan_photo")?.files[0];
      const certFile = document.getElementById("reg_biz_cert")?.files[0];
      const businessfrontFile = document.getElementById(
        "reg_businessfront_photo",
      )?.files[0];

      if (panFile) {
        formData.append("pan_vat_photo", panFile);
      } else {
        alert("please provide pan photo!");
        break;
      }
      if (certFile) {
        formData.append("business_cert_photo", certFile);
      } else {
        alert("please provide certification photo!");
        break;
      }
      if (businessfrontFile) {
        formData.append("business_front_photo", businessfrontFile);
      } else {
        alert("please provide Business Front photo!");
        break;
      }

      break;
    }

    case "issues": {
      formData.append(
        "error_code",
        document.getElementById("issue_code")?.value || "",
      );

      // File Upload
      const screenshot = document.getElementById("issue_screenshot")?.files[0];
      if (screenshot) formData.append("error_screenshot", screenshot);
      break;
    }

    case "help": {
      formData.append(
        "help_feature",
        document.getElementById("help_feature")?.value || "",
      );
      break;
    }

    case "billing": {
      formData.append(
        "billing_id",
        document.getElementById("billing_id")?.value || "",
      );
      break;
    }

    case "others": {
      formData.append(
        "others_subject",
        document.getElementById("others_subject")?.value || "",
      );
      break;
    }
  }

  console.log("Type: ", issueType);

  console.log("formData: ", formData);
  // 3. Dispatch to C++ / Backend Endpoint
  try {
    const response = await fetch("/api/contactform/submit", {
      method: "POST",
      // DO NOT set 'Content-Type' header here.
      // The browser automatically computes boundary strings for multipart FormData.
      body: formData,
    });

    if (response.ok) {
      alert("Your inquiry has been submitted successfully!");

      // Optional: Reset form on success
      const form = document.querySelector("form");
      if (form) form.reset();

      // Hide active dynamic groups
      const dynamicGroups = document.querySelectorAll(".dynamic-group");
      dynamicGroups.forEach((group) => (group.style.display = "none"));
    } else {
      const errorText = await response.text();
      alert(`Submission failed: ${errorText || "Server error occurred"}`);
    }
  } catch (error) {
    console.error("Error submitting contact form:", error);
    alert("Network error: Failed to connect to server.");
  }
}
