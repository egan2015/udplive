
include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(APP_SRC test_uls.c)
add_executable(test_uls ${APP_SRC})
target_link_libraries(test_uls vlstp )

set(THREAD_SRC thread.c)
add_executable(thread ${THREAD_SRC})
target_link_libraries(thread pthread )

set(HASH_SRC hash.c)
add_executable(hash ${HASH_SRC})
