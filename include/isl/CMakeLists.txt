add_library(isl)

file(
        GLOB_RECURSE
        ISL_OBJECTS
        "include/isl/*.ixx"
        "include/isl/*.cxxm"
)

target_include_directories(isl
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(
        isl
        PUBLIC
        FILE_SET CXX_MODULES FILES
        ${ISL_OBJECTS}
)

isl_set_project_warnings(
        isl
        ${ISL_STRICT_COMPILATION}
        ""
        ""
        ""
        ""
        ${ISL_HARDENING}
        ${ISL_LIBCPP}
        ${ISL_UNITY_BUILD}
)

target_link_libraries(
        isl
        fmt::fmt
)
