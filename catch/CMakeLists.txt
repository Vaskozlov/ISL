enable_testing()

file(
        GLOB_RECURSE
        ISL_CATCH_OBJECTS
        "catch/*.cpp"
)

add_executable(
        isl-catch
        ${ISL_CATCH_OBJECTS}
)

target_include_directories(isl-catch
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

isl_set_target_flags(
        isl-catch
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
        isl-catch
        PRIVATE
        isl
        fmt::fmt-header-only
        Catch2::Catch2WithMain
)

if (ISL_PRECOMPILED_HEADER)
    target_precompile_headers(
            isl-catch
            REUSE_FROM
            isl
    )
endif ()

add_test(isl-catch-test isl-catch)
