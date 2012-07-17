include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

rock_executable(test_ptu test_ptu.cpp
    HEADERS ../src/Driver.h ../src/Cmd.h
    DEPS ptu_directedperception
    DEPS_CMAKE Boost
    NOINSTALL)

