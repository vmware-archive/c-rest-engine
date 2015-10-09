add_executable(vmrestd globals.c main.c	service.c signal.c)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/public"
    "${PROJECT_BINARY_DIR}/include"
    )

target_link_libraries(vmrestd LINK_PUBLIC libcommon)

install(TARGETS vmrestd DESTINATION sbin)
