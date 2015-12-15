file(GLOB_RECURSE src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM src  "src/caffe_stats.cpp")
file(GLOB hdr ${PROJECT_SOURCE_DIR}/*.h)

add_library(${CPM_LIB_TARGET_NAME} SHARED
        ${src} ${hdr} ${UI_RESOURCES} ${UI_HEADERS})
target_link_libraries(${CPM_LIB_TARGET_NAME} ${LIBS})
add_dependencies(${CPM_LIB_TARGET_NAME} caffe)

add_executable(caffe_stats EXCLUDE_FROM_ALL "src/caffe_stats.cpp" ${hdr})
target_link_libraries(caffe_stats ${CPM_LIB_TARGET_NAME} ${LIBS})
