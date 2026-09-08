export async function FetchPurchaseOrderDetail(UserId, OrderId) {
  try {
    const response = await fetch(
      "/api/business/inventory/purchaseorderdetail/query",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ AccountId: UserId, OrderId: OrderId }),
      },
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

export async function UpdatePurchaseOrderState(
  UserId,
  OrderId,
  LineReceivedQuantities,
  ItemInvoiceInfo,
  ChangeStateId,
) {
  try {
    const response = await fetch(
      "/api/business/inventory/purchaseorder/changeorderstate",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          // Print gathered values to console
          AccountId: UserId,
          OrderId: OrderId,
          LineReceivedQuantity: LineReceivedQuantities,
          InvoiceInfo: ItemInvoiceInfo,
          ChangeStateId: ChangeStateId,
        }),
      },
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
