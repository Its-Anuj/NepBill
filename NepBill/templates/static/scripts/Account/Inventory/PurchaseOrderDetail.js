import { PurchaseOrderStateMap } from "/Account/Inventory/PurchaseOrderEnum.js";
import { FetchPurchaseOrderDetail } from "/Account/Inventory/BasicQueries.js";

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const UserId = urlParams.get("id");
  const OrderId = urlParams.get("order_id");

  const backBtn = document.getElementById("btn_back_to_orders");
  if (backBtn) {
    backBtn.href = `/Account/Inventory/PurchaseOrder.html?id=${UserId}`;
  }

  if (!UserId || !OrderId) {
    document.getElementById("po_loading").innerText =
      "Error: Invalid Order or User ID.";
    return;
  }

  const data = await FetchPurchaseOrderDetail(UserId, OrderId);
  RenderPurchaseOrderDetail(data, UserId, OrderId);
});

let areAllCollapsed = false;

function RenderPurchaseOrderDetail(data, UserId, OrderId) {
  const loadingEl = document.getElementById("po_loading");
  const containerEl = document.getElementById("po_detail_container");

  if (!data || data.State === undefined) {
    loadingEl.innerText = "Failed to load purchase order details.";
    return;
  }

  loadingEl.style.display = "none";
  containerEl.style.display = "grid";

  const isDraft =
    data.State === PurchaseOrderStateMap.Draft ||
    String(data.PurchaseOrderState).toLowerCase() === "draft";

  // Left Section Info
  const supplierLink = document.getElementById("po_supplier_link");
  supplierLink.innerText = data.SupplierName || "Unknown Supplier";
  supplierLink.href = `/Account/Inventory/SupplierDetails.html?id=${UserId}&supplier_uuid=${data.SupplierUUID}`;

  const stateBadge = document.getElementById("po_state_badge");
  stateBadge.innerHTML = `<span class="badge ${getStatusBadgeClass(data.PurchaseOrderState)}">${data.PurchaseOrderState}</span>`;

  document.getElementById("po_created_at").innerText = data.CreatedAt || "N/A";
  document.getElementById("po_lines_count").innerText = data.LinesCount || 0;

  // Setup State Change Interface
  setupStateChangeControls(UserId, OrderId, data.State, isDraft);

  // Render Right Section Lines
  const linesWrapper = document.getElementById("po_lines_wrapper");
  linesWrapper.innerHTML = "";

  let grandAssumedTotal = 0;
  let grandActualTotal = 0;

  if (data.Lines && data.Lines.length > 0) {
    data.Lines.forEach((line, index) => {
      const conversionRate = parseFloat(line.PurchaseToStockConversion) || 1;
      const orderedQty = parseFloat(line.OrderedQuantity) || 0;
      const receivedQty = parseFloat(line.ReceivedQuantity) || 0;

      const totalOrderedStock = orderedQty * conversionRate;
      const totalReceivedStock = receivedQty * conversionRate;

      const unitPrice = parseFloat(line.UnitPrice) || 0;
      const discount = parseFloat(line.DiscountPercent) || 0;

      // Calculate Assumed Price (from Ordered Qty)
      const lineAssumedRaw = unitPrice * orderedQty;
      const lineAssumedTotal = lineAssumedRaw * (1 - discount / 100);

      // Calculate Actual Price (from Received Qty)
      const lineActualRaw = unitPrice * receivedQty;
      const lineActualTotal = lineActualRaw * (1 - discount / 100);

      grandAssumedTotal += lineAssumedTotal;
      grandActualTotal += lineActualTotal;

      const lineCard = document.createElement("div");
      lineCard.className = "line-item-card";
      lineCard.id = `line_card_${index}`;

      // Calculation breakdown strings
      const mathConversionStr = `${conversionRate} ${escapeHtml(line.StockUnit)} per 1 ${escapeHtml(line.PurchaseUnit)} &times; ${orderedQty} ${escapeHtml(line.PurchaseUnit)} = ${totalOrderedStock} ${escapeHtml(line.StockUnit)}`;

      const mathPriceStr = isDraft
        ? `(NPR ${unitPrice.toFixed(2)} &times; ${orderedQty}) - ${discount}% off = NPR ${lineAssumedTotal.toFixed(2)}`
        : `Actual: NPR ${lineActualTotal.toFixed(2)} / Assumed: NPR ${lineAssumedTotal.toFixed(2)}`;

      const displayLineTotal = isDraft ? lineAssumedTotal : lineActualTotal;

      lineCard.innerHTML = `
        <div class="line-item-header" onclick="toggleLineCard(${index})">
          <div class="line-item-title">
            <span class="line-number">${index + 1}</span>
            <div style="display: flex; flex-direction: column;">
              <a href="/Account/Inventory/ItemDetails.html?id=${UserId}&item_uuid=${line.ItemUUID}" 
                 class="entity-link" 
                 onclick="event.stopPropagation();">
                ${escapeHtml(line.ItemName)}
              </a>
              <span style="font-size: 0.75rem; color: var(--text-muted); font-weight: normal; margin-top: 2px;">
                ${mathConversionStr}
              </span>
            </div>
          </div>
          <div style="display: flex; align-items: center; gap: 16px;">
            <div style="text-align: right;">
              <div style="font-weight: 700; color: var(--text-main);">NPR ${displayLineTotal.toFixed(2)}</div>
              <div style="font-size: 0.75rem; color: var(--text-muted); font-weight: normal;">${mathPriceStr}</div>
            </div>
            <span class="toggle-icon" id="toggle_icon_${index}">&black25;</span>
          </div>
        </div>

        <div class="line-item-body">
          <div class="meta-group">
            <div class="meta-label">Received / Ordered Qty</div>
            <div class="meta-value">${receivedQty} / ${orderedQty} ${escapeHtml(line.PurchaseUnit)}</div>
          </div>

          <div class="meta-group">
            <div class="meta-label">Stock Converted Qty</div>
            <div class="meta-value">${totalReceivedStock} / ${totalOrderedStock} ${escapeHtml(line.StockUnit)}</div>
          </div>

          <div class="meta-group">
            <div class="meta-label">Conversion Rate</div>
            <div class="meta-value">1 ${escapeHtml(line.PurchaseUnit)} = ${conversionRate} ${escapeHtml(line.StockUnit)}</div>
          </div>

          <div class="meta-group">
            <div class="meta-label">Actual / Assumed Price</div>
            <div class="meta-value">NPR ${lineActualTotal.toFixed(2)} / NPR ${lineAssumedTotal.toFixed(2)}</div>
          </div>

          <div class="meta-group" style="grid-column: span 2;">
            <div class="meta-label">Remarks</div>
            <div class="meta-value">${escapeHtml(line.Remarks) || "None"}</div>
          </div>
        </div>
      `;

      linesWrapper.appendChild(lineCard);
    });
  } else {
    linesWrapper.innerHTML = `<div style="padding: 16px; color: var(--text-muted);">No item lines found.</div>`;
  }

  // Update Left Window Total Box with Actual vs Assumed Price
  const grandTotalEl = document.getElementById("po_grand_total");
  if (grandTotalEl) {
    if (isDraft) {
      grandTotalEl.innerText = `NPR ${grandAssumedTotal.toFixed(2)}`;
    } else {
      grandTotalEl.innerHTML = `
        <span style="font-size: 1.1rem; color: #2563eb;">NPR ${grandActualTotal.toFixed(2)}</span>
        <span style="font-size: 0.8rem; color: var(--text-muted); font-weight: normal; margin-left: 6px;">
          (Assumed: NPR ${grandAssumedTotal.toFixed(2)})
        </span>
      `;
    }
  }

  // Render Item Invoice Summary Box ONLY if NOT Draft
  if (!isDraft) {
    renderItemInvoiceSummary(linesWrapper, data, grandActualTotal);
  }

  // Attach Global Collapse Button
  const toggleAllBtn = document.getElementById("btn_toggle_all_lines");
  if (toggleAllBtn) {
    toggleAllBtn.addEventListener("click", () => {
      areAllCollapsed = !areAllCollapsed;
      const cards = document.querySelectorAll(".line-item-card");
      cards.forEach((card, idx) => {
        if (areAllCollapsed) {
          card.classList.add("collapsed");
          document.getElementById(`toggle_icon_${idx}`).innerHTML = "&#9654;";
        } else {
          card.classList.remove("collapsed");
          document.getElementById(`toggle_icon_${idx}`).innerHTML = "&black25;";
        }
      });
      toggleAllBtn.innerText = areAllCollapsed ? "Expand All" : "Collapse All";
    });
  }
}

function renderItemInvoiceSummary(containerEl, data, subtotal) {
  // Query non-draft invoice fields
  const invoiceId = data.InvoiceId || data.ItemInvoiceId || "N/A";
  const invoiceState = data.InvoiceState || "N/A";
  const vatPercent = parseFloat(data.VatPercent || data.Vatpercent) || 0;

  const vatAmount = subtotal * (vatPercent / 100);
  const grandTotalWithVat = subtotal + vatAmount;

  // Check or remove existing summary box to prevent duplication
  let invoiceBox = document.getElementById("po_invoice_summary_box");
  if (!invoiceBox) {
    invoiceBox = document.createElement("div");
    invoiceBox.id = "po_invoice_summary_box";
    invoiceBox.className = "invoice-summary-card";
    invoiceBox.style.cssText = `
      margin-top: 24px;
      padding: 20px;
      background: var(--bg-card, #ffffff);
      border: 1px solid var(--border-color, #e2e8f0);
      border-radius: 8px;
    `;
    containerEl.appendChild(invoiceBox);
  }

  invoiceBox.innerHTML = `
    <h3 style="margin-bottom: 16px; font-size: 1.05rem; font-weight: 700; color: var(--text-main);">
      Item Invoice Summary
    </h3>
    <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 16px;">
      <div class="meta-group">
        <div class="meta-label">Invoice ID</div>
        <div class="meta-value">${escapeHtml(invoiceId)}</div>
      </div>
      <div class="meta-group">
        <div class="meta-label">Invoice State</div>
        <div class="meta-value">
          <span class="badge ${getStatusBadgeClass(invoiceState)}">${escapeHtml(invoiceState)}</span>
        </div>
      </div>
      <div class="meta-group">
        <div class="meta-label">VAT Rate</div>
        <div class="meta-value">${vatPercent.toFixed(2)}%</div>
      </div>
      <div class="meta-group">
        <div class="meta-label">Subtotal</div>
        <div class="meta-value">NPR ${subtotal.toFixed(2)}</div>
      </div>
      <div class="meta-group">
        <div class="meta-label">VAT Amount</div>
        <div class="meta-value">NPR ${vatAmount.toFixed(2)}</div>
      </div>
      <div class="meta-group">
        <div class="meta-label">Grand Total (Incl. VAT)</div>
        <div class="meta-value" style="font-size: 1.15rem; font-weight: 700; color: #2563eb;">
          NPR ${grandTotalWithVat.toFixed(2)}
        </div>
      </div>
    </div>
  `;
}

function setupStateChangeControls(UserId, OrderId, currentState, isDraft) {
  const editBtn = document.getElementById("btn_edit_order");
  if (editBtn) editBtn.style.display = "none";

  const changeStateBtn = document.getElementById("btn_change_state");
  const saveBtn = document.getElementById("btn_save_order");
  if (saveBtn) saveBtn.style.display = "none";

  if (!changeStateBtn) return;

  // ONLY allow action if state is Draft
  if (!isDraft) {
    changeStateBtn.disabled = true;
    changeStateBtn.style.cursor = "not-allowed";
    changeStateBtn.style.opacity = "0.6";
    changeStateBtn.title = "Order state is locked and cannot be changed after leaving Draft state.";
    return;
  }

  // Create state transition popover container
  let stateMenu = document.getElementById("state_change_popover");
  if (!stateMenu) {
    stateMenu = document.createElement("div");
    stateMenu.id = "state_change_popover";
    stateMenu.style.cssText = `
      display: none;
      margin-top: 10px;
      padding: 12px;
      background: var(--bg-card, #ffffff);
      border: 1px solid var(--border-color, #e2e8f0);
      border-radius: 8px;
      box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1);
      flex-direction: column;
      gap: 8px;
    `;

    stateMenu.innerHTML = `
      <div style="font-size: 0.75rem; font-weight: 700; text-transform: uppercase; color: var(--text-muted); margin-bottom: 4px;">
        Select New State:
      </div>
      <button class="btn-secondary" style="text-align: left; justify-content: flex-start;" onclick="redirectToStateChange('${UserId}', '${OrderId}', ${PurchaseOrderStateMap.PartiallyReceived})">
        🟡 Partially Received (${PurchaseOrderStateMap.PartiallyReceived})
      </button>
      <button class="btn-secondary" style="text-align: left; justify-content: flex-start;" onclick="redirectToStateChange('${UserId}', '${OrderId}', ${PurchaseOrderStateMap.Completed})">
        🟢 Completed (${PurchaseOrderStateMap.Completed})
      </button>
      <button class="btn-secondary" style="text-align: left; justify-content: flex-start; color: #b91c1c; border-color: #fee2e2;" onclick="redirectToStateChange('${UserId}', '${OrderId}', ${PurchaseOrderStateMap.Cancelled})">
        🔴 Cancelled (${PurchaseOrderStateMap.Cancelled})
      </button>
    `;

    changeStateBtn.parentNode.insertBefore(stateMenu, changeStateBtn.nextSibling);
  }

  // Toggle options menu visibility
  changeStateBtn.onclick = () => {
    const isHidden = stateMenu.style.display === "none";
    stateMenu.style.display = isHidden ? "flex" : "none";
  };
}

// Redirect Helper
window.redirectToStateChange = function (UserId, OrderId, stateInt) {
  window.location.href = `/Account/Inventory/ChangePurchaseOrderState.html?id=${UserId}&order_id=${OrderId}&state=${stateInt}`;
};

// Window helper for card collapsing
window.toggleLineCard = function (index) {
  const card = document.getElementById(`line_card_${index}`);
  const icon = document.getElementById(`toggle_icon_${index}`);
  if (card) {
    card.classList.toggle("collapsed");
    const isCollapsed = card.classList.contains("collapsed");
    icon.innerHTML = isCollapsed ? "&#9654;" : "&black25;";
  }
};

function getStatusBadgeClass(state) {
  if (!state) return "badge-secondary";

  switch (String(state).toLowerCase()) {
    case "draft":
      return "badge-secondary";
    case "sent":
      return "badge-primary";
    case "partiallyreceived":
      return "badge-warning";
    case "completed":
      return "badge-success";
    case "cancelled":
      return "badge-danger";
    case "fullypaid":
      return "badge-success";
    case "paying":
      return "badge-warning";
    case "dropped":
      return "badge-danger";
    default:
      return "badge-secondary";
  }
}

function escapeHtml(str) {
  if (!str) return "";
  return String(str)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}