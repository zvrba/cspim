cmake_minimum_required(VERSION 2.6)
PROJECT(MIPS)
ADD_DEFINITIONS(-g -Wall)

# TODO! Add optimization option
set(HOSTED ON CACHE BOOL "Hosted or embedded build.")
set(SKIP_APPS ON CACHE BOOL "Skip building mips apps in mipsapps")

ADD_SUBDIRECTORY(vm)
ADD_SUBDIRECTORY(mipsapps)

if(HOSTED)
	ADD_SUBDIRECTORY(hostapps)
endif(HOSTED)