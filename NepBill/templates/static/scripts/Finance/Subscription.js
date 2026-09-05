class SubcriptionData {
  /**
   * @param {string} UserId
   * @param {string} role
   * @param {number} type
   */
  constructor(AccountId, Role, Type) {
    this.AccountId = AccountId;
    this.Role = RoleMapper.FromStr(Role);
    this.Type = AccountIdTypeMapper.FromInt(Type);
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL
  const plan = urlParams.get("plan"); // Retrieves the 'id' parameter from the URL
  const price = urlParams.get("price"); // Retrieves the 'id' parameter from the URL
  const type = urlParams.get("type"); // Retrieves the 'id' parameter from the URL

  console.log(userId);
  console.log(plan);
  console.log(price);
  console.log(type);
});
