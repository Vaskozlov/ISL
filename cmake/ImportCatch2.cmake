FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG devel
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(Catch2)
