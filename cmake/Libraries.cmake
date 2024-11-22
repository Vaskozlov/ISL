include(cmake/CPM.cmake)

find_package(Threads REQUIRED)

CPMFindPackage(
        NAME fmt
        GIT_TAG master
        GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage(
        NAME unordered_dense
        GITHUB_REPOSITORY martinus/unordered_dense
        GIT_TAG main
)

if (ISL_INCLUDE_TESTS)
    CPMFindPackage(
            NAME Catch2
            GITHUB_REPOSITORY catchorg/Catch2
            GIT_TAG devel
    )
endif ()

if (ISL_INCLUDE_BENCHMARK)
    CPMAddPackage(
            NAME benchmark
            GITHUB_REPOSITORY google/benchmark
            VERSION 1.5.0
            OPTIONS
            "BENCHMARK_ENABLE_TESTING Off"
            "BENCHMARK_USE_LIBCXX ${ISL_LIBCPP}"
            "BENCHMARK_DOWNLOAD_DEPENDENCIES ON"
    )

    if (benchmark_ADDED)
        set_target_properties(benchmark PROPERTIES CXX_STANDARD 23)
    endif ()
endif ()
