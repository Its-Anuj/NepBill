async function logout() {
  const response = await fetch("/api/logout", {
    method: "POST",
  });
  if (response.ok) {
    console.log("logout");

    window.location.href = "/";
  }
}
