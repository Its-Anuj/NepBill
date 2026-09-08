document.addEventListener("DOMContentLoaded", () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL
  console.log("ROOM MOTHER FUCKER: ", userId);

  let AddRoomElement = document.getElementById("add_room_button");
  AddRoomElement.addEventListener("click", () => {window.location.href = `/Account/Rooms/AddRoom.html?id=${userId}`});
});
