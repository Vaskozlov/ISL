include(cmake/CPM.cmake)

find_package(Threads REQUIRED)

CPMFindPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 11.2.0
)

CPMAddPackage(
        NAME unordered_dense
        GITHUB_REPOSITORY martinus/unordered_dense
        GIT_TAG v4.5.0
)

if(ISL_INCLUDE_TESTS)
        CPMFindPackage(
                NAME Catch2
                GITHUB_REPOSITORY catchorg/Catch2
                GIT_TAG v3.8.1
        )
endif()

if(ISL_INCLUDE_BENCHMARK)
        CPMAddPackage(
                NAME benchmark
                GITHUB_REPOSITORY google/benchmark
                GIT_TAG v1.9.0
                OPTIONS
                "BENCHMARK_ENABLE_TESTING Off"
                "BENCHMARK_USE_LIBCXX ${ISL_LIBCPP}"
                "BENCHMARK_DOWNLOAD_DEPENDENCIES ON"
        )

        if(benchmark_ADDED)
                set_target_properties(benchmark PROPERTIES CXX_STANDARD 23)
        endif()
endif()
