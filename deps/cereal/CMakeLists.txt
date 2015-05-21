message("Build system will fetch and build the Cereal serialization library")
message("==================================================================")
include(ExternalProject)
ExternalProject_Add(libcereal
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external
    URL http://www.cs.cmu.edu/~robp/files/cereal-v1.0.0.tgz
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/cereal-1.0.0
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/install
    UPDATE_COMMAND sh -c "mkdir -p <SOURCE_DIR>/build"
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/cereal-1.0.0/build
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND sh -c "mkdir -p <INSTALL_DIR>/include && cp -r <SOURCE_DIR>/include/cereal <INSTALL_DIR>/include"
)
ExternalProject_Get_Property(libcereal source_dir)
set(CEREAL_INCLUDE_DIR ${source_dir}/include CACHE INTERNAL "Path to include folder for cereal")
