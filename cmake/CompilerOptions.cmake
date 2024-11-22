include(CheckCXXCompilerFlag)

macro(
        isl_enable_hardening
        target
)
    message(STATUS "** Enabling Hardening (Target ${target}) **")

    if (MSVC)
        set(NEW_COMPILE_OPTIONS "${NEW_COMPILE_OPTIONS} /Zm 1000 /sdl /DYNAMICBASE /guard:cf ")
        message(STATUS "*** MSVC flags: /sdl /DYNAMICBASE /guard:cf /NXCOMPAT /CETCOMPAT")
        set(NEW_LINK_OPTIONS "${NEW_LINK_OPTIONS} /NXCOMPAT /CETCOMPAT")

    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
        set(NEW_CXX_DEFINITIONS ${NEW_CXX_DEFINITIONS} -D_GLIBCXX_ASSERTIONS)
        message(STATUS "*** GLIBC++ Assertions (vector[], string[], ...) enabled")

        if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
            set(NEW_COMPILE_OPTIONS ${NEW_COMPILE_OPTIONS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3)
            message(STATUS "*** g++/clang _FORTIFY_SOURCE=3 enabled")
        endif ()

        check_cxx_compiler_flag(-fstack-protector-strong STACK_PROTECTOR)
        if (STACK_PROTECTOR)
            set(NEW_COMPILE_OPTIONS ${NEW_COMPILE_OPTIONS} -fstack-protector-strong)
            message(STATUS "*** g++/clang -fstack-protector-strong enabled")
        else ()
            message(STATUS "*** g++/clang -fstack-protector-strong NOT enabled (not supported)")
        endif ()

        check_cxx_compiler_flag(-fcf-protection CF_PROTECTION)
        if (CF_PROTECTION)
            set(NEW_COMPILE_OPTIONS ${NEW_COMPILE_OPTIONS} -fcf-protection)
            message(STATUS "*** g++/clang -fcf-protection enabled")
        else ()
            message(STATUS "*** g++/clang -fcf-protection NOT enabled (not supported)")
        endif ()

        check_cxx_compiler_flag(-fstack-clash-protection CLASH_PROTECTION)
        if (CLASH_PROTECTION)
            if (LINUX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                set(NEW_COMPILE_OPTIONS ${NEW_COMPILE_OPTIONS} -fstack-clash-protection)
                message(STATUS "*** g++/clang -fstack-clash-protection enabled")
            else ()
                message(STATUS "*** g++/clang -fstack-clash-protection NOT enabled (clang on non-Linux)")
            endif ()
        else ()
            message(STATUS "*** g++/clang -fstack-clash-protection NOT enabled (not supported)")
        endif ()
    endif ()

    message(STATUS "** Hardening Compiler Flags: ${NEW_COMPILE_OPTIONS}")
    message(STATUS "** Hardening Linker Flags: ${NEW_LINK_OPTIONS}")
    message(STATUS "** Hardening Compiler Defines: ${NEW_CXX_DEFINITIONS}")

    target_compile_options(${target} INTERFACE ${NEW_COMPILE_OPTIONS})
    target_link_options(${target} INTERFACE ${NEW_LINK_OPTIONS})
    target_compile_definitions(${target} INTERFACE ${NEW_CXX_DEFINITIONS})
endmacro()


function(
        isl_set_target_flags
        project_name
        WARNINGS_AS_ERRORS
        MSVC_WARNINGS
        CLANG_WARNINGS
        GCC_WARNINGS
        CUDA_WARNINGS
        ENABLE_HARDENING
        ENABLE_LIBCPP
        UNITY_BUILD
)
    if ("${MSVC_WARNINGS}" STREQUAL "")
        set(MSVC_WARNINGS
                /W4 # Baseline reasonable warnings
                /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss of data
                /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
                /w14263 # 'function': member function does not override any base class virtual member function
                /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not
#be destructed correctly
                /w14287 # 'operator': unsigned/negative constant mismatch
                /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside
#the for - loop scope
                /w14296 # 'operator': expression is always 'boolean_value'
                /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
                /w14545 # expression before comma evaluates to a function which is missing an argument list
                /w14546 # function call before comma missing argument list
                /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
                /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
                /w14555 # expression has no effect; expected expression with side- effect
                /w14619 # pragma warning: there is no warning number 'number'
                /w14640 # Enable warning on thread un-safe static member initialization
                /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
                /w14905 # wide string literal cast to 'LPSTR'
                /w14906 # string literal cast to 'LPWSTR'
                /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
                /permissive- # standards conformance mode for MSVC compiler.
        )
    endif ()

    if ("${CLANG_WARNINGS}" STREQUAL "")
        set(CLANG_WARNINGS
                -Wall
                -Wextra
                -Wshadow
                -Wnon-virtual-dtor
                -Wold-style-cast
                -Wcast-align
                -Wunused
                -Woverloaded-virtual
                -pedantic
                -Wpedantic
                -pedantic-errors
                -Wconversion
                -Wsign-conversion
                -Wnull-dereference
                -Wdouble-promotion
                -Wfloat-conversion
                -Wformat=2
                -Wuninitialized
                -Wnull-dereference
                -Wfloat-equal
                -Wimplicit-fallthrough
        )

        if (ISL_ADDRESS_SANITIZER)
            target_link_options(${project_name} INTERFACE -fsanitize=address,undefined,leak)
            list(APPEND CLANG_WARNINGS -fsanitize=address,undefined,leak)
        endif()

        if (ISL_THREAD_SANITIZER)
            target_link_options(${project_name} INTERFACE -fsanitize=thread)
            list(APPEND CLANG_WARNINGS -fsanitize=thread)
        endif()
    endif ()

    if ("${GCC_WARNINGS}" STREQUAL "")
        set(GCC_WARNINGS
                ${CLANG_WARNINGS}
                -Wmisleading-indentation
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
        )
    endif ()

    if ("${CUDA_WARNINGS}" STREQUAL "")
        set(CUDA_WARNINGS
                -Wall
                -Wextra
                -Wunused
                -Wconversion
                -Wshadow
        )
    endif ()

    if (WARNINGS_AS_ERRORS)
        message(TRACE "Warnings are treated as errors")
        list(APPEND CLANG_WARNINGS -Werror)
        list(APPEND GCC_WARNINGS -Werror)
        list(APPEND MSVC_WARNINGS /WX)
    endif ()

    if (ENABLE_HARDENING)
        isl_enable_hardening(${project_name})
    endif ()

    if (MSVC)
        list(APPEND MSVC_WARNINGS /utf-8)
        set(PROJECT_WARNINGS_CXX ${MSVC_WARNINGS})
    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_WARNINGS_CXX ${CLANG_WARNINGS})
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_WARNINGS_CXX ${GCC_WARNINGS})
    else ()
        message(AUTHOR_WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
    endif ()

#use the same warning flags for C
    set(PROJECT_WARNINGS_C "${PROJECT_WARNINGS_CXX}")

    set(PROJECT_WARNINGS_CUDA "${CUDA_WARNINGS}")

    target_compile_options(
            ${project_name}
            INTERFACE # C++ warnings
            $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_WARNINGS_CXX}>
#C warnings
            $<$<COMPILE_LANGUAGE:C>:${PROJECT_WARNINGS_C}>
#Cuda warnings
            $<$<COMPILE_LANGUAGE:CUDA>:${PROJECT_WARNINGS_CUDA}>)

    add_compile_options(${PROJECT_WARNINGS_CXX})

    if (UNITY_BUILD)
        set_target_properties(${project_name} PROPERTIES UNITY_BUILD ON)
    endif ()
endfunction()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(ISL_DEBUG=1)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${ISL_ENABLE_IPO})
    isl_enable_ipo()
endif ()
