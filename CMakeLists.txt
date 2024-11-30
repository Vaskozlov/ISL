cmake_minimum_required(VERSION 3.20.0)

project(isl-project VERSION 0.0.2.1 LANGUAGES CXX)

include(CPack)
include(GNUInstallDirs)
include(FetchContent)
include(CMakePackageConfigHelpers)

include(cmake/Libraries.cmake)
include(cmake/InterproceduralOptimization.cmake)
include(cmake/ProjectOptions.cmake)
include(cmake/CompilerOptions.cmake)

include(src/CMakeLists.txt)

if (ISL_INCLUDE_BENCHMARK)
    include(benchmark/CMakeLists.txt)
endif ()

if (ISL_INCLUDE_TESTS)
    include(catch/CMakeLists.txt)
endif ()

install(TARGETS isl unordered_dense
        EXPORT islTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/isl
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

install(EXPORT islTargets
        FILE islTargets.cmake
        NAMESPACE isl::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/isl
)

set_property(TARGET isl PROPERTY VERSION ${version})

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

install(FILES cmake/CompilerOptions.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/isl/cmake)

export(EXPORT islTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/islTargets.cmake"
)