project(NepBill VERSION 0.1.0 LANGUAGES C CXX)

add_executable(NepBill Main.cpp Src/Window.cpp Src/NepBill.cpp Src/UI.cpp)

include_directories("Src/")
include_directories("Libs/ImGui/")
include_directories("Libs/ImGui/backends/")
include_directories("Libs/Glfw/include/")
include_directories("Libs/Sqlite/include/")
include_directories("Shared/")

add_subdirectory("Libs/Sqlite")
add_subdirectory("Libs/Glfw")
add_subdirectory("Shared")

add_library(ImGui Libs/ImGui/imgui.cpp Libs/ImGui/imgui_demo.cpp Libs/ImGui/imgui_draw.cpp
Libs/ImGui/imgui_widgets.cpp Libs/ImGui/imgui_tables.cpp 
Libs/ImGui/backends/imgui_impl_opengl3.cpp Libs/ImGui/backends/imgui_impl_glfw.cpp)

target_compile_features(NepBill PRIVATE cxx_std_20)

target_link_libraries(NepBill ImGui glfw opengl32 kernel32 SQLiteCpp Shared)
