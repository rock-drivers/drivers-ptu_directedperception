rock_library(ptu_directedperception
    SOURCES Driver.cpp Cmd.cpp
    HEADERS Driver.h Cmd.h
    DEPS_PKGCONFIG base-types iodrivers_base)

set(module_name ${PROJECT_NAME})
#target_link_libraries(${PROJECT_NAME} ueye_api)

SET(BOOST_COMPONENTS program_options)
FIND_PACKAGE(Boost 1.46.0 COMPONENTS ${BOOST_COMPONENTS})
IF(Boost_FOUND)
        SET(Boost_USE_MULTITHREADED ON)
ELSE(Boost_FOUND)
            MESSAGE(FATAL_ERROR "Boost is required with compontents ${BOOST_COMPONENTS}")
ENDIF()

TARGET_LINK_LIBRARIES(${LIBS_LDFLAGS} ${Boost_LIBRARIES})
