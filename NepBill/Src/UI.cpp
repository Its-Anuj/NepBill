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
            UI.LastSection = UI.Section;
            // Top navigation buttons
            if (ImGui::Button("Dashboard"))
            {
                UI.Section = DashboardSection::Dashboard;
            }

            ImGui::SameLine();

            if (ImGui::Button("Rooms"))
            {
                UI.Section = DashboardSection::Rooms;
            }

            ImGui::SameLine();

            if (ImGui::Button("Bookings"))
            {
                UI.Section = DashboardSection::Bookings;
            }

            ImGui::SameLine();

            if (ImGui::Button("Suppliers"))
            {
                UI.Section = DashboardSection::Suppliers;
            }

            ImGui::SameLine();

            if (ImGui::Button("Payment Tickets"))
            {
                UI.Section = DashboardSection::PaymentTickets;
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

            ImGui::SameLine();

            if (ImGui::Button("Staffs"))
            {
                UI.Section = DashboardSection::Staffs;
            }

            ImGui::Separator();

            switch (UI.Section)
            {
            case DashboardSection::Dashboard:
                /* code */

                // Show Info
                ImGui::Text("Dashboard!");

                break;
            case DashboardSection::Bookings:
                UIBookingSection(UI, App);
                break;
            case DashboardSection::Rooms:
                /* code */
                UIRoomSection(UI, App);
                break;
            case DashboardSection::Staffs:
                /* code */
                break;
            case DashboardSection::Invoices:
                UInvoiceection(UI, App);
                break;
            case DashboardSection::PaymentTickets:
                /* code */
                break;
            case DashboardSection::Suppliers:
                /* code */
                break;
            case DashboardSection::Items:
                UItemSection(UI, App);
                break;
            default:
                break;
            }

            if (!UI.ErrorMessage.empty())
            {
                ImGui::PushStyleColor(
                    ImGuiCol_Text,
                    ImVec4(1, 0, 0, 1));

                ImGui::TextWrapped(
                    "%s",
                    UI.ErrorMessage.c_str());

                ImGui::PopStyleColor();
            }
        }

        std::string TmToStr(const tm &Time)
        {
            std::string Date = std::to_string(Time.tm_year);

            Date += ":" + std::to_string(Time.tm_mon);
            Date += ":" + std::to_string(Time.tm_mday);
            Date += ":" + std::to_string(Time.tm_hour);
            Date += ":" + std::to_string(Time.tm_min);
            Date += ":" + std::to_string(Time.tm_sec);

            return Date;
        }

        void UIRoomSection(UIApp &UI, App &App)
        {
            ImGui::SeparatorText("Rooms");

            if (UI.Room.UpdateList == true)
            {
                RoomQuery Query;

                UI.Room.ShowRooms = GetRooms(App, Query);
                UI.Room.ShowExtraInfo.clear();

                for (auto &Room : UI.Room.ShowRooms)
                {
                    UIRoomInfo::RoomExtraInfo ExtraInfo;

                    RoomFacilityQuery FacilityQuery;
                    FacilityQuery.RoomID = Room.UniqueID;
                    auto Facilities = GetRoomFacilities(App, FacilityQuery);

                    double RoomPrice = Room.BasePrice;
                    for (auto &Facility : Facilities)
                        RoomPrice += Facility.FacilityPrice;
                    ExtraInfo.RoomPrice = RoomPrice;
                    UI.Room.ShowExtraInfo.push_back(ExtraInfo);
                }

                UI.Room.UpdateList = false;
            }

            if (UI.LastSection != DashboardSection::Rooms)
            {
                // Our current section newly checked
                UI.Room.Section = RoomSection::None;
            }

            if (ImGui::Button("Register Room"))
            {
                // TODO:
                UI.Room.Section = RoomSection::RegisterRoom;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("RoomsTable", 6, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("BasePrice");
                ImGui::TableSetupColumn("RoomPrice");
                ImGui::TableSetupColumn("Bed Count");
                ImGui::TableSetupColumn("State");
                ImGui::TableSetupColumn("Actions");

                ImGui::TableHeadersRow();

                uint32_t Idx = 0;
                for (const auto &Room : UI.Room.ShowRooms)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(Room.Name.data());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.2f", Room.BasePrice);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%.2f", UI.Room.ShowExtraInfo[Idx].RoomPrice);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%u", Room.BedCount);

                    ImGui::TableSetColumnIndex(4);

                    const char *StateText = "Unknown";

                    switch (Room.State)
                    {
                    case RoomStates::Available:
                        StateText = "Available";
                        break;

                    case RoomStates::Taken:
                        StateText = "Taken";
                        break;

                    case RoomStates::Unavailable:
                        StateText = "Unavailable";
                        break;
                    }

                    ImGui::TextUnformatted(StateText);

                    ImGui::TableSetColumnIndex(5);

                    ImGui::PushID(Room.Id);

                    if (ImGui::Button("Edit"))
                    {
                        UI.Room.Section = RoomSection::EditRoom;
                        UI.Room.EditRoom = Room;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("View Facility"))
                    {
                        UI.Room.Section = RoomSection::ViewRoomFacility;
                        UI.Room.FacilityActiveRoom = Room;
                    }

                    ImGui::PopID();
                    Idx++;
                }

                ImGui::EndTable();
            }

            switch (UI.Room.Section)
            {
            case RoomSection::RegisterRoom:
            {
                /* code */
                ImGui::Begin("Register Room");

                // ---- Name ----
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::InputText("##Name", UI.Room.NewRoom.Name.data(), UI.Room.NewRoom.Name.size());
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("BasePrice");
                ImGui::NextColumn();
                ImGui::InputDouble("##BasePrice", (double *)&UI.Room.NewRoom.BasePrice);
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("BedCount");
                ImGui::NextColumn();
                ImGui::InputInt("##BedCount", (int *)&UI.Room.NewRoom.BedCount);
                ImGui::NextColumn();

                // ---- Name ----
                ImGui::Text("Description");
                ImGui::NextColumn();
                ImGui::InputText("##Description", UI.Room.NewRoom.Description.data(), UI.Room.NewRoom.Description.size());
                ImGui::NextColumn();

                // Combo box
                if (ImGui::BeginCombo("##State", RoomStateStr[(int)UI.Room.NewRoom.State]))
                {
                    for (int i = 0; i < (int)RoomStates::Count; i++)
                    {
                        bool isSelected = ((int)UI.Room.NewRoom.State == i);

                        if (ImGui::Selectable(RoomStateStr[i], isSelected))
                            UI.Room.NewRoom.State = (RoomStates)i;

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Register"))
                {
                    Insert(App, UI.Room.NewRoom);

                    UI.Room.NewRoom = RoomInfo();
                    UI.Room.UpdateList = true;
                    UI.Room.Section = RoomSection::None;
                }
                ImGui::End();
                break;
            }
            case RoomSection::EditRoom:
            {
                /* code */
                ImGui::Begin("Edit Room");

                // ---- Name ----
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::InputText("##Name", UI.Room.EditRoom.Name.data(), UI.Room.EditRoom.Name.size());
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("BasePrice");
                ImGui::NextColumn();
                ImGui::InputDouble("##BasePrice", (double *)&UI.Room.EditRoom.BasePrice);
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("BedCount");
                ImGui::NextColumn();
                ImGui::InputInt("##BedCount", (int *)&UI.Room.EditRoom.BedCount);
                ImGui::NextColumn();

                // ---- Name ----
                ImGui::Text("Description");
                ImGui::NextColumn();
                ImGui::InputText("##Description", UI.Room.EditRoom.Description.data(), UI.Room.EditRoom.Description.size());
                ImGui::NextColumn();

                // Combo box
                if (ImGui::BeginCombo("##State", RoomStateStr[(int)UI.Room.EditRoom.State]))
                {
                    for (int i = 0; i < (int)RoomStates::Count; i++)
                    {
                        bool isSelected = ((int)UI.Room.EditRoom.State == i);

                        if (ImGui::Selectable(RoomStateStr[i], isSelected))
                            UI.Room.EditRoom.State = (RoomStates)i;

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Edit"))
                {
                    UpdateRoomInfo(App, UI.Room.EditRoom.Id, UI.Room.EditRoom);

                    UI.Room.EditRoom = RoomInfo();
                    UI.Room.UpdateList = true;
                    UI.Room.Section = RoomSection::None;
                }
                ImGui::End();
                break;
            }
            case RoomSection::None:
            {
                break;
            }
            case RoomSection::ViewRoomFacility:
            {
                if (UIRoomFacilitySection(UI, App, UI.Room.FacilityActiveRoom) == false)
                    UI.Room.Section = RoomSection::None;

                break;
            }
            }
            UI.Room.LastSection = UI.Room.Section;
        }

        bool UIRoomFacilitySection(UIApp &UI, App &App, RoomInfo &ActiveRoom)
        {
            std::string RoomFacilityName = "Room Facility: " + std::string(ActiveRoom.Name.data());

            if (UI.Room.Facility.LastRoomId != ActiveRoom.Id)
                UI.Room.Facility.ShowWindow = true;

            if (UI.Room.Facility.ShowWindow == false)
            {
                UI.Room.Facility.LastRoomId = UINT32_MAX;
                return false;
            }

            ImGui::Begin(RoomFacilityName.c_str(), &UI.Room.Facility.ShowWindow);

            ImGui::SeparatorText("Rooms");

            if (UI.Room.Facility.LastRoomId != ActiveRoom.Id)
                UI.Room.Facility.UpdateList = true;

            if (UI.Room.Facility.UpdateList == true)
            {
                RoomFacilityQuery Query;
                Query.RoomID = ActiveRoom.UniqueID;

                UI.Room.Facility.Show = GetRoomFacilities(App, Query);
                UI.Room.Facility.UpdateList = false;
            }

            if (UI.Room.LastSection != RoomSection::ViewRoomFacility)
            {
                // Our current section newly checked
                UI.Room.Facility.Section = RoomFacilitySection::None;
            }

            if (ImGui::Button("Register Facility"))
            {
                // TODO:
                UI.Room.Facility.Section = RoomFacilitySection::RegisterRoomFacility;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("FacilityTable", 3, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Info");
                ImGui::TableSetupColumn("PricePerDay");
                ImGui::TableSetupColumn("Actions");

                ImGui::TableHeadersRow();

                for (const auto &Facility : UI.Room.Facility.Show)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(Facility.FacilityInfo.data());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.2f", Facility.FacilityPrice);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushID(Facility.Id);

                    if (ImGui::Button("Edit"))
                    {
                        UI.Room.Facility.Section = RoomFacilitySection::EditRoomFacility;
                        UI.Room.Facility.Edit = Facility;
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }

            switch (UI.Room.Facility.Section)
            {
            case RoomFacilitySection::RegisterRoomFacility:
            {
                ImGui::Begin("Register Facility");

                // ---- Name ----
                ImGui::Text("Info");
                ImGui::NextColumn();
                ImGui::InputText("##Info", UI.Room.Facility.New.FacilityInfo.data(), UI.Room.Facility.New.FacilityInfo.size());
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("Price Per Day");
                ImGui::NextColumn();
                ImGui::InputDouble("##Price", (double *)&UI.Room.Facility.New.FacilityPrice);
                ImGui::NextColumn();

                if (ImGui::Button("Register"))
                {
                    UI.Room.Facility.New.RoomID = ActiveRoom.UniqueID;
                    Insert(App, UI.Room.Facility.New);

                    UI.Room.Facility.New = RoomFacilityInfo();
                    UI.Room.Facility.UpdateList = true;
                    UI.Room.Facility.Section = RoomFacilitySection::None;
                    UI.Room.UpdateList = true;
                }

                ImGui::End();
                break;
            }
            case RoomFacilitySection::EditRoomFacility:
            {
                ImGui::Begin("Edit Facility");

                // ---- Name ----
                ImGui::Text("Info");
                ImGui::NextColumn();
                ImGui::InputText("##Info", UI.Room.Facility.Edit.FacilityInfo.data(), UI.Room.Facility.Edit.FacilityInfo.size());
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("Price Per Day");
                ImGui::NextColumn();
                ImGui::InputDouble("##Price", (double *)&UI.Room.Facility.Edit.FacilityPrice);
                ImGui::NextColumn();

                if (ImGui::Button("Update"))
                {
                    UpdateRoomFacilityInfo(App, UI.Room.Facility.Edit.Id, UI.Room.Facility.Edit);

                    UI.Room.Facility.Edit = RoomFacilityInfo();
                    UI.Room.Facility.UpdateList = true;
                    UI.Room.Facility.Section = RoomFacilitySection::None;
                    UI.Room.UpdateList = true;
                }

                ImGui::End();
            }
            case RoomFacilitySection::None:
            {
                break;
            }
            }

            ImGui::End();
            UI.Room.Facility.LastRoomId = ActiveRoom.Id;
            return true;
        }

        bool ImGuiLink(const char *label)
        {
            ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "%s", label);

            if (ImGui::IsItemHovered())
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                if (ImGui::IsMouseClicked(0))
                    return true;
            }

            return false;
        }

        void UIBookingSection(UIApp &UI, App &App)
        {
            ImGui::SeparatorText("Bookings");

            if (UI.Booking.UpdateList == true)
            {
                BookingQuery Query;

                UI.Booking.Show = GetBookings(App, Query);
                UI.Booking.UpdateList = false;

                for (auto &Booking : UI.Booking.Show)
                {
                    RoomQuery RoomQuery;
                    RoomQuery.UniqueID = Booking.RoomID;

                    UI.Booking.BookingRooms.push_back(GetRooms(App, RoomQuery)[0]);
                }
            }

            if (UI.LastSection != DashboardSection::Bookings)
            {
                // Our current section newly checked
                UI.Booking.Section = BookingInfoSection::None;
                RoomQuery RoomQuery;
                RoomQuery.State = RoomStates::Available;

                UI.Booking.FreeRooms = GetRooms(App, RoomQuery);
                UI.Booking.FreeRoomInfo.clear();

                for (int i = 0; i < UI.Booking.FreeRooms.size(); i++)
                {
                    UI.Booking.FreeRoomInfo.push_back(UI.Booking.FreeRooms[i].Name.data());
                    ;
                }
            }

            if (ImGui::Button("Create New Booking"))
            {
                // TODO:
                UI.Booking.Section = BookingInfoSection::CreateNewBooking;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("BookingsTable", 6, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Rooms");
                ImGui::TableSetupColumn("CreatedAt");
                ImGui::TableSetupColumn("CheckIn");
                ImGui::TableSetupColumn("CheckOut");
                ImGui::TableSetupColumn("Status");
                ImGui::TableSetupColumn("Actions");

                ImGui::TableHeadersRow();

                uint32_t idx = 0;
                for (const auto &Booking : UI.Booking.Show)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(UI.Booking.BookingRooms[idx].Name.data());

                    ImGui::TableSetColumnIndex(1);
                    auto BsDate = NepaliDateConverter::toNepaliTm(Booking.CreatedAt);
                    std::string DateStr = TmToStr(BsDate);
                    ImGui::TextUnformatted(DateStr.c_str());

                    ImGui::TableSetColumnIndex(2);

                    if (Booking.CheckIn.tm_year == 0)
                    {
                        ImGui::TextUnformatted("Not Checked In!");
                    }
                    else
                    {
                        auto BsCheckIn =
                            NepaliDateConverter::toNepaliTm(Booking.CheckIn);

                        std::string BsCheckInStr =
                            TmToStr(BsCheckIn);

                        ImGui::TextUnformatted(BsCheckInStr.c_str());
                    }

                    ImGui::TableSetColumnIndex(3);

                    if (Booking.CheckOut.tm_year == 0)
                    {
                        ImGui::TextUnformatted("Not Checked Out!");
                    }
                    else
                    {
                        auto BsCheckOut =
                            NepaliDateConverter::toNepaliTm(Booking.CheckOut);

                        std::string BsCheckOutStr =
                            TmToStr(BsCheckOut);

                        ImGui::TextUnformatted(BsCheckOutStr.c_str());
                    }

                    ImGui::TableSetColumnIndex(4);

                    if (Booking.CheckOut.tm_year == 0)
                    {
                        ImGui::TextUnformatted("Not Checked Out!");
                    }
                    else
                    {
                        auto BsCheckOut =
                            NepaliDateConverter::toNepaliTm(Booking.CheckOut);

                        std::string BsCheckOutStr =
                            TmToStr(BsCheckOut);

                        ImGui::TextUnformatted(BsCheckOutStr.c_str());
                    }

                    ImGui::TableSetColumnIndex(5);

                    if (ImGui::Button(("Edit##" + std::to_string(Booking.Id)).c_str()))
                    {
                        UI.Booking.Edit = Booking;
                        UI.Booking.Section = BookingInfoSection::EditBooking;
                    }
                    ImGui::SameLine();

                    if (ImGui::Button(("CheckIn##" + std::to_string(Booking.Id)).c_str()))
                    {
                        if (Booking.CheckIn.tm_year == 0)
                        {
                            UI.Booking.Edit = Booking;
                            UI.Booking.Section = BookingInfoSection::CheckInBooking;
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button(("CheckOut##" + std::to_string(Booking.Id)).c_str()))
                    {
                        if (Booking.CheckOut.tm_year == 0)
                        {
                            UI.Booking.Edit = Booking;
                            UI.Booking.Section = BookingInfoSection::CheckOutBooking;
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button(("View Additional Costs##" + std::to_string(Booking.Id)).c_str()))
                    {
                        UI.Booking.Edit = Booking;
                        UI.Booking.Section = BookingInfoSection::ViewBookingAdditionalCost;
                    }

                    idx++;
                }

                ImGui::EndTable();
            }

            switch (UI.Booking.Section)
            {
            case BookingInfoSection::CreateNewBooking:
            {
                /* code */
                ImGui::Begin("Create Booking");

                // ---- Phone ----
                ImGui::Text("GuestCount");
                ImGui::NextColumn();
                ImGui::InputInt("##GuestCount", (int *)&UI.Booking.NewBookingGuestCount);
                ImGui::NextColumn();

                if (UI.Booking.NewBookingGuestCount != UI.Booking.BookingGuests.size())
                    UI.Booking.BookingGuests.resize(UI.Booking.NewBookingGuestCount);

                for (size_t i = 0; i < UI.Booking.NewBookingGuestCount; i++)
                {
                    std::string GuestIdentifier = "Guest" + std::to_string(i);
                    ImGui::Text(GuestIdentifier.c_str());
                    ImGui::Spacing();
                    // ---- Name ----
                    std::string NameIdentifier = "##Name" + std::to_string(i);
                    ImGui::Text("Name");
                    ImGui::NextColumn();
                    ImGui::InputText(NameIdentifier.c_str(), UI.Booking.BookingGuests[i].Name.data(), UI.Booking.BookingGuests[i].Name.size());
                    ImGui::NextColumn();

                    // ---- Phone ----
                    std::string PhnneNumberIdentifier = "##PhnneNumber" + std::to_string(i);
                    ImGui::Text("PhnneNumber");
                    ImGui::NextColumn();
                    ImGui::InputText(PhnneNumberIdentifier.c_str(), UI.Booking.BookingGuests[i].PhoneNumber.data(), UI.Booking.BookingGuests[i].PhoneNumber.size());
                    ImGui::NextColumn();

                    static int selectedCountry = 0;

                    std::string CountryIdentifier = "##Country" + std::to_string(i);
                    // Combo box
                    if (ImGui::BeginCombo(CountryIdentifier.c_str(), NepBill::Countries[selectedCountry]))
                    {
                        for (int i = 0; i < NepBill::Countries.size(); i++)
                        {
                            bool isSelected = (selectedCountry == i);

                            if (ImGui::Selectable(NepBill::Countries[i], isSelected))
                                selectedCountry = i;

                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    strncpy(UI.Booking.BookingGuests[i].Country.data(),
                            NepBill::Countries[selectedCountry],
                            UI.Booking.BookingGuests[i].Country.size());
                }

                ImGui::Text("Select Room:");
                ImGui::SameLine();

                static int selectedCountry = 0;

                bool Combo = false;
                if (UI.Booking.FreeRooms.size() == 0)
                {
                    Combo = ImGui::BeginCombo("##Room", "NoRooms");
                }
                else
                {
                    Combo = ImGui::BeginCombo("##Room", UI.Booking.FreeRooms[selectedCountry].Name.data());
                    UI.Booking.New.RoomID = UI.Booking.FreeRooms[selectedCountry].UniqueID;
                }

                // Combo box
                if (Combo)
                {
                    for (int i = 0; i < UI.Booking.FreeRooms.size(); i++)
                    {
                        bool isSelected = (selectedCountry == i);

                        if (ImGui::Selectable(UI.Booking.FreeRooms[i].Name.data(), isSelected))
                        {
                            selectedCountry = i;
                            UI.Booking.New.RoomID = UI.Booking.FreeRooms[i].UniqueID;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Register"))
                {
                    // First Guest Pass

                    if (UI.Booking.BookingGuests.size() == 0)
                    {
                        UI.ErrorMessage = "Guest is required";
                        return;
                    }

                    for (auto &Guest : UI.Booking.BookingGuests)
                    {
                        GuestQuery GuestQuery;
                        GuestQuery.UniqueID = Guest.UniqueID;
                        auto GuestVector = GetGuests(App, GuestQuery);

                        // we insert this new guest
                        if (GuestVector.size() == 0)
                        {
                            Insert(App, Guest);
                        }

                        BookingGuestTable GuestTable;
                        GuestTable.GuestID = Guest.UniqueID;
                        GuestTable.BookingID = UI.Booking.New.UniqueID;
                        Insert(App, GuestTable);
                    }

                    RoomQuery RoomQuery;
                    RoomQuery.UniqueID = UI.Booking.New.RoomID;
                    auto Rooms = GetRooms(App, RoomQuery);
                    Rooms[0].State = RoomStates::Taken;
                    UpdateRoomInfo(App, Rooms[0].Id, Rooms[0]);
                    UI.Room.UpdateList = true;

                    UI.Booking.New.CreatedAt = GetCurrentTime();

                    Insert(App, UI.Booking.New);

                    UI.Booking.Section = BookingInfoSection::None;
                    UI.Booking.UpdateList = true;
                    UI.Booking.New = BookingInfo();
                    UI.Booking.BookingGuests.clear();
                    UI.Booking.NewBookingGuestCount = 0;
                }

                ImGui::End();
                break;
            }
            case BookingInfoSection::CheckInBooking:
            {
                UI.Booking.Edit.CheckIn = GetCurrentTime();

                UpdateBookingInfo(App, UI.Booking.Edit.Id, UI.Booking.Edit);

                UI.Booking.UpdateList = true;
                UI.Booking.Edit = BookingInfo();
                UI.Booking.Section = BookingInfoSection::None;
                break;
            }
            case BookingInfoSection::CheckOutBooking:
            {
                UI.Booking.Edit.CheckOut = GetCurrentTime();

                if (UI.Booking.Edit.CheckIn.tm_year == 0)
                {
                    UI.ErrorMessage = "Checkin is required";
                    return;
                }

                UpdateBookingInfo(App, UI.Booking.Edit.Id, UI.Booking.Edit);

                RoomQuery RoomQuery;
                RoomQuery.UniqueID = UI.Booking.Edit.RoomID;
                auto Rooms = GetRooms(App, RoomQuery);
                Rooms[0].State = RoomStates::Available;
                UpdateRoomInfo(App, Rooms[0].Id, Rooms[0]);
                UI.Room.UpdateList = true;

                UI.Booking.UpdateList = true;
                UI.Booking.Edit = BookingInfo();
                UI.Booking.Section = BookingInfoSection::None;
                break;
            }
            case BookingInfoSection::ViewBookingAdditionalCost:
            {
                if (UIBookingAdditionalCostSection(UI, App, UI.Booking.Edit) == false)
                    UI.Booking.Section = BookingInfoSection::None;
            }
            case BookingInfoSection::EditBooking:
            {
                break;
            }
            }
            UI.Booking.LastSection = UI.Booking.Section;
        }

        bool UIBookingAdditionalCostSection(UIApp &UI, App &App, BookingInfo &Info)
        {
            if (UI.Booking.AdditionalCost.LastId != Info.Id)
            {
                UI.Booking.AdditionalCost.ShowWindow = true;
                UI.Booking.AdditionalCost.UpdateList = true;
            }

            if (!UI.Booking.AdditionalCost.ShowWindow)
            {
                UI.Booking.AdditionalCost.LastId = UINT32_MAX;
                return false;
            }

            if (UI.Booking.AdditionalCost.LastId != Info.Id)
                UI.Booking.AdditionalCost.UpdateList = true;

            ImGui::Begin(
                "Booking Additional Cost",
                &UI.Booking.AdditionalCost.ShowWindow);

            ImGui::SeparatorText("Additional Costs");

            if (UI.Booking.AdditionalCost.UpdateList)
            {
                BookingAdditionalCostQuery Query;
                Query.BookingID = Info.UniqueID;

                UI.Booking.AdditionalCost.Show =
                    GetBookingAdditionalCosts(App, Query);

                UI.Booking.AdditionalCost.ExtraItemInfo.clear();
                for (auto &CostInfo : UI.Booking.AdditionalCost.Show)
                {
                    ItemQuery ItemQuery;
                    ItemQuery.UniqueID = CostInfo.ItemID;
                    auto Item = GetItems(App, ItemQuery);

                    UI.Booking.AdditionalCost.ExtraItemInfo.push_back(Item[0]);
                }

                UI.Booking.AdditionalCost.UpdateList = false;
            }

            if (UI.Booking.LastSection !=
                BookingInfoSection::ViewBookingAdditionalCost)
            {
                UI.Booking.AdditionalCost.Section =
                    BookingAdditionalInfoSection::None;
            }

            if (ImGui::Button("Add Additional Cost"))
            {
                UI.Booking.AdditionalCost.Section =
                    BookingAdditionalInfoSection::AddAdditionalCost;
            }

            ImGui::Spacing();

            // Table goes here

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("Table", 4, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Item");
                ImGui::TableSetupColumn("UnitCount");
                ImGui::TableSetupColumn("Cost");
                ImGui::TableSetupColumn("Actions");

                ImGui::TableHeadersRow();

                uint32_t Idx = 0;
                for (const auto &CostInfo : UI.Booking.AdditionalCost.Show)
                {
                    ImGui::TableNextRow();
                    auto CostItem = UI.Booking.AdditionalCost.ExtraItemInfo[Idx];

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(CostItem.Name.data());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.2f", CostInfo.UnitCount);

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%.2f", CostInfo.UnitCount * CostInfo.PricePerUnit);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::PushID(CostInfo.Id);

                    if (ImGui::Button("Edit"))
                    {
                        UI.Booking.AdditionalCost.Section = BookingAdditionalInfoSection::EditAdditionalCost;
                        UI.Booking.AdditionalCost.Edit = CostInfo;
                    }

                    ImGui::PopID();
                    Idx++;
                }

                ImGui::EndTable();
            }
            // Register/Edit windows go here
            switch (UI.Booking.AdditionalCost.Section)
            {
            case BookingAdditionalInfoSection::AddAdditionalCost:
            {
                ImGui::Begin("Add Additional Cost");

                ImGui::End();
                break;
            }
            case BookingAdditionalInfoSection::EditAdditionalCost:
            {
                break;
            }
            case BookingAdditionalInfoSection::None:
            {
                break;
            }
            }

            ImGui::End();

            UI.Booking.AdditionalCost.LastId = Info.Id;

            return true;
        }

        void UInvoiceection(UIApp &UI, App &App)
        {
            ImGui::SeparatorText("Invoices");

            if (UI.Invoice.UpdateList == true)
            {
                InvoiceQuery Query;

                UI.Invoice.Show = GetInvoices(App, Query);
                UI.Invoice.UpdateList = false;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("InvoiceTable", 10, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Id");
                ImGui::TableSetupColumn("Sender");
                ImGui::TableSetupColumn("Reciever");
                ImGui::TableSetupColumn("State");
                ImGui::TableSetupColumn("VatPercent");
                ImGui::TableSetupColumn("LineTotal");
                ImGui::TableSetupColumn("CreatedAT");
                ImGui::TableSetupColumn("LastPaymentDate");
                ImGui::TableSetupColumn("ClosedAt");
                ImGui::TableSetupColumn("Actions");

                ImGui::TableHeadersRow();

                for (const auto &Invoice : UI.Invoice.Show)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);

                    switch (Invoice.SenderType)
                    {
                    case FinancialPaymentReceipientType::Guest:
                    {
                        GuestQuery GuestQuery;
                        GuestQuery.UniqueID = Invoice.SenderId;

                        if (ImGuiLink("Guest"))
                        {
                        }

                        break;
                    }
                    };

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                }

                ImGui::EndTable();
            }
        }

        void UItemSection(UIApp &UI, App &App)
        {
            ImGui::SeparatorText("Items");

            if (UI.Item.UpdateList == true)
            {
            }

            if (UI.LastSection != DashboardSection::Items)
            {
                UI.Item.Section = ItemSections::None;
                UI.Item.LastSection = ItemSections::None;
            }

            if (UI.Item.Section != UI.Item.LastSection)
            {
                ItemCategoryQuery CategoryQuery;
                UI.Item.Category.Show =
                    GetItemCategories(App, CategoryQuery);

                UI.Item.LastSection = UI.Item.Section;
            }

            if (ImGui::Button("Add New Item"))
            {
                // TODO:
                UI.Item.Section = ItemSections::RegisterItem;
            }

            ImGui::SameLine();
            if (ImGui::Button("View Category"))
            {
                // TODO:
                UI.Item.Section = ItemSections::ViewCategory;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("ItemsTable", 8, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Category");
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("LowStockThresold");
                ImGui::TableSetupColumn("CostPrice");
                ImGui::TableSetupColumn("DiscountPercent");
                ImGui::TableSetupColumn("SalesPrice");
                ImGui::TableSetupColumn("Description");
                ImGui::TableSetupColumn("Actions");

                ImGui::TableHeadersRow();

                uint32_t Idx = 0;
                for (const auto &Item : UI.Item.Show)
                {
                    ImGui::TableNextRow();
                    auto Category = Item.Category;

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(Category.Name.data());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.2f", Item.Item.Name.data());

                    Idx++;
                }

                ImGui::EndTable();
            }

            switch (UI.Item.Section)
            {
            case ItemSections::ViewCategory:
            {
                if (UItemCategorySection(UI, App) == false)
                    UI.Item.Section = ItemSections::None;
                if (UI.Item.LastSection != ItemSections::ViewCategory)
                    UI.Item.Category.ShowWindow = true;
                break;
            }
            case ItemSections::RegisterItem:
            {
                ImGui::Begin("Register Item");

                // ---- Name ----
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::InputText("##Name", UI.Item.New.Name.data(), UI.Item.New.Name.size());
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("LowStockThresold");
                ImGui::NextColumn();
                ImGui::InputInt("##LowStockThresold", (int *)&UI.Item.New.LowStockThresold);
                ImGui::NextColumn();

                // ---- Phone ----
                ImGui::Text("CostPrice");
                ImGui::NextColumn();
                ImGui::InputDouble("##CostPrice", (double *)&UI.Item.New.CostPrice);
                ImGui::NextColumn();
                // ---- Phone ----
                ImGui::Text("SalesPrice");
                ImGui::NextColumn();
                ImGui::InputDouble("##SalesPrice", (double *)&UI.Item.New.SalesPrice);
                ImGui::NextColumn();

                ImGui::Text("DiscountPercent");
                ImGui::NextColumn();
                ImGui::InputDouble("##DiscountPercent", (double *)&UI.Item.New.DiscountPercent);
                ImGui::NextColumn();

                // ---- Name ----
                ImGui::Text("Description");
                ImGui::NextColumn();
                ImGui::InputText("##Description", UI.Item.New.Description.data(), UI.Item.New.Description.size());
                ImGui::NextColumn();

                static int selectedCountry = 0;

                bool Combo = false;
                if (UI.Item.Category.Show.size() == 0)
                {
                    Combo = ImGui::BeginCombo("##CategorySelect", "NoCategory");
                }
                else
                {
                    Combo = ImGui::BeginCombo("##CategorySelect", UI.Item.Category.Show[selectedCountry].Name.data());
                    UI.Item.New.CategoryId = UI.Item.Category.Show[selectedCountry].UniqueID;
                }

                // Combo box
                if (Combo)
                {
                    for (int i = 0; i < UI.Item.Category.Show.size(); i++)
                    {
                        bool isSelected = (selectedCountry == i);

                        if (ImGui::Selectable(UI.Item.Category.Show[i].Name.data(), isSelected))
                        {
                            selectedCountry = i;
                            UI.Item.New.CategoryId = UI.Item.Category.Show[selectedCountry].UniqueID;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::End();

                break;
            }
            default:
                break;
            }
        }

        bool UItemCategorySection(UIApp &UI, App &App)
        {
            if (!UI.Item.Category.ShowWindow)
                return false;

            ImGui::Begin(
                "Item Categories",
                &UI.Item.Category.ShowWindow);

            if (UI.Item.Category.UpdateList)
            {
                ItemCategoryQuery Query;

                UI.Item.Category.Show =
                    GetItemCategories(App, Query);

                UI.Item.Category.UpdateList = false;
            }

            if (ImGui::Button("Add Category"))
            {
                UI.Item.Category.Section =
                    ItemCategoriesSections::RegisterCategory;
            }

            ImGui::Spacing();

            constexpr ImGuiTableFlags Flags =
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp |
                ImGuiTableFlags_ScrollY;

            if (ImGui::BeginTable("Categories", 1, Flags, ImVec2(0.0f, 450.0f)))
            {
                ImGui::TableSetupColumn("Name");

                ImGui::TableHeadersRow();

                uint32_t Idx = 0;
                for (const auto &Category : UI.Item.Category.Show)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(Category.Name.data());

                    Idx++;
                }

                ImGui::EndTable();
            }

            switch (UI.Item.Category.Section)
            {
            case ItemCategoriesSections::RegisterCategory:
            {
                ImGui::Begin("Add Category");

                // ---- Name ----
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::InputText("##Name", UI.Item.Category.New.Name.data(), UI.Item.Category.New.Name.size());
                ImGui::NextColumn();

                if (ImGui::Button("Register"))
                {
                    Insert(App, UI.Item.Category.New);

                    UI.Item.Category.New = ItemCategory();
                    UI.Item.Category.UpdateList = true;
                    UI.Item.Category.Section = ItemCategoriesSections::None;
                }

                ImGui::End();
                break;
            }
            case ItemCategoriesSections::None:
            {
                break;
            }
            }

            ImGui::End();

            return true;
        }

    } // namespace ClientCpp

} // namespace NepBil
