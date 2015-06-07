file(GLOB tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test*.sh)

find_program(BASH_PROGRAM bash)

foreach(test ${tests})
    get_filename_component(name ${test} NAME)
    set(DEST ${CMAKE_CURRENT_BINARY_DIR}/${name})
    file(COPY ${test} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    add_test(${name} ${BASH_PROGRAM} ${DEST})
endforeach()
