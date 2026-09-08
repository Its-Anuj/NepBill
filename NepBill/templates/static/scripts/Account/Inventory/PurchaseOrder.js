import { escapeHtml } from "/index.js";

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id");

  const createPurchaseElement = document.getElementById(
    "create_purchase_order"
  );

  if (createPurchaseElement) {
    createPurchaseElement.addEventListener("click", () => {
      window.location.href = `/Account/Inventory/CreatePurchaseOrder.html?id=${userId}`;
    });
  }

  if (userId) {
    await LoadPurchaseOrders(userId);
  } else {
    console.error("User ID is missing in URL parameters.");
  }
});

async function FetchPurchaseOrders(userId) {
  try {
    const response = await fetch(
      "/api/business/inventory/purchaseorderlist/query",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ AccountId: userId }),
      }
    );

    if (!response.ok) {
      throw new Error(`HTTP Error Status: ${response.status}`);
    }

    const data = await response.json();
    return data;
  } catch (err) {
    console.error("Failed to fetch purchase orders:", err);
    return null;
  }
}

async function LoadPurchaseOrders(userId) {
  const container = document.getElementById("orders_table");
  container.innerHTML = `<p class="loading-text">Loading purchase orders...</p>`;

  const result = await FetchPurchaseOrders(userId);

  if (!result || !result.State) {
    container.innerHTML = `<p class="error-text">Failed to load purchase orders: ${
      result?.Message || "Unknown error"
    }</p>`;
    return;
  }

  if (!result.Orders || result.Count === 0) {
    container.innerHTML = `<p class="empty-text">No purchase orders found.</p>`;
    return;
  }

  // Construct HTML Table
  let tableHtml = `
    <table class="data-table">
      <thead>
        <tr>
          <th>Supplier Name</th>
          <th>State</th>
          <th>Created At</th>
          <th>Line Items</th>
          <th>Actions</th>
        </tr>
      </thead>
      <tbody>
  `;

  result.Orders.forEach((order) => {
    const stateClass = getStatusClass(order.State);

    tableHtml += `
      <tr>
        <td>
          <a href="/Account/Inventory/SupplierDetails.html?id=${userId}&supplier_uuid=${
      order.SupplierUUID
    }" class="supplier-link">
            ${escapeHtml(order.SupplierName)}
          </a>
        </td>
        <td>
          <span class="badge ${stateClass}">${escapeHtml(order.State)}</span>
        </td>
        <td>${escapeHtml(order.CreatedAt)}</td>
        <td>${order.LinesCount}</td>
        <td class="action-cell">
          <a href="/Account/Inventory/PurchaseOrderDetail.html?id=${userId}&order_id=${
      order.OrderUUID
    }" class="btn-action btn-view">View</a>
          <a href="/Account/Inventory/EditPurchaseOrder.html?id=${userId}&order_id=${
      order.OrderUUID
    }" class="btn-action btn-edit">Edit</a>
        </td>
      </tr>
    `;
  });

  tableHtml += `
      </tbody>
    </table>
  `;

  container.innerHTML = tableHtml;
}

// Optional helper for CSS styling badges based on order state
function getStatusClass(state) {
  switch (state?.toLowerCase()) {
    case "completed":
    case "received":
      return "badge-success";
    case "pending":
    case "draft":
      return "badge-warning";
    case "cancelled":
      return "badge-danger";
    default:
      return "badge-secondary";
  }
}