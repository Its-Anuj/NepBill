import { escapeHtml } from "/index.js";

document.addEventListener("DOMContentLoaded", async () => {
  const container = document.getElementById("ItemDetailContainer");
  if (!container) return;

  const urlParams = new URLSearchParams(window.location.search);
  const itemUuid = urlParams.get("item_uuid");
  const userId = urlParams.get("id");

  if (!itemUuid) {
    container.innerHTML = `<div class="error-msg">Item ID is missing.</div>`;
    return;
  }

  try {
    const response = await fetch("/api/business/inventory/itemdetail/query", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ ItemId: itemUuid, AccountId: userId }),
    });

    const data = await response.json();

    if (!data.State) {
      container.innerHTML = `<div class="error-msg">${escapeHtml(data.Message || "Failed to load item details.")}</div>`;
      return;
    }

    renderItemDetails(container, data);
  } catch (err) {
    console.error("Error fetching item details:", err);
    container.innerHTML = `<div class="error-msg">An error occurred while fetching item details.</div>`;
  }
});

function renderItemDetails(container, item) {
  const tracksStockText = item.TracksStock ? "Yes" : "No";
  const purchaseUnit = item.DefaultPurchaseUnit || item.Name || "N/A"; // Handles old C++ payload key fallback

  container.innerHTML = `
    <!-- LEFT SIDE: ITEM DETAILS -->
    <div class="item-card">
      <div class="item-card-title">Item Overview</div>
      <div class="item-detail-grid">
        <div class="detail-group full-width">
          <span class="detail-label">Item Name</span>
          <span class="detail-value">${escapeHtml(item.Name || "-")}</span>
        </div>

        <div class="detail-group">
          <span class="detail-label">Category</span>
          <span class="detail-value">${escapeHtml(item.CategoryName || "Uncategorized")}</span>
        </div>

        <div class="detail-group">
          <span class="detail-label">Tracks Stock</span>
          <span class="detail-value">${tracksStockText}</span>
        </div>

        <div class="detail-group">
          <span class="detail-label">Stock Unit</span>
          <span class="detail-value">${escapeHtml(item.StockUnit || "-")}</span>
        </div>

        <div class="detail-group">
          <span class="detail-label">Low Stock Threshold</span>
          <span class="detail-value">${item.LowStockThresold ?? item.LowStockThreshold ?? 0} ${escapeHtml(item.StockUnit || "")}</span>
        </div>

        <div class="detail-group">
          <span class="detail-label">Default Purchase Unit</span>
          <span class="detail-value">${escapeHtml(purchaseUnit)}</span>
        </div>

        <div class="detail-group">
          <span class="detail-label">Conversion Rate</span>
          <span class="detail-value">1 ${escapeHtml(purchaseUnit)} = ${item.DefaultPurchaseConversion || 1} ${escapeHtml(item.StockUnit || "")}</span>
        </div>

        <div class="detail-group full-width">
          <span class="detail-label">Description</span>
          <span class="detail-value">${escapeHtml(item.Description || "No description provided.")}</span>
        </div>
      </div>
    </div>

    <!-- RIGHT SIDE: STOCK LEDGERS -->
    <div class="item-card">
      <div class="item-card-title">Stock Ledgers & Movement</div>
      <p style="font-size: 0.85rem; color: var(--text-muted); margin-bottom: 12px;">
        History of stock adjustments, purchase orders, and sales for this item.
      </p>

      <!-- PLACEHOLDER FOR STOCK LEDGERS TABLE -->
      <div id="StockLedgerTableContainer">
        <div class="ledger-empty-state">
          <div style="font-weight: 600; margin-bottom: 4px;">No Stock Movements Yet</div>
          <div style="font-size: 0.8rem;">Stock changes, purchase order receipts, and audit logs for this item will appear here.</div>
        </div>
      </div>
    </div>
  `;
}