cmake_minimum_required(VERSION 3.10)
project(compdf_c_demo C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# SDK root: <pkg>/, this CMakeLists lives at <pkg>/samples/
get_filename_component(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

set(SDK_INCLUDE_DIR "${SDK_ROOT}/include")
set(SDK_LIB_DIR     "${SDK_ROOT}/lib")

# Pick executable name based on host arch (matches RunDemo.sh)
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(DEMO_NAME "demo_linux64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "i686")
    set(DEMO_NAME "demo_linux86")
else()
    set(DEMO_NAME "demo_linux")
endif()

add_executable(${DEMO_NAME} demo/demo.c)

target_include_directories(${DEMO_NAME} PRIVATE "${SDK_INCLUDE_DIR}")
target_link_directories(${DEMO_NAME} PRIVATE "${SDK_LIB_DIR}")
target_link_libraries(${DEMO_NAME} PRIVATE libpdfsolidconversionsdk.so)

# Embed an rpath so the binary finds the SDK .so files without LD_LIBRARY_PATH.
set_target_properties(${DEMO_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    BUILD_RPATH   "${SDK_LIB_DIR}"
    INSTALL_RPATH "${SDK_LIB_DIR}"
)
# Belt-and-suspenders: also pass -rpath directly to the linker.
target_link_options(${DEMO_NAME} PRIVATE "-Wl,-rpath,${SDK_LIB_DIR}")
