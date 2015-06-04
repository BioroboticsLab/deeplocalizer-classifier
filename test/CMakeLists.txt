
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
        Qt5::Widgets
        Qt5::OpenGL
        ${OPENGL_LIBRARIES}
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        deeplocalizer)
    set(test_bin ${CMAKE_CURRENT_BINARY_DIR}/${name})
    add_test(${name} ${test_bin} --force-colour)
    set(test_bins ${test_bins} ${test_bin})
endforeach()


add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${test_bins})
