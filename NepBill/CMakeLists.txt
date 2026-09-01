cmake_minimum_required(VERSION 3.16)
project(NepBill)

add_executable(NepBill
    Main.cpp
    Src/UUID.cpp
    Src/EncryptionManager.cpp
    Src/NepalDateConverter.cpp
    Src/Routers.cpp
    Src/Tables.cpp
    Src/NepBill.cpp
)

# 1. Find OpenSSL
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

# Option B: build from source as subdirectory (if you keep Libs/Libpqxx/)
add_subdirectory("Libs/Libpqxx/")

set(ASIO_INCLUDE_DIR
    "Libs/Asio/include")

# Disable tests before making available
set(SODIUM_DISABLE_TESTS ON)
target_include_directories(
    NepBill PRIVATE
    Libs/Asio/include
    Libs/Crow/include
    Libs/LibSodium/libsodium/src/libsodium/include/sodium
    Src/
    Libs/JwtCpp/include
    Libs/TlExpected/include
)

add_subdirectory("Libs/Crow/")
add_subdirectory("Libs/LibSodium/")

target_compile_features(NepBill PRIVATE cxx_std_20)

# Linux: no kernel32/ws2_32/iphlpapi/mswsock needed
target_link_libraries(NepBill
    Crow
    pqxx
    PostgreSQL::PostgreSQL
    OpenSSL::Crypto
    OpenSSL::SSL
    sodium
    pthread
)

target_precompile_headers(NepBill PRIVATE Src/PCH.h)   