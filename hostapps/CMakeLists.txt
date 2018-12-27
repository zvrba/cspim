project(HOST_APPS)
ADD_DEFINITIONS(-O3)
LINK_LIBRARIES(mipsvm)
INCLUDE_DIRECTORIES(${MIPS_SOURCE_DIR}/vm ${MIPS_SOURCE_DIR}/vm/hosted)
ADD_EXECUTABLE(runtorture runtorture.c util.c rc5-16.c)
ADD_EXECUTABLE(run run.c util.c rc5-16.c)
ADD_EXECUTABLE(runbench runbench.c util.c rc5-16.c)
ADD_EXECUTABLE(elfcrypt elfcrypt.c util.c rc5-16.c)
ADD_EXECUTABLE(load-lvl1 load-lvl1.c util.c rc5-16.c)

# Benchmarking stuff
if (NOT SKIP_SSTEP)
    LINK_LIBRARIES(mipsvm rt)
    ADD_EXECUTABLE(sstep sstep.c)
endif(NOT SKIP_SSTEP)

ADD_EXECUTABLE(hanoi-bench-native ${MIPS_APPS_SOURCE_DIR}/hanoi-bench.c)
ADD_EXECUTABLE(mmult-bench-native ${MIPS_APPS_SOURCE_DIR}/mmult-bench.c)
SET_TARGET_PROPERTIES(hanoi-bench-native mmult-bench-native
                      PROPERTIES COMPILE_FLAGS -DNATIVE)
