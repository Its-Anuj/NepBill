#include <iostream>
#include "Tables.h"
#include "UUID.h"
#include "Maths.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"
#include "Window.h"
#include "NepBill.h"
#include "Data.h"
#include "UI.h"

#define PRINT(x)        \
    {                   \
        std::cout << x; \
    }
#define PRINTLN(x)              \
    {                           \
        std::cout << x << "\n"; \
    }

int main(int argc, char const *argv[])
{
    const char *glsl_version = "#version 130";
    NepBill::Window Window;
    Window.Init("NepBill", {1280, 720});

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)Window.GetRawWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    NepBill::ClientCpp::App App;
    NepBill::ClientCpp::UIApp UIApp;
    NepBill::ClientCpp::InitDatabase(App, "NepBill.db3");

    auto Code = NepBill::ClientCpp::ShouldBusinessRowAsk(App);

    if (Code == NepBill::ClientCpp::DatabaseAskReturnCodes::TablesNotPresent)
    {
        NepBill::ClientCpp::SetupDatabase(App);
        Code = NepBill::ClientCpp::DatabaseAskReturnCodes::HotelRowNotPresent;
    }

    while (!Window.ShouldClose())
    {
        Window.Update();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (Code == NepBill::ClientCpp::DatabaseAskReturnCodes::HotelRowNotPresent)
        {
            NepBill::HotelInfo &Info = App.Info;

            ImGui::Begin("Hotel Information");

            ImGui::Columns(2, "HotelInfoColumns", false);

            // ---- Hotel Name ----
            ImGui::Text("Hotel Name");
            ImGui::NextColumn();
            ImGui::InputText("##HotelName",
                             Info.Name.data(),
                             Info.Name.size());
            ImGui::NextColumn();

            // ---- VAT Number ----
            ImGui::Text("VAT Number");
            ImGui::NextColumn();
            ImGui::InputText("##VatNumber",
                             Info.VatNumber.data(),
                             Info.VatNumber.size());
            ImGui::NextColumn();

            // ---- Address ----
            ImGui::Text("Address");
            ImGui::NextColumn();
            ImGui::InputText("##Address",
                             Info.Address.data(),
                             Info.Address.size());
            ImGui::NextColumn();

            // ---- PAN Number ----
            ImGui::Text("PAN Number");
            ImGui::NextColumn();
            ImGui::InputText("##PanNumber",
                             Info.PanNumber.data(),
                             Info.PanNumber.size());
            ImGui::NextColumn();

            // ---- Phone Number ----
            ImGui::Text("Phone Number");
            ImGui::NextColumn();
            ImGui::InputText("##PhoneNumber",
                             Info.PhoneNumber.data(),
                             Info.PhoneNumber.size());
            ImGui::NextColumn();

            // ---- Country ----
            ImGui::Text("Country");
            ImGui::NextColumn();

            static int selectedCountry = 0;

            // Combo box
            if (ImGui::BeginCombo("##Country", NepBill::Countries[selectedCountry]))
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

            strncpy(Info.Country.data(),
                    NepBill::Countries[selectedCountry],
                    Info.Country.size());

            ImGui::NextColumn();

            ImGui::Columns(1);

            ImGui::Spacing();

            if (ImGui::Button("Save", ImVec2(120.0f, 0.0f)))
            {
                NepBill::ClientCpp::Insert(App, Info);
                Code = NepBill::ClientCpp::DatabaseAskReturnCodes::NoAsk;
            }

            ImGui::End();
        }
        else
        {
            ImGui::Begin("Hotel");

            NepBill::ClientCpp::UIDashboard(UIApp, App);

            ImGui::End();
        }

        NepBill::Vec4 ClearColor{0.2f, 0.2f, 0.2f, 1.0f};
        // Rendering
        ImGui::Render();
        glViewport(0, 0, 1280, 720);
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Window.Destroy();
    PRINTLN("Destroyed Window");

    return 0;
}
