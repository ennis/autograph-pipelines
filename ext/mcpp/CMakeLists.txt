# mcpp
project(mcpp C)
set(MCPP_SOURCES directive.c eval.c expand.c main.c mbchar.c support.c system.c)
add_library(mcpp STATIC ${MCPP_SOURCES})

