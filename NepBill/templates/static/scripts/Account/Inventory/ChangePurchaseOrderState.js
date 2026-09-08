import {
  PurchaseOrderStateMap,
  PurchaseOrderStateToStr,
} from "/Account/Inventory/PurchaseOrderEnum.js";
import { FetchPurchaseOrderDetail, UpdatePurchaseOrderState } from "/Account/Inventory/BasicQueries.js";

let currentLinesData = [];

function sanitizeId(str) {
  return String(str || "item").replace(/[^a-zA-Z0-9_-]/g, "_");
}

function updateLineCalculations(index, lineDetail) {
  const safeItemName = sanitizeId(lineDetail.ItemName || index);
  const recvQtyInput = document.getElementById(
    `order_line_received_quantity_${safeItemName}`,
  );
  const ordQtyInput = document.getElementById(
    `order_line_ordered_quantity_${safeItemName}`,
  );
  const recvStockInput = document.getElementById(
    `order_line_received_stock_quantity_${safeItemName}`,
  );
  const actualPriceInput = document.getElementById(
    `order_line_received_actual_price_${safeItemName}`,
  );

  let receivedQty = parseFloat(recvQtyInput.value) || 0;
  const orderedQty = parseFloat(ordQtyInput.value) || 0;

  // Validation: Received quantity must be <= Ordered quantity
  if (receivedQty > orderedQty) {
    receivedQty = orderedQty;
    recvQtyInput.value = orderedQty;
  } else if (receivedQty < 0) {
    receivedQty = 0;
    recvQtyInput.value = 0;
  }

  // Update Stock Converted Qty
  const conversionRate = parseFloat(lineDetail.PurchaseToStockConversion) || 1;
  const totalReceivedStock = receivedQty * conversionRate;
  recvStockInput.value = totalReceivedStock.toFixed(2);

  // Update Line Actual Price (UnitPrice * RecvQty * (1 - Discount/100))
  const unitPrice = parseFloat(lineDetail.UnitPrice) || 0;
  const discountPercent = parseFloat(lineDetail.DiscountPercent) || 0;
  const actualPrice = unitPrice * receivedQty * (1 - discountPercent / 100);
  actualPriceInput.value = actualPrice.toFixed(2);

  // Recalculate Invoice Summary Total
  recalculateInvoiceSummary();
}

function recalculateInvoiceSummary() {
  let subtotal = 0;

  currentLinesData.forEach((lineDetail, idx) => {
    const safeItemName = sanitizeId(lineDetail.ItemName || idx);
    const actualPriceInput = document.getElementById(
      `order_line_received_actual_price_${safeItemName}`,
    );
    if (actualPriceInput) {
      subtotal += parseFloat(actualPriceInput.value) || 0;
    }
  });

  const vatPercentInput = document.getElementById("inv_vat_percent");
  const vatPercent = parseFloat(vatPercentInput.value) || 0;
  const grandTotal = subtotal * (1 + vatPercent / 100);

  document.getElementById("inv_subtotal").innerText =
    `NPR ${subtotal.toFixed(2)}`;
  document.getElementById("inv_grand_total").innerText =
    `NPR ${grandTotal.toFixed(2)}`;
}

async function HandleAllLines(UserId, OrderId, ChangeStateId) {
  const lineDetailsData = await FetchPurchaseOrderDetail(UserId, OrderId);

  if (!lineDetailsData || !lineDetailsData.Lines) {
    console.error("Failed to fetch order details or lines empty.");
    return;
  }

  currentLinesData = lineDetailsData.Lines;

  // Set Supplier Info
  document.getElementById("inv_supplier_name").innerText =
    lineDetailsData.SupplierName || "N/A";

  const linesContainer = document.getElementById("show_changeable_lines");
  const templateEl = document.getElementById("template_order_line_show");
  linesContainer.innerHTML = "";

  const paymentStateSelect = document.getElementById("inv_payment_state");
  const bannerEl = document.getElementById("state_info_banner");

  // Configure screen UI state
  if (ChangeStateId === PurchaseOrderStateMap.Cancelled) {
    bannerEl.innerText =
      "State: CANCELLED (All quantities set to 0. Invoice state locked to Dropped)";
    bannerEl.style.color = "#b91c1c";
    paymentStateSelect.value = "Dropped";
    paymentStateSelect.disabled = true;
  } else if (ChangeStateId === PurchaseOrderStateMap.Completed) {
    bannerEl.innerText =
      "State: COMPLETED (Received quantities matched to ordered amounts)";
    bannerEl.style.color = "#15803d";
    paymentStateSelect.disabled = false;
  } else {
    bannerEl.innerText =
      "State: PARTIALLY RECEIVED (Adjust received quantities as needed)";
    bannerEl.style.color = "#b45309";
    paymentStateSelect.disabled = false;
  }

  // Render cards
  lineDetailsData.Lines.forEach((lineDetail, index) => {
    const safeItemName = sanitizeId(lineDetail.ItemName || index);
    const clone = templateEl.cloneNode(true);

    clone.removeAttribute("id");
    clone.style.display = "block";

    // Item Header Labels
    clone.querySelector(".lbl-item-name").innerText =
      lineDetail.ItemName || "N/A";
    clone.querySelector(".lbl-purchase-unit").innerText =
      lineDetail.PurchaseUnit || "Unit";
    clone.querySelector(".lbl-stock-unit").innerText =
      lineDetail.StockUnit || "Unit";
    clone.querySelector(".lbl-remarks-box").innerText =
      lineDetail.Remarks || "No remarks provided.";

    // Assign uniquely formatted element IDs
    const recvQtyInput = clone.querySelector(".input-received-qty");
    recvQtyInput.id = `order_line_received_quantity_${safeItemName}`;

    const ordQtyInput = clone.querySelector(".input-ordered-qty");
    ordQtyInput.id = `order_line_ordered_quantity_${safeItemName}`;

    const recvStockInput = clone.querySelector(".input-received-stock-qty");
    recvStockInput.id = `order_line_received_stock_quantity_${safeItemName}`;

    const ordStockInput = clone.querySelector(".input-ordered-stock-qty");
    ordStockInput.id = `order_line_ordered_stock_quantity_${safeItemName}`;

    const convRateInput = clone.querySelector(".input-conversion-rate");
    convRateInput.id = `order_line_ordered_conversion_rate_${safeItemName}`;

    const actualPriceInput = clone.querySelector(".input-actual-price");
    actualPriceInput.id = `order_line_received_actual_price_${safeItemName}`;

    const assumedPriceInput = clone.querySelector(".input-assumed-price");
    assumedPriceInput.id = `order_line_received_assumed_price_${safeItemName}`;

    // Populate Initial Values
    const orderedQty = parseFloat(lineDetail.OrderedQuantity) || 0;
    const conversionRate =
      parseFloat(lineDetail.PurchaseToStockConversion) || 1;
    const unitPrice = parseFloat(lineDetail.UnitPrice) || 0;
    const discountPercent = parseFloat(lineDetail.DiscountPercent) || 0;

    const orderedStock = orderedQty * conversionRate;
    const assumedPrice = unitPrice * orderedQty * (1 - discountPercent / 100);

    ordQtyInput.value = orderedQty;
    ordStockInput.value = orderedStock;
    convRateInput.value = conversionRate;
    assumedPriceInput.value = assumedPrice.toFixed(2);

    // Apply logic depending on the targeted state
    if (ChangeStateId === PurchaseOrderStateMap.Cancelled) {
      recvQtyInput.value = 0;
      recvQtyInput.readOnly = true;
    } else if (ChangeStateId === PurchaseOrderStateMap.Completed) {
      recvQtyInput.value = orderedQty;
      recvQtyInput.readOnly = true;
    } else {
      // PartiallyReceived: Allow user input, default to received quantity if present or 0
      recvQtyInput.value = parseFloat(lineDetail.ReceivedQuantity) || 0;
      recvQtyInput.readOnly = false;
    }

    // Attach real-time calculation handler
    recvQtyInput.addEventListener("input", () =>
      updateLineCalculations(index, lineDetail),
    );

    linesContainer.appendChild(clone);

    // Initial calculation trigger for the line
    updateLineCalculations(index, lineDetail);
  });

  document
    .getElementById("inv_vat_percent")
    .addEventListener("input", recalculateInvoiceSummary);
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const UserId = urlParams.get("id");
  const OrderId = urlParams.get("order_id");
  const stateString = urlParams.get("state");
  const ChangeStateId = stateString !== null ? parseInt(stateString, 10) : 0;

  if (!UserId || !OrderId) {
    document.getElementById("show_changeable_lines").innerText =
      "Invalid order parameters.";
    return;
  }

  await HandleAllLines(UserId, OrderId, ChangeStateId);

  // Submit Handler// Submit Handler
  document
    .getElementById("btn_submit_state_change")
    .addEventListener("click", async () => {
      const urlParams = new URLSearchParams(window.location.search);
      const accountId = urlParams.get("id");
      const orderId = urlParams.get("order_id");

      // Collect received quantity for each item line into an array
      const lineReceivedQuantities = currentLinesData.map(
        (lineDetail, index) => {
          const safeItemName = sanitizeId(lineDetail.ItemName || index);
          const recvQtyInput = document.getElementById(
            `order_line_received_quantity_${safeItemName}`,
          );
          return parseFloat(recvQtyInput?.value) || 0;
        },
      );

      // Build the invoice item state info array / object
      const invoiceInfo = {
        paymentState:
          document.getElementById("inv_payment_state")?.value || "FullyPaid",
        vatPercent:
          parseFloat(document.getElementById("inv_vat_percent")?.value) || 0,
        subtotal:
          parseFloat(
            document
              .getElementById("inv_subtotal")
              ?.innerText.replace("NPR ", ""),
          ) || 0,
        grandTotal:
          parseFloat(
            document
              .getElementById("inv_grand_total")
              ?.innerText.replace("NPR ", ""),
          ) || 0,
      };

      // Print gathered values to console
      console.log("Account ID:", accountId);
      console.log("Order ID:", orderId);
      console.log("Line Received Quantities:", lineReceivedQuantities);
      console.log("Invoice Info:", invoiceInfo);
      await UpdatePurchaseOrderState(accountId, orderId, lineReceivedQuantities, invoiceInfo, ChangeStateId);
    });
});
