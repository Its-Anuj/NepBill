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

export function clearFieldError(inputId) {
  const inputElem = document.getElementById(inputId);
  const errElem = document.getElementById("err_" + inputId);
  if (inputElem) inputElem.classList.remove("input-error");
  if (errElem) errElem.classList.remove("visible");
}

export function showFieldError(inputId, message) {
  const inputElem = document.getElementById(inputId);
  const errElem = document.getElementById("err_" + inputId);
  if (inputElem) inputElem.classList.add("input-error");
  if (errElem) {
    if (message) errElem.textContent = message;
    errElem.classList.add("visible");
  }
}

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
  clearFieldError("item_input_name");
  clearFieldError("item_input_low_stock_threshold");
  clearFieldError("item_input_category");
  clearFieldError("item_input_description");
  clearFieldError("item_input_measurement_unit");

  let isValid = true;

  if (!CategoryName) {
    showFieldError(
      "item_input_category",
      "Please select or enter a category name.",
    );
    isValid = false;
  }

  if (!NameVal) {
    showFieldError("item_input_name", "Item name cannot be empty.");
    isValid = false;
  }

  if (LowStockThreshold === "") {
    showFieldError(
      "item_input_low_stock_threshold",
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

  const response = await fetch("/api/admin/inventory/itembyname/add", {
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
    alert(`${NameVal} successfuly added as a new item!`);
  } else {
    alert(`${NameVal} unsuccessfuly`);
  }
}

export async function AddItemCategory(UserId) {
  const categoryNameInput = document.getElementById("category_input_name");
  const NameVal = categoryNameInput.value.trim();

  clearFieldError("category_input_name");

  if (!NameVal) {
    showFieldError("category_input_name", "Category name cannot be empty.");
    return;
  }

  const response = await fetch("/api/admin/inventory/itemcateogrybyname/add", {
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
    alert(`${NameVal} successfuly added as a new item categroy!`);
  } else {
    alert(`${NameVal} unsuccessfuly`);
  }
}

export async function GetDesiredCategories(userId, FilterName) {
  const response = await fetch(
    "/api/admin/inventory/itemcateogrybyname/query",
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

  const result = await response.json();
  return result;
}

document.addEventListener("DOMContentLoaded", async () => {
  const urlParams = new URLSearchParams(window.location.search);
  const userId = urlParams.get("id"); // Retrieves the 'id' parameter from the URL

  // Clear error states on user typing
  const fieldsToTrack = [
    "category_input_name",
    "item_input_category",
    "item_input_name",
    "item_input_low_stock_threshold",
    "item_input_description",
    "item_input_measurement_unit",
  ];

  fieldsToTrack.forEach((id) => {
    const el = document.getElementById(id);
    if (el) {
      el.addEventListener("input", () => clearFieldError(id));
    }
  });

  // Future check roles etc
  let AddItemElement = document.getElementById("add_item");

  AddItemElement.addEventListener("click", (event) => AddItem(userId));

  let AddCategoryElement = document.getElementById("add_item_category");

  AddCategoryElement.addEventListener("click", (event) =>
    AddItemCategory(userId),
  );

  const unitSelect = document.getElementById("item_input_measurement_unit");

  Object.entries(MeasurementUnits).forEach(([value, label]) => {
    const option = document.createElement("option");
    option.value = value;
    option.textContent = label;
    unitSelect.appendChild(option);
  });

  const input = document.getElementById("item_input_category");
  const list = document.getElementById("category-list");

  function render(matches) {
    console.log(matches);
    list.innerHTML = matches
      .map((c) => `<li data-value="${c["Name"]}">${c["Name"]}</li>`)
      .join("");
    list.style.display = matches.length ? "block" : "none";
  }

  input.addEventListener("input", async () => {
    console.log(input.value.toLowerCase());
    const q = input.value.toLowerCase();
    let DesiredCategories = await GetDesiredCategories(
      userId,
      input.value.toLowerCase(),
    );
    console.log(DesiredCategories);

    if (DesiredCategories["Count"] > 0) {
      render(
        DesiredCategories["Categories"].filter((c) =>
          c["Name"].toLowerCase().includes(q),
        ),
      );
    }
  });

  list.addEventListener("click", (e) => {
    if (e.target.tagName === "LI") {
      input.value = e.target.dataset.value;
      list.style.display = "none";
      clearFieldError("item_input_category");
    }
  });

  // Close on outside click
  document.addEventListener("click", (e) => {
    if (!e.target.closest(".typeahead-wrapper")) list.style.display = "none";
  });
});
