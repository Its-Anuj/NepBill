add_executable(NepBill Main.cpp Src/UUID.cpp Src/NepalDateConverter.cpp Src/Tables.cpp Src/NepBill.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-error=c++11-narrowing)
endif()
# 1. Find OpenSSL
find_package(OpenSSL REQUIRED)

target_include_directories(
    NepBill PRIVATE
    Libs/asio/include
    Libs/Crow/include
    Src/
    Libs/Libpqxx/include
    Libs/JwtCpp/include
    Libs/TlExpected/include
)

set(ASIO_INCLUDE_DIR
    "Libs/Asio/include")

set(PostgreSQL_LIBRARY "D:/Downloads/PostgressSQL/lib/libpq.lib")
set(PostgreSQL_INCLUDE_DIR "D:/Downloads/PostgressSQL/include")

add_subdirectory("Libs/Crow/")
add_subdirectory("Libs/Libpqxx/")

target_compile_features(NepBill PRIVATE cxx_std_20)

target_link_libraries(NepBill kernel32 Crow ws2_32 iphlpapi mswsock pqxx OpenSSL::Crypto OpenSSL::SSL)

target_precompile_headers(NepBill PRIVATE
    Src/PCH.h
)

message(STATUS "Templates dir = ${CMAKE_SOURCE_DIR}/NepBill/templates")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_options(NepBill PRIVATE "-static-openmp" "-static-libgcc")
    # Note: Full static linking on Windows can sometimes conflict with runtime UCRT dlls, 
    # so Environment Path updates (Solution 1 & 2) remain cleanest for local development workflows.
endif()
