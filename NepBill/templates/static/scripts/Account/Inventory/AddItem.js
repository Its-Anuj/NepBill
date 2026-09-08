import { ShowFieldError, ClearFieldError} from "/ErrorHandler.js";
// Map matching C++ backend MeasurementUnit enum
export const MeasurementUnits = {
  0: "Kg",
  1: "Gram",
  2: "Liter",
  3: "Ml",
  4: "Piece",
  5: "Dozen",
  6: "Bag",
  7: "Box",
  8: "Meter",
  9: "None",
  10: "Other",
};

export async function AddItem(UserId) {
  const nameElem = document.getElementById("item_input_name");
  const thresholdElem = document.getElementById(
    "item_input_low_stock_threshold",
  );
  const categoryElem = document.getElementById("item_input_category");
  const descElem = document.getElementById("item_input_description");

  const NameVal = nameElem.value.trim();
  const LowStockThreshold = thresholdElem.value.trim();
  const TrackStock = document.getElementById("item_input_track_stock").checked;
  const Description = descElem.value.trim();
  const CategoryName = categoryElem.value.trim();
  const Unit = document.getElementById("item_input_measurement_unit");

  // Reset errors
  ClearFieldError("item_input_name", "err_item_input_name", "input-error");
  ClearFieldError("item_input_low_stock_threshold", "err_item_input_low_stock_threshold", "input-error");
  ClearFieldError("item_input_category", "err_item_input_category", "input-error");
  ClearFieldError("item_input_description", "err_item_input_description", "input-error");
  ClearFieldError("item_input_measurement_unit", "err_item_input_measurement_unit", "input-error");

  let isValid = true;

  if (!CategoryName) {
    ShowFieldError(
      "item_input_category",
      "err_item_input_category",
      "input-errpr",
      "Please select or enter a category name.",
    );
    isValid = false;
  }

  if (!NameVal) {
    ShowFieldError("item_input_name", 
      "err_item_input_name",
      "input-errpr",
      "Item name cannot be empty.");
    isValid = false;
  }

  if (LowStockThreshold === "") {
    ShowFieldError(
      "item_input_low_stock_threshold", 
      "err_item_input_name",
      "input-errpr",
      "Please enter a low stock threshold.",
    );
    isValid = false;
  }

  if (!isValid) return;

  console.log({
    AccountId: UserId,
    NameVal: NameVal,
    LowStockThreshold: LowStockThreshold,
    TrackStock: TrackStock,
    Description: Description,
    CategoryName: CategoryName,
    MeasurementUnit: Unit.value,
  });

  const response = await fetch("/api/business/inventory/item/add", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: UserId,
      ItemName: NameVal,
      StockThreshold: LowStockThreshold,
      TrackStockBool: TrackStock,
      Desc: Description,
      ItemCategoryName: CategoryName,
      MeasurementUnit: Unit.value,
    }),
  });

  const result = await response.json();
  console.log(result);

  if (result["State"] == true) {
    alert(result["Message"]);
    categoryNameInput.value = "";
  } else {
    alert(result["Message"]);
  }
}

export async function AddItemCategory(UserId) {
  const categoryNameInput = document.getElementById("category_input_name");
  const NameVal = categoryNameInput.value.trim();

  ClearFieldError("category_input_name");

  if (!NameVal) {
    ShowFieldError("category_input_name", "Category name cannot be empty.");
    return;
  }

  const response = await fetch("/api/business/inventory/itemcategory/add", {
    method: "POST",

    headers: {
      "Content-Type": "application/json",
    },

    body: JSON.stringify({
      AccountId: UserId,
      NewCategoryName: NameVal,
    }),
  });

  const result = await response.json();
  console.log(result);

  if (result["State"] == true) {
    alert(result["Message"]);
    categoryNameInput.value = "";
  } else {
    alert(result["Message"]);
  }
}

export async function GetDesiredCategories(userId, FilterName) {
  console.log(FilterName);
  const response = await fetch(
    "/api/business/inventory/itemcategory/query/byname",
    {
      method: "POST",

      headers: {
        "Content-Type": "application/json",
      },

      body: JSON.stringify({
        AccountId: userId,
        CategoryName: FilterName,
      }),
    },
  );

  console.log(FilterName);
  const result = await response.json();
  console.log(result);
  return result;
}
