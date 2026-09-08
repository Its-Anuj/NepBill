async function AddItemAndCategory(UserId) {
  window.location.href = `/Account/Inventory/AddItem.html?id=${UserId}`;
}

async function TableStock(UserId) {
  const tableBody = document.getElementById("inventory-table-body");

  try {
    const response = await fetch("/api/business/inventory/item/query", {
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

    if (!result.Item || result.Item.length === 0) {
      tableBody.innerHTML = `
        <tr>
          <td colspan="7" class="table-placeholder">No inventory items found.</td>
        </tr>`;
      return;
    }

    tableBody.innerHTML = result.Item.map((item) => {
      const isTracking =
        item.TracksStock === "true" || item.TracksStock === true;
      const trackingBadge = isTracking
        ? `<span class="badge paid">Yes</span>`
        : `<span class="badge unpaid">No</span>`;

      return `
        <tr>
          <td>
          <a href="/Account/Inventory/ItemDetail.html?id=${UserId}&item_uuid=${
            item.ItemUUID
          }" class="supplier-link">
            ${escapeHtml(item.Name)}
          </a>
        </td>
          <td>${item.CategoryName || "Uncategorized"}</td>
                  <td>${item.LowStockThresold}</td>
          <td><span class="stock-value-empty">${item.CurrentStock}—</span></td>
          <td>${trackingBadge}</td>
          <td class="description-cell">${item.Description || "—"}</td>
          <td style="text-align: right;">
            <button class="btn-sm action-btn-secondary" title="Edit Item">Edit</button>
            <button class="btn-sm action-btn-danger" title="Delete Item">Delete</button>
          </td>
        </tr>
      `;
    }).join("");
  } catch (error) {
    console.error("Error fetching inventory stock:", error);
    tableBody.innerHTML = `
      <tr>
        <td colspan="7" class="table-placeholder error">Failed to load inventory data.</td>
      </tr>`;
  }
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL

  // Future check roles etc
  let AddButtonElement = document.getElementById("add_item_and_category");

  AddButtonElement.addEventListener("click", (event) =>
    AddItemAndCategory(userId),
  );

  TableStock(userId);
});


// Helper to escape HTML characters to prevent XSS
function escapeHtml(str) {
  if (!str) return "";
  return String(str)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}