function(add_memcheck_test name binary)
  set(memcheck_command "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS}")
  separate_arguments(memcheck_command)
  add_test(memcheck_${name} ${memcheck_command} ${binary} ${ARGN})
endfunction(add_memcheck_test)

function(set_memcheck_test_properties name)
  set_tests_properties(${name} ${ARGN})
  set_tests_properties(memcheck_${name} ${ARGN})
endfunction(set_memcheck_test_properties)

enable_testing()
include_directories(. ../source
    ${CATCH_INCLUDE_DIR}
)
add_subdirectory(testdata)

file(GLOB tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Test*.cpp)

foreach(test ${tests})
    get_filename_component(name ${test} NAME_WE)
    add_executable(${name} ${test})
    target_link_libraries(${name}
        ${OPENGL_LIBRARIES}
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        libdeeplocalizer)
    set(test_bin ${CMAKE_CURRENT_BINARY_DIR}/${name})
    add_test(${name} ${test_bin} --force-colour)
    set(test_bins ${test_bins} ${test_bin})
    add_memcheck_test(${name} ${test_bin})
endforeach()


add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${test_bins})
