add_library(Shared Tables.cpp UUID.cpp NepaliDateConverter.cpp)

target_compile_features(Shared PRIVATE cxx_std_20)
