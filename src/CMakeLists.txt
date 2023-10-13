file(
        GLOB_RECURSE
        ISL_OBJECTS
        "src/isl/*.cpp"
)

if (ISL_STATIC_LIBRARY)
    add_library(isl STATIC ${ISL_OBJECTS})
else ()
    add_library(isl SHARED ${ISL_OBJECTS})
endif ()

isl_set_project_warnings(
        isl
        ${ISL_STRICT_COMPILATION}
        ""
        ""
        ""
        ""
        ${ISL_HARDENING}
        ${ISL_LIBCPP}
)

if (ISL_LIBCPP AND LINUX)
    add_dependencies(ccl fmt::fmt-header-only)

    target_link_libraries(
            isl
            fmt::fmt-header-only
    )
else ()
    add_dependencies(isl fmt::fmt)

    target_link_libraries(
            isl
            fmt::fmt
    )
endif ()

if (ISL_PRECOMPILED_HEADERS)
    target_precompile_headers(
            isl
            PUBLIC
            "pch.hpp"
    )
endif ()

target_include_directories(isl
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
