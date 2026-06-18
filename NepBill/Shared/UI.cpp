#include <iostream>
#include <sstream>
#include <string>
#include "Tables.h"
#include "UI.h"
#include "imgui.h"

namespace NepBill
{
    namespace ClientCpp
    {
        void UIDashboard(UIApp &UI, App &App)
        {
            // Top navigation buttons
            if (ImGui::Button("Home"))
            {
                UI.Section = DashboardSection::Home;
            }

            ImGui::SameLine();

            if (ImGui::Button("Orders"))
            {
                UI.Section = DashboardSection::Order;
            }

            ImGui::SameLine();

            if (ImGui::Button("Suppliers"))
            {
                UI.Section = DashboardSection::Suppliers;
            }

            ImGui::SameLine();

            if (ImGui::Button("Payment Tickets"))
            {
                UI.Section = DashboardSection::PaymentTicket;
            }

            ImGui::SameLine();

            if (ImGui::Button("Items"))
            {
                UI.Section = DashboardSection::Items;
            }

            ImGui::SameLine();

            if (ImGui::Button("Invoices"))
            {
                UI.Section = DashboardSection::Invoices;
            }

            ImGui::Separator();

            switch (UI.Section)
            {
            case DashboardSection::Home:
                /* code */
                UIHomeSection(UI, App);
                break;
            case DashboardSection::Invoices:
                /* code */
                UIInvoiceSection(UI, App);
                break;
            case DashboardSection::Order:
                /* code */
                UIOrderSection(UI, App);
                break;
            case DashboardSection::PaymentTicket:
                /* code */
                UIPaymentTicketSection(UI, App);
                break;
            case DashboardSection::Suppliers:
                /* code */
                UISupplierSection(UI, App);
                break;
            case DashboardSection::Items:
                /* code */
                UIItemSection(UI, App);
                break;
            default:
                break;
            }
        }

        void UIHomeSection(UIApp &UI, App &App)
        {
        }

        void UIItemSection(UIApp &UI, App &App)
        {
            ImGui::SeparatorText("Items");

            if (UI.ItemInfo.UpdateList == true)
            {
                ItemQuery Query;

                UI.ItemInfo.ShowItems = GetItems(App, Query);
                UI.ItemInfo.UpdateList = false;
            }

            if (UI.ItemInfo.UpdateCategoryList == true)
            {
                ItemCategoryQuery Query;
                Query.Limit = UINT32_MAX;
                UI.ItemInfo.Categories = GetItemCategories(App, Query);
                UI.ItemInfo.UpdateCategoryList = false;
            }

            if (ImGui::Button("Register Item"))
            {
                // TODO:
                UI.ItemInfo.Section = ItemSection::RegisterItem;
            }
            if (ImGui::Button("Register Category"))
            {
                // TODO:
                UI.ItemInfo.Section = ItemSection::RegisterCategory;
            }
            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("ItemsTable", 6, Flags, ImVec2(0.0f, 450.0f))) // Adjusted to 6 columns to match your setups

            {
                ImGui::TableSetupColumn("Item UUID");
                ImGui::TableSetupColumn("Category");
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("CostPrice");
                ImGui::TableSetupColumn("DiscountPrice");
                ImGui::TableSetupColumn("SalesPrice");

                for (int i = 0; i < UI.ItemInfo.ShowItems.size(); i++)
                {
                    /* code */
                }

                ImGui::TableHeadersRow();

                ImGui::EndTable(); // <-- FIXED: This prevents the scroll context leakage!
            }

            switch (UI.ItemInfo.Section)
            {
            case ItemSection::RegisterItem:
            {
                /* code */
                ImGui::Begin("Register Item");

                // ---- Name ----
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::InputText("##Name", UI.ItemInfo.NewItem.Name.data(), UI.ItemInfo.NewItem.Name.size());
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("LowStockThreshold");
                ImGui::NextColumn();
                ImGui::InputInt("##LowStockThreshold", (int *)&UI.ItemInfo.NewItem.LowStockThresold);
                ImGui::NextColumn();

                if (ImGui::BeginCombo(
                        "Category",
                        UI.ItemInfo.SelectedCategory >= 0
                            ? UI.ItemInfo.Categories[UI.ItemInfo.SelectedCategory].Name.data()
                            : "Select Category"))
                {
                    ImGui::InputText(
                        "Search",
                        UI.ItemInfo.SearchBuffer,
                        sizeof(UI.ItemInfo.SearchBuffer));

                    ImGui::Separator();

                    for (int i = 0; i < UI.ItemInfo.Categories.size(); i++)
                    {
                        const char *Name =
                            UI.ItemInfo.Categories[i].Name.data();

                        if (strlen(UI.ItemInfo.SearchBuffer) > 0)
                        {
                            if (strstr(Name, UI.ItemInfo.SearchBuffer) == nullptr)
                                continue;
                        }

                        bool Selected =
                            (UI.ItemInfo.SelectedCategory == i);

                        if (ImGui::Selectable(Name, Selected))
                        {
                            UI.ItemInfo.SelectedCategory = i;
                        }

                        if (Selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                ImGui::Text("CostPrice");
                ImGui::NextColumn();
                ImGui::InputDouble("##CostPrice", &UI.ItemInfo.NewItem.CostPrice);
                ImGui::NextColumn();

                ImGui::Text("DiscountPercent");
                ImGui::NextColumn();
                ImGui::InputDouble("##DiscountPercent", &UI.ItemInfo.NewItem.DiscountPercent);
                ImGui::NextColumn();

                std::string Sales = "SalesPrice: " + std::to_string(UI.ItemInfo.NewItem.CostPrice * (100 - UI.ItemInfo.NewItem.DiscountPercent) / 100);
                ImGui::Text(Sales.c_str());
                ImGui::NextColumn();

                ImGui::Text("Description");
                ImGui::NextColumn();
                ImGui::InputText("##Description", UI.ItemInfo.NewItem.Description.data(), UI.ItemInfo.NewItem.Description.size());
                ImGui::NextColumn();

                if (ImGui::Button("Register"))
                {
                    if (UI.ItemInfo.SelectedCategory == -1)
                    {
                        UI.ItemInfo.ShowSelectedCategoryError = true;
                    }
                    else
                    {
                        UI.ItemInfo.ShowSelectedCategoryError = false;
                        UI.ItemInfo.NewItem.CategoryId = UI.ItemInfo.Categories[UI.ItemInfo.SelectedCategory].UniqueID;
                        Insert(App, UI.ItemInfo.NewItem);

                        UI.ItemInfo.NewItem = Item();
                        UI.ItemInfo.UpdateList = true;
                        UI.ItemInfo.Section = ItemSection::None;
                        UI.ItemInfo.SelectedCategory = -1;
                        std::memset(UI.ItemInfo.SearchBuffer, 0, sizeof(UI.ItemInfo.SelectedCategory));
                    }
                }
            }
            case ItemSection::RegisterCategory:
            {
                /* code */
                ImGui::Begin("Register Category");

                // ---- Name ----
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::InputText("##Name", UI.ItemInfo.NewItemCategory.Name.data(), UI.ItemInfo.NewItemCategory.Name.size());
                ImGui::NextColumn();

                if (ImGui::Button("Register"))
                {
                    Insert(App, UI.ItemInfo.NewItemCategory);

                    UI.ItemInfo.NewItemCategory = ItemCategory();
                    UI.ItemInfo.UpdateCategoryList = true;
                    UI.ItemInfo.Section = ItemSection::None;
                }

                ImGui::End();
                break;
            }
            }
            if (UI.ItemInfo.ShowSelectedCategoryError)
            {
                if (ImGui::Begin("Error no selected category", &UI.ItemInfo.ShowSelectedCategoryError))
                {
                }
                ImGui::End();
            }

            ImGui::End();
        }

        void UIInvoiceSection(UIApp &UI, App &App)
        {
        }

        void UIPaymentTicketSection(UIApp &UI, App &App)
        {
        }

        void UIOrderSection(UIApp &UI, App &App)
        {
            ImGui::SeparatorText("Orders");

            if (UI.OrderInfo.UpdateList == true)
            {
                InvoiceQuery Query;
                Query.Limit = 25;

                UI.OrderInfo.ShowInvoices = GetInvoices(App, Query);
            }

            if (ImGui::Button("Create Order"))
            {
                // TODO:
                UI.OrderInfo.Section = OrderSection::CreateOrder;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("OrdersTable", 7, Flags,
                                  ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Order UUID");
                ImGui::TableSetupColumn("Customer");
                ImGui::TableSetupColumn("Total");
                ImGui::TableSetupColumn("Remaining");
                ImGui::TableSetupColumn("Created");
                ImGui::TableSetupColumn("Last Paid");
                ImGui::TableSetupColumn("Status");

                ImGui::TableHeadersRow();

                for (int i = 0; i < UI.OrderInfo.ShowInvoices.size(); i++)
                {
                    const char *CustomerName =
                        (i % 3 == 0) ? "John Doe" : (i % 3 == 1) ? "Alice Smith"
                                                                 : "Bob Johnson";

                    double TotalAmount = 1000.0 + (i * 500.0);

                    double Remaining =
                        (i % 4 == 0) ? 0.0 : (i % 4 == 1) ? TotalAmount * 0.25
                                         : (i % 4 == 2)   ? TotalAmount * 0.50
                                                          : TotalAmount;

                    InvoiceStates Status =
                        Remaining == 0.0
                            ? InvoiceStates::FullyPaid
                        : Remaining == TotalAmount
                            ? InvoiceStates::Paying
                            : InvoiceStates::PartiallyPaid;

                    ImGui::TableNextRow();

                    // UUID
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Selectable(
                        ("INV-" + std::to_string(1000 + i)).c_str(),
                        false,
                        ImGuiSelectableFlags_SpanAllColumns);

                    // Customer
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(CustomerName);

                    // Total
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("Rs %.2f", TotalAmount);

                    // Remaining
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("Rs %.2f", Remaining);

                    // Created
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("2026-08-%02d", (i % 28) + 1);

                    // Last Paid
                    ImGui::TableSetColumnIndex(5);

                    if (Remaining < TotalAmount)
                        ImGui::Text("2026-08-%02d", (i % 28) + 2);
                    else
                        ImGui::TextUnformatted("-");

                    // Status
                    ImGui::TableSetColumnIndex(6);

                    switch (Status)
                    {
                    case InvoiceStates::Paying:
                        ImGui::TextUnformatted("Unpaid");
                        break;

                    case InvoiceStates::PartiallyPaid:
                        ImGui::TextUnformatted("Partially Paid");
                        break;

                    case InvoiceStates::FullyPaid:
                        ImGui::TextUnformatted("Paid");
                        break;

                    case InvoiceStates::Dropped:
                        ImGui::TextUnformatted("Dropped");
                        break;
                    }
                }

                ImGui::EndTable();

                switch (UI.OrderInfo.Section)
                {
                case OrderSection::CreateOrder:
                    ImGui::Begin("Create Order");

                    // Ask for

                    ImGui::End();

                    break;

                default:
                    break;
                }
            }
        }

        void UISupplierSection(UIApp &UI, App &App)
        {
        }

    } // namespace ClientCpp

} // namespace NepBill
