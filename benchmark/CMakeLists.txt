file(
        GLOB_RECURSE
        ISL_BENCH_OBJECTS
        "benchmark/*.cpp"
)

add_executable(
        isl-bench
        ${ISL_BENCH_OBJECTS}
)

isl_set_project_warnings(
        isl-bench
        ${ISL_STRICT_COMPILATION}
        ""
        ""
        ""
        ""
        ${ISL_HARDENING}
        ${ISL_LIBCPP}
        OFF
)

add_dependencies(
        isl-bench
        isl
        benchmark
        fmt::fmt
)

target_link_libraries(
        isl-bench
        benchmark
        isl
        fmt::fmt-header-only
)

target_include_directories(isl-bench
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)