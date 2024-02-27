include(FetchContent)
FetchContent_Declare(
        std_module
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

message(STATUS ${LIBCXX_DIR}/include/c++/v1)

target_link_options(std
        INTERFACE
        -stdlib=libc++
        -L${LIBCXX_DIR}/lib
        -Wl,-rpath,${LIBCXX_DIR}/lib
)

target_link_libraries(std
        INTERFACE
        c++
)

link_libraries(std)