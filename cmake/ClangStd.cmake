include(FetchContent)
FetchContent_Declare(
        std_module
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external-dependencies
        URL "file://${LIBCXX_DIR}/share/libc++/v1"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SYSTEM
)

if (NOT std_module_POPULATED)
    FetchContent_Populate(std_module)
endif()

add_library(std)

target_sources(std
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${std_module_SOURCE_DIR}/std.cppm
        ${std_module_SOURCE_DIR}/std.compat.cppm
)

target_include_directories(std SYSTEM PUBLIC ${LIBCXX_DIR}/include/c++/v1)

target_compile_options(std
        PRIVATE
        -Wno-reserved-module-identifier
        -Wno-reserved-user-defined-literal
)

target_compile_options(std
        PUBLIC
        -nostdinc++
        -I ${LIBCXX_DIR}/include/c++/v1
)

message(STATUS ${LIBCXX_DIR}/include/c++/v1)

target_link_options(std
        INTERFACE
        -nostdlib++
        -stdlib=libc++
        -I ${LIBCXX_DIR}/include/c++/v1
        -L ${LIBCXX_DIR}/lib
        -Wl,-rpath,${LIBCXX_DIR}/lib
        -L${LIBCXX_DIR}/lib
        -Wl,-rpath,${LIBCXX_DIR}/lib
)

target_link_libraries(std
        INTERFACE
        c++
)

link_libraries(std)