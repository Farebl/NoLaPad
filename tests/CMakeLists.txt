# tests/CMakeLists.txt

find_package(Qt6 REQUIRED COMPONENTS Test Widgets Multimedia)

add_subdirectory(unit)
add_subdirectory(integration)
