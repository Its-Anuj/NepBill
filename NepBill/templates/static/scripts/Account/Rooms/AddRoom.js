async function SubmitNewRoomInfo(userId) {
  const nameElement = document.getElementById("room_name");
  const bedCountElement = document.getElementById("room_bed_count");
  const basePriceElement = document.getElementById("room_base_price");
  const descriptionElement = document.getElementById("room_description");
  const submitButton = document.getElementById("add_room_button");

  if (!nameElement.value || !bedCountElement.value || !basePriceElement.value) {
    alert("Please fill in all required fields.");
    return;
  }

  try {
    submitButton.disabled = true;
    submitButton.innerText = "Submitting...";

    const response = await fetch("/api/business/room/room/add", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        AccountId: userId,
        Name: nameElement.value.trim(),
        Description: descriptionElement.value.trim(),
        BasePrice: parseFloat(basePriceElement.value),
        BedCount: parseInt(bedCountElement.value, 10),
      }),
    });

    const data = await response.json();

    if (data.State === false || data.State === "false") {
      alert(`Error: ${data.Message || "Failed to create room."}`);
    } else {
      alert("New Room Created Successfully!");
      document.getElementById("add_room_form").reset();
    }
  } catch (error) {
    console.error("Error creating room:", error);
    alert("Network error: Could not reach server.");
  } finally {
    submitButton.disabled = false;
    submitButton.innerText = "Create Room";
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id");

  // Room Submission Trigger
  const addRoomBtn = document.getElementById("add_room_button");
  if (addRoomBtn) {
    addRoomBtn.addEventListener("click", async () => {
      await SubmitNewRoomInfo(userId);
    });
  }

  // Collapsible Sidebar Setup
  const sidebarToggleBtn = document.getElementById("sidebar-toggle");
  const sidebarOverlay = document.getElementById("sidebar-overlay");

  if (sidebarToggleBtn) {
    sidebarToggleBtn.addEventListener("click", () => {
      if (window.innerWidth <= 768) {
        document.body.classList.toggle("sidebar-mobile-open");
      } else {
        document.body.classList.toggle("sidebar-collapsed");
      }
    });
  }

  if (sidebarOverlay) {
    sidebarOverlay.addEventListener("click", () => {
      document.body.classList.remove("sidebar-mobile-open");
    });
  }
});