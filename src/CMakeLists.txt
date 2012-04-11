rock_library(ptu_flir
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(ptu_flir_bin Main.cpp
    DEPS ptu_flir)

