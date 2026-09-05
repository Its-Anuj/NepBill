import {MeasurementUnits} from "/Inventory/AddItem.js";
// Global State Tracking

const PurchaseOrderState = {
  Supplier: { UUID: "", Name: "" },
  Items: [], // Holds full line item dynamic details
};

// Purchase Unit Enum Options
const PurchaseUnits = [
  { value: 0, label: "None" },
  { value: 1, label: "Bag" },
  { value: 2, label: "Box" },
  { value: 3, label: "Crate" },
  { value: 4, label: "Liter" },
  { value: 5, label: "Kg" },
  { value: 6, label: "Piece" },
];

let lineIdCounter = 0;

// API Fetchers
async function GetDesiredSuppliers(userId, filterName) {
  const response = await fetch(
    "/api/admin/inventorypurchaseorder/suppliersbyname/query",
    {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ AccountId: userId, Name: filterName }),
    },
  );
  return await response.json();
}

async function GetItemStockUnit(userId, filterName) {
  const response = await fetch("/api/business/inventory/itemstockunit/query", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ AccountId: userId, ItemName: filterName }),
  });
  return await response.json();
}

async function GetDesiredItemName(userId, filterName) {
  const response = await fetch("/api/admin/inventory/itembyname/query", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ AccountId: userId, Name: filterName }),
  });
  return await response.json();
}

// Supplier Setup
function SetupSupplierSection(userId) {
  const input = document.getElementById("order_supplier_name");
  const list = document.getElementById("suppliers-list");
  const err = document.getElementById("err_order_supplier_name");

  input.addEventListener("input", async () => {
    err.classList.remove("visible");
    PurchaseOrderState.Supplier = { UUID: "", Name: "" };

    const val = input.value.trim();
    if (!val) {
      list.style.display = "none";
      return;
    }

    const res = await GetDesiredSuppliers(userId, val);
    if (res && res.Count > 0 && res.Categories) {
      list.innerHTML = res.Categories.map(
        (c) => `<li data-uuid="${c.UUID}" data-name="${c.Name}">${c.Name}</li>`,
      ).join("");
      list.style.display = "block";
    } else {
      list.style.display = "none";
    }
  });

  list.addEventListener("click", (e) => {
    if (e.target.tagName === "LI") {
      const name = e.target.dataset.name;
      const uuid = e.target.dataset.uuid;

      input.value = name;
      PurchaseOrderState.Supplier = { UUID: uuid, Name: name };
      list.style.display = "none";
      err.classList.remove("visible");
    }
  });

  document.addEventListener("click", (e) => {
    if (!e.target.closest(".typeahead-wrapper")) list.style.display = "none";
  });
}

// Order Line Management
function AddItemRow(userId) {
  const container = document.getElementById("order_lines_container");
  const currentLineId = ++lineIdCounter;

  // Initialize line state
  PurchaseOrderState.Items.push({
    id: currentLineId,
    itemUUID: "",
    itemName: "",
    stockUnit: "Pcs",
    orderedQuantity: 1,
    purchaseUnit: 0,
    purchaseUnitLabel: "Unit",
    conversionRate: 1.0,
    unitPrice: 0.0,
    discountPercent: 0.0,
    remarks: "",
  });

  // Build Options for Purchase Unit Dropdown
  const unitOptionsHtml = PurchaseUnits.map(
    (u) => `<option value="${u.value}">${u.label}</option>`,
  ).join("");

  const rowHtml = `
    <div class="item-order-card" id="row_${currentLineId}" style="border: 1px solid #cbd5e1; padding: 16px; margin-bottom: 12px; border-radius: 8px;">
      <div class="form-grid" style="display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 12px;">
        
        <!-- Item Search -->
        <div class="form-group">
          <label>Item</label>
          <div class="typeahead-wrapper">
            <input type="text" class="input-item-search" id="item_search_${currentLineId}" placeholder="Search item..." autocomplete="off" />
            <ul class="typeahead-list" id="item_list_${currentLineId}"></ul>
          </div>
          <span class="error-msg" id="err_item_${currentLineId}">Select valid item</span>
        </div>

        <!-- Stock Unit (Auto-filled) -->
        <div class="form-group">
          <label>Stock Unit</label>
          <input type="text" id="item_stock_unit_${currentLineId}" readonly placeholder="--" style="background: #f1f5f9; cursor: not-allowed;" />
        </div>

        <!-- Quantity Ordered -->
        <div class="form-group">
          <label>Ordered Qty</label>
          <input type="number" class="input-item-qty" id="item_qty_${currentLineId}" value="1" min="1" />
          <span class="error-msg" id="err_qty_${currentLineId}">Qty &ge; 1</span>
        </div>

        <!-- Purchase Unit -->
        <div class="form-group">
          <label>Purchase Unit</label>
          <select id="item_purchase_unit_${currentLineId}">
            ${unitOptionsHtml}
          </select>
        </div>

        <!-- Conversion Factor -->
        <div class="form-group">
          <label id="lbl_conversion_${currentLineId}">Conversion Rate</label>
          <input type="number" step="0.01" min="0.01" id="item_conversion_${currentLineId}" value="1.0" placeholder="e.g. 25" />
        </div>

        <!-- Unit Price -->
        <div class="form-group">
          <label id="lbl_price_${currentLineId}">Price per Unit</label>
          <input type="number" class="input-item-price" id="item_price_${currentLineId}" placeholder="0.00" step="0.01" min="0" />
          <span class="error-msg" id="err_price_${currentLineId}">Required</span>
        </div>

        <!-- Discount % -->
        <div class="form-group">
          <label>Discount %</label>
          <input type="number" step="0.1" min="0" max="100" id="item_discount_${currentLineId}" value="0.0" />
        </div>

        <!-- Remarks -->
        <div class="form-group" style="grid-column: span 2;">
          <label>Remarks</label>
          <input type="text" id="item_remarks_${currentLineId}" placeholder="Optional line notes..." />
        </div>
      </div>

      <!-- Info Calculation Line & Row Actions -->
      <div style="display: flex; justify-content: space-between; align-items: center; margin-top: 12px; padding-top: 8px; border-top: 1px dashed #e2e8f0;">
        <div style="font-size: 0.875rem; color: #475569;" id="info_conversion_${currentLineId}">
          Total Stock Inventory: <strong>1 Pcs</strong> (1 Unit &times; 1)
        </div>
        
        <div style="display: flex; align-items: center; gap: 16px;">
          <div style="font-weight: 700; font-size: 1rem; color: #0f172a;" id="item_total_${currentLineId}">
            NPR 0.00
          </div>
          <button type="button" class="btn-icon-danger btn-remove-row" data-id="${currentLineId}">&minus;</button>
        </div>
      </div>
    </div>
  `;

  container.insertAdjacentHTML("beforeend", rowHtml);
  SetupRowListeners(userId, currentLineId);
}

function SetupRowListeners(userId, lineId) {
  const searchInput = document.getElementById(`item_search_${lineId}`);
  const list = document.getElementById(`item_list_${lineId}`);
  const stockUnitInput = document.getElementById(`item_stock_unit_${lineId}`);
  const qtyInput = document.getElementById(`item_qty_${lineId}`);
  const purchaseUnitSelect = document.getElementById(
    `item_purchase_unit_${lineId}`,
  );
  const conversionInput = document.getElementById(`item_conversion_${lineId}`);
  const priceInput = document.getElementById(`item_price_${lineId}`);
  const discountInput = document.getElementById(`item_discount_${lineId}`);
  const remarksInput = document.getElementById(`item_remarks_${lineId}`);

  const lblConversion = document.getElementById(`lbl_conversion_${lineId}`);
  const lblPrice = document.getElementById(`lbl_price_${lineId}`);
  const infoConversion = document.getElementById(`info_conversion_${lineId}`);
  const totalDisplay = document.getElementById(`item_total_${lineId}`);
  const removeBtn = document.querySelector(`#row_${lineId} .btn-remove-row`);

  const rowState = PurchaseOrderState.Items.find((i) => i.id === lineId);

  // Typeahead Item Search
  searchInput.addEventListener("input", async () => {
    document.getElementById(`err_item_${lineId}`).classList.remove("visible");
    rowState.itemUUID = "";
    rowState.itemName = "";

    const val = searchInput.value.trim();
    if (!val) {
      list.style.display = "none";
      return;
    }

    const res = await GetDesiredItemName(userId, val);
    const StockUnitRes = await GetItemStockUnit(userId, val);
    console.log(StockUnitRes);

    if (res && res.Count > 0 && res.Categories) {
      list.innerHTML = res.Categories.map(
        (c) =>
          `<li data-uuid="${c.UUID}" data-name="${c.Name}" data-stockunit="${c.StockUnitName || "Pcs"}" data-defconv="${c.DefaultPurchaseConversion || 1}" data-defunit="${c.DefaultPurchaseUnit || 0}">${c.Name}</li>`,
      ).join("");
      list.style.display = "block";
    } else {
      list.style.display = "none";
    }
  });

  list.addEventListener("click", async (e) => {
    if (e.target.tagName === "LI") {
      rowState.itemName = e.target.dataset.name;
      rowState.itemUUID = e.target.dataset.uuid;
      rowState.stockUnit = e.target.dataset.stockunit || "Pcs";

      searchInput.value = rowState.itemName;
      stockUnitInput.value = rowState.stockUnit;

      // Auto-fill defaults if present from item configuration
      if (e.target.dataset.defunit) {
        purchaseUnitSelect.value = e.target.dataset.defunit;
      }
      if (e.target.dataset.defconv) {
        conversionInput.value = e.target.dataset.defconv;
      }

      const StockUnitRes = await GetItemStockUnit(userId, searchInput.value);
      console.log(StockUnitRes);
      if(StockUnitRes["State"] == true)
      {
        console.log(MeasurementUnits[StockUnitRes["StockUnit"]]);
      }

      list.style.display = "none";
      updateCalculations();
    }
  });

  // Dynamic Calculation and Info Text Updater
  const updateCalculations = () => {
    const qty = parseFloat(qtyInput.value) || 0;
    const conversion = parseFloat(conversionInput.value) || 1;
    const unitPrice = parseFloat(priceInput.value) || 0;
    const discount = parseFloat(discountInput.value) || 0;

    const unitLabel =
      purchaseUnitSelect.options[purchaseUnitSelect.selectedIndex].text;

    rowState.orderedQuantity = qty;
    rowState.purchaseUnit = parseInt(purchaseUnitSelect.value, 10);
    rowState.purchaseUnitLabel = unitLabel;
    rowState.conversionRate = conversion;
    rowState.unitPrice = unitPrice;
    rowState.discountPercent = discount;
    rowState.remarks = remarksInput.value.trim();

    // Update dynamic UI labels based on chosen Purchase Unit
    lblConversion.textContent = `${rowState.stockUnit} per ${unitLabel}`;
    lblPrice.textContent = `Price per ${unitLabel}`;

    // Total Stock Units = Ordered Qty * Conversion Factor
    const totalStockQty = qty * conversion;
    infoConversion.innerHTML = `Total Stock Inventory: <strong>${totalStockQty} ${rowState.stockUnit}</strong> (${qty} ${unitLabel} &times; ${conversion} ${rowState.stockUnit}/${unitLabel})`;

    // Line Total Calculation with Discount: Total = Qty * UnitPrice * (1 - Discount/100)
    const rawTotal = qty * unitPrice;
    const finalTotal = rawTotal * (1 - discount / 100);
    totalDisplay.textContent = `NPR ${finalTotal.toFixed(2)}`;
  };

  // Event bindings for real-time recalculation
  qtyInput.addEventListener("input", updateCalculations);
  purchaseUnitSelect.addEventListener("change", updateCalculations);
  conversionInput.addEventListener("input", updateCalculations);
  priceInput.addEventListener("input", updateCalculations);
  discountInput.addEventListener("input", updateCalculations);
  remarksInput.addEventListener("input", () => {
    rowState.remarks = remarksInput.value.trim();
  });

  // Remove Row Handler
  removeBtn.addEventListener("click", () => {
    document.getElementById(`row_${lineId}`).remove();
    const idx = PurchaseOrderState.Items.findIndex((i) => i.id === lineId);
    if (idx !== -1) PurchaseOrderState.Items.splice(idx, 1);
  });
}

// Complete Order Validation & Submission
function ValidateAndSubmitPO(userId) {
  let isValid = true;

  // Validate Supplier
  const suppErr = document.getElementById("err_order_supplier_name");
  if (!PurchaseOrderState.Supplier.UUID) {
    suppErr.classList.add("visible");
    isValid = false;
  } else {
    suppErr.classList.remove("visible");
  }

  // Validate Items
  if (PurchaseOrderState.Items.length === 0) {
    alert("Please add at least one item line.");
    return;
  }

  PurchaseOrderState.Items.forEach((line) => {
    const errItem = document.getElementById(`err_item_${line.id}`);
    const errQty = document.getElementById(`err_qty_${line.id}`);
    const errPrice = document.getElementById(`err_price_${line.id}`);

    if (!line.itemUUID) {
      errItem.classList.add("visible");
      isValid = false;
    } else {
      errItem.classList.remove("visible");
    }

    if (line.orderedQuantity <= 0) {
      errQty.classList.add("visible");
      isValid = false;
    } else {
      errQty.classList.remove("visible");
    }

    if (line.unitPrice < 0 || line.unitPrice === "") {
      errPrice.classList.add("visible");
      isValid = false;
    } else {
      errPrice.classList.remove("visible");
    }
  });

  if (!isValid) return;

  // Payload Construction matching C++ PurchaseOrderLine API schema
  const payload = {
    AccountId: userId,
    SupplierUUID: PurchaseOrderState.Supplier.UUID,
    Lines: PurchaseOrderState.Items.map((item) => ({
      ItemUUID: item.itemUUID,
      OrderedQuantity: item.orderedQuantity,
      PurchaseUnit: item.purchaseUnit,
      PurchaseToStockConversion: item.conversionRate,
      UnitPrice: item.unitPrice,
      DiscountPercent: item.discountPercent,
      Remarks: item.remarks,
    })),
  };

  console.log("Submitting Purchase Order Payload:", payload);
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id");

  SetupSupplierSection(userId);

  // Add initial row by default
  AddItemRow(userId);

  document.getElementById("add_line_btn").addEventListener("click", () => {
    AddItemRow(userId);
  });

  document.getElementById("create_po_btn").addEventListener("click", () => {
    ValidateAndSubmitPO(userId);
  });
});
