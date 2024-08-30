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

isl_set_target_flags(
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

if (ISL_PRECOMPILED_HEADERS)
    target_precompile_headers(
            isl
            PUBLIC
            "pch.hpp"
    )
endif ()

target_link_libraries(
        isl
        PUBLIC
        Threads::Threads
        fmt::fmt-header-only
        unordered_dense::unordered_dense
)

target_include_directories(isl
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
