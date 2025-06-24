
# Log a status message to confirm we're in the correct CMakeLists.txt
message(STATUS "src cmakelists")

# Collect all .cpp files from the main folder
file(GLOB_RECURSE srcs "*.cpp")

# Log all collected .cpp files from the main folder
message(STATUS "Collected source files: ${srcs}")

# Combine both sources from the main and the library
list(APPEND srcs ${WDFLASH_SOURCES})


# Register the component with ESP-IDF's build system
idf_component_register(SRCS "main.cpp" "debug.cpp" "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES esp_adc
                       #REQUIRES driver bt esp_timer nvs_flash
                       )

# Log the registration process
message(STATUS "idf_component_register completed with sources: ${srcs}")
