include(cmake/CPM.cmake)

find_package(Threads REQUIRED)

CPMFindPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 577fd3be883accf8629423ed77fcca8a24bccee2
)

CPMAddPackage(
        NAME unordered_dense
        GITHUB_REPOSITORY martinus/unordered_dense
        GIT_TAG 73f3cbb237e84d483afafc743f1f14ec53e12314
)

if (ISL_INCLUDE_TESTS)
    CPMFindPackage(
            NAME Catch2
            GITHUB_REPOSITORY catchorg/Catch2
            GIT_TAG 914aeecfe23b1e16af6ea675a4fb5dbd5a5b8d0a
    )
endif ()

if (ISL_INCLUDE_BENCHMARK)
    CPMAddPackage(
            NAME benchmark
            GITHUB_REPOSITORY google/benchmark
            GIT_TAG ff5c94d860dcffd4f4159edf79d805bcfaca3cb8
            OPTIONS
            "BENCHMARK_ENABLE_TESTING Off"
            "BENCHMARK_USE_LIBCXX ${ISL_LIBCPP}"
            "BENCHMARK_DOWNLOAD_DEPENDENCIES ON"
    )

    if (benchmark_ADDED)
        set_target_properties(benchmark PROPERTIES CXX_STANDARD 23)
    endif ()
endif ()
