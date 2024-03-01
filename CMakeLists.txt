cmake_minimum_required(VERSION 3.28.0)

project(isl-project VERSION 0.0.2.1 LANGUAGES CXX)

set(CMAKE_SCAN_FOR_MODULES)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(CPack)

include(cmake/InterproceduralOptimization.cmake)
include(cmake/ProjectOptions.cmake)
include(cmake/CompilerOptions.cmake)
include(cmake/Libraries.cmake)

include(include/isl/CMakeLists.txt)

if (ISL_INCLUDE_TESTS)
    include(catch/CMakeLists.txt)
endif ()

install(DIRECTORY include/isl
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/islConfigVersion.cmake"
        VERSION "${version}"
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/islConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/isl
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/islConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/islConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/isl
)

