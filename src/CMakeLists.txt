include_directories(/usr/local/include)
link_directories (/usr/local/lib)

rock_library(ptu_flir
    SOURCES Driver.cpp Cmd.cpp
    HEADERS Driver.h Cmd.h
    DEPS_PKGCONFIG base-types iodrivers_base)

set(module_name ${PROJECT_NAME})
#target_link_libraries(${PROJECT_NAME} ueye_api)

rock_executable(test_bin test.cpp
    DEPS ptu_flir)

