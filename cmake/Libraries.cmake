if (ISL_INCLUDE_TESTS)
    find_package(Catch2 CONFIG REQUIRED)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    include(cmake/ClangStd.cmake)
endif()
