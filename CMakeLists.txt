cmake_minimum_required (VERSION 3.2)
project (optee_fiovb C)

# https://cmake.org/Wiki/CMake_Useful_Variables
set (CMAKE_TOOLCHAIN_FILE CMakeToolchain.txt)

include(GNUInstallDirs)

add_compile_options (-Wall)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

file(GLOB dirs *)
foreach(dir ${dirs})
	if(EXISTS ${dir}/CMakeLists.txt)
		add_subdirectory(${dir})
	endif()
endforeach()
