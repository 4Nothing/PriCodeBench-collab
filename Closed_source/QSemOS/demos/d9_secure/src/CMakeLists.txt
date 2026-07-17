file(GLOB C_SRCS    "*.c"
                    "*.S")

set(SRCS ${C_SRCS})
add_library(src OBJECT ${SRCS})