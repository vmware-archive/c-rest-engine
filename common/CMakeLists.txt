add_library(libcommon libmain.c memory.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
    )
