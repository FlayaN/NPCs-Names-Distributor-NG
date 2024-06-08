cmake_minimum_required(VERSION 3.20)

project(
  NPCsNamesDistributor
  VERSION 2.5.1
  LANGUAGES CXX)

# ##############################################################################
# # Source build guard
# ##############################################################################

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ##############################################################################
# # Add CommonLibNG plugin
# ##############################################################################

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CLIBNGPlugin)
add_simple_commonlibsse_ng_plugin(AUTHOR "sasnikol" LICENSE "MIT License")

# ##############################################################################
# # Mod specific
# ##############################################################################

find_package(nlohmann_json CONFIG REQUIRED)
set(CLIB_UTIL_INCLUDE_DIRS "extern/CLibUtil/include")
set(CRC32_DIR "extern/crc32")

target_include_directories(${PROJECT_NAME} PRIVATE ${CLIB_UTIL_INCLUDE_DIRS}
                                                   ${CRC32_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_sources(${PROJECT_NAME} PRIVATE "${CRC32_DIR}/Crc32.cpp")
