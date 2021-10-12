file(GLOB_RECURSE SOURCES ./*.c)
add_library(src STATIC ${SOURCES})
