cmake_minimum_required(VERSION 2.6)
PROJECT(MIPS)
ADD_DEFINITIONS(-g -Wall)

# TODO! Add optimization option
set(VIRTUALIZED OFF CACHE BOOL "Hosted or embedded build.")
set(MIPS_BE OFF CACHE BOOL "Big or little endian MIPS target.")
set(SKIP_APPS ON CACHE BOOL "Skip building mips apps in mipsapps.")
set(SKIP_SSTEP ON CACHE BOOL "Skip sstep (useful on non-POSIX).")

if(MIPS_BE)
	ADD_DEFINITIONS(-DMIPS_BE)
endif(MIPS_BE)

ADD_SUBDIRECTORY(vm)
ADD_SUBDIRECTORY(mipsapps)

if(NOT VIRTUALIZED)
	ADD_SUBDIRECTORY(hostapps)
endif(NOT VIRTUALIZED)
