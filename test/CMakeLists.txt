
enable_testing(true)
include_directories(. ../source ${CATCH_INCLUDE_DIR})
add_subdirectory(testdata)
add_subdirectory(scripts)

file(GLOB tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Test*.cpp)

foreach(test ${tests})
    get_filename_component(name ${test} NAME_WE)
    add_executable(${name} ${test})
    target_link_libraries(${name} ${libs})
    set(test_bin ${CMAKE_CURRENT_BINARY_DIR}/${name})
    add_test(${name} ${test_bin})
    set(test_bins ${test_bins} ${test_bin})
endforeach()
