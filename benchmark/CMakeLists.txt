file(
        GLOB_RECURSE
        ISL_BENCH_OBJECTS
        "benchmark/*.cpp"
)

add_executable(
        isl-bench
        ${ISL_BENCH_OBJECTS}
)

isl_set_target_flags(
        isl-bench
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
        isl-bench
        PRIVATE
        benchmark
        isl
        fmt::fmt
)

target_include_directories(isl-bench
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_test(isl-catch-test isl-bench)
