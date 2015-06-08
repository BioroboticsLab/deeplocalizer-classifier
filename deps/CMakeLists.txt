file(GLOB_RECURSE src ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR} pipeline/*.cpp)
file(GLOB_RECURSE hdr ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR} pipeline/*.h *.hpp)

set(PIPELINE_SRC ${src} CACHE INTERNAL "pipeline sources")
set(PIPELINE_HDR ${hdr} CACHE INTERNAL "pipeline headers")

add_subdirectory(catch)
