include(cmake/CPM.cmake)

find_package(Threads REQUIRED)

CPMAddPackage(
        NAME fmt
        GIT_TAG master
        GITHUB_REPOSITORY fmtlib/fmt
)

if (ISL_INCLUDE_TESTS)
    CPMAddPackage(
            NAME Catch2
            GITHUB_REPOSITORY catchorg/Catch2
            GIT_TAG devel
    )
endif ()
