include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
    )

add_library(restengine SHARED librestengine.c)

install(TARGETS restengine DESTINATION lib)
