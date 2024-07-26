include(cmake/CPM.cmake)

if (ISL_INCLUDE_TESTS)
    CPMAddPackage(
            NAME Catch2
            GITHUB_REPOSITORY catchorg/Catch2
            GIT_TAG devel
    )
endif ()
