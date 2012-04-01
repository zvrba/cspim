cmake_minimum_required(VERSION 2.6)
project(MIPS)
add_definitions(-g -Wall)
if(NOT HOSTED)
       add_definitions(-ffreestanding)
endif(NOT HOSTED)

# TODO! Add optimization option
set(HOSTED ON CACHE BOOL "Hosted or embedded build.")

add_subdirectory(vm)
add_subdirectory(mipsapps)

if(HOSTED)
	ADD_SUBDIRECTORY(hostapps)
endif(HOSTED)
