if (ENABLE_CXX)
  foreach(NAME target_nowait_task taskloop_offload_nowait taskloop)
    set(FULLNAME ${NAME})
    add_executable(cxx.${FULLNAME}.x ${FULLNAME}.cpp)
    set_target_properties(cxx.${FULLNAME}.x PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY ${TESTS_BINDIR})
    add_test(NAME cxx.${FULLNAME}
             COMMAND cxx.${FULLNAME}.x
             WORKING_DIRECTORY ${TESTS_BINDIR})
    set_tests_properties(cxx.${FULLNAME} PROPERTIES
                         LABELS cxx)
  endforeach()
endif()
