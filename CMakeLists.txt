cmake_minimum_required(VERSION 3.20)
project(HospitalEmergencyResourceManagement LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
add_library(hospital_model src/Hospital.cpp)
target_include_directories(hospital_model PUBLIC include)
target_compile_features(hospital_model PUBLIC cxx_std_17)

add_executable(hospital_demo src/main.cpp src/ConsoleApp.cpp)
target_link_libraries(hospital_demo PRIVATE hospital_model)

enable_testing()
add_executable(hospital_tests tests/hospital_tests.cpp)
target_link_libraries(hospital_tests PRIVATE hospital_model)
add_test(NAME hospital_model_tests COMMAND hospital_tests)
