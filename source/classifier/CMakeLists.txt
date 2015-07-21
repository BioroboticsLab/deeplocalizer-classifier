file(GLOB_RECURSE src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM src  "caffe_stats.cpp")
file(GLOB hdr ${PROJECT_SOURCE_DIR}/include/deeplocalizer/classifier/*.h)

include_directories(
    ${PROJECT_SOURCE_DIR}/include/deeplocalizer/classifier/
    ${PROJECT_SOURCE_DIR}/include/deeplocalizer/common/
)

add_library(deeplocalizer-classifier
    ${src} ${hdr} ${UI_RESOURCES} ${UI_HEADERS})



add_executable(caffe_stats "caffe_stats.cpp" ${hdr})
target_link_libraries(caffe_stats ${classifier-libs})
