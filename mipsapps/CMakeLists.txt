set(CMAKE_SYSTEM_NAME "Generic")

# MIPS xcompiler mipsel-elf-gcc not available on most systems.
# Unspecified/default is not to skip, so explicit builds in this folder will always build apps. 
if (NOT SKIP_APPS)
	if (MIPS_BE)
		set(CMAKE_C_COMPILER mips-elf-gcc)
	else()
		set(CMAKE_C_COMPILER mipsel-elf-gcc)
	endif(MIPS_BE)
endif (NOT SKIP_APPS)

# Even if unavailable, still add mips apps source dir for other projects.
project(MIPS_APPS LANGUAGES C)

if (NOT SKIP_APPS)
	add_definitions(-Wall -O3 -mips1 -mno-check-zero-division -mlong-calls)
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,-q -nostdlib -Ttext 0x1000")
	set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

	include_directories(cputorture)
	if (MIPS_BE)
		include_directories(cputorture/be)
		add_executable(cputorture cputorture/be/cputorture.s)
		set_property(SOURCE cputorture/be/cputorture.s PROPERTY LANGUAGE C)
	else()
		include_directories(cputorture/le)
		add_executable(cputorture cputorture/le/cputorture.s)
		set_property(SOURCE cputorture/le/cputorture.s PROPERTY LANGUAGE C)
	endif(MIPS_BE)

	add_executable(hello hello.c spim.c)
	add_executable(console console.c spim.c)
	add_executable(file file.c spim.c)
	add_executable(fact fact.c spim.c)
	add_executable(adder adder.c spim.c)
	add_executable(max3sum max3sum.c spim.c)
	add_executable(digits digits.c spim.c)
	add_executable(hanoi hanoi.c spim.c)

	add_executable(mmult-bench mmult-bench.c spim.c)
	add_executable(hanoi-bench hanoi-bench.c spim.c)

	# Unspecified assumes hosted, so this is only built in parent project build or if requested explicitly (cmake arg).
	if(VIRTUALIZED)
		include_directories(${MIPS_SOURCE_DIR}/vm ${MIPS_SOURCE_DIR}/vm/cspim)
		add_executable(run-lvl1 run-lvl1.c spim.c)
		target_link_libraries(run-lvl1 mipsvm)
	endif(VIRTUALIZED)
endif(NOT SKIP_APPS)
