  enable_testing()

  file(
          GLOB_RECURSE
          ISL_CATCH_OBJECTS
          "catch/*.cpp"
  )

  add_executable(
          isl-catch
          ${ISL_CATCH_OBJECTS}
  )

  target_include_directories(isl-catch
          PUBLIC
          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )

  isl_set_project_warnings(
          isl-catch
          ${ISL_STRICT_COMPILATION}
          ""
          ""
          ""
          ""
          ${ISL_HARDENING}
          ${ISL_LIBCPP}
  )

  target_link_libraries(
          isl-catch
          fmt::fmt
          Catch2::Catch2
          Catch2::Catch2WithMain
  )

  if (PRECOMPILED_HEADER)
      target_precompile_headers(
              isl-catch
              REUSE_FROM
              isl-lexer
      )
  endif ()

  add_test(isl-catch-test isl-catch)