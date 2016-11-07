include_directories(${PROJECT_SOURCE_DIR}/include)
set(LIB_SRC libuls.c)
add_library(uls SHARED ${LIB_SRC})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)