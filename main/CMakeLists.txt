set(srcs)

idf_component_register(SRCS "ble-cts-central.cpp" "serial-module.cpp" "main.cpp" "${srcs}"
                       INCLUDE_DIRS ".")
