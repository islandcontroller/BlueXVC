cmake_minimum_required(VERSION 3.20)

# Toolchain configuration
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_CONFIGS_PATH}/gcc-arm-none-eabi.cmake)


#- Project setup ---------------------------------------------------------------
project(BlueXVC)

# Language configuration
enable_language(C ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Default file endings
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_HEXFILE_SUFFIX ".hex")
set(CMAKE_LISTING_SUFFIX ".lst")
set(CMAKE_MAPFILE_SUFFIX ".map")


#- Common build setup ----------------------------------------------------------
# Toolchain common options
set(MACHINE_OPTIONS
	-march=armv7-m
	-mcpu=cortex-m3
	-mthumb
)

# Project-wide compiler options
add_compile_options(
	${MACHINE_OPTIONS}
	
	-fdata-sections
	-ffunction-sections
	-nostartfiles
	
	-Wall
	-Wextra
	
	-O1
	-g
)
add_compile_definitions(
	-DSTM32F103xB
)
include_directories(
	hw_layer
	Controller
	Controller/STM32F1xx
	Controller/STM32F1xx/Core
	Controller/STM32F1xx/Peripheral/inc
)

# Project-wide linker options
file(GLOB LINKER_FILE Controller/STM32F1xx/linker_script_stm32f103x8.ld)
cmake_path(REMOVE_FILENAME LINKER_FILE OUTPUT_VARIABLE LINKER_BASEDIR)
cmake_path(GET LINKER_FILE FILENAME LINKER_FILE)
add_link_options(
	-L${LINKER_BASEDIR}
	-T${LINKER_FILE}

	${MACHINE_OPTIONS}

	-specs=nano.specs
	-nostartfiles
	
	-lc
	-lm

	-Wl,--gc-sections
	-Wl,--print-memory-usage
)


#- Target specific setup -------------------------------------------------------
set(TARGET_NAME ${PROJECT_NAME})
set(TARGET_ADD_EXECUTABLE_MARKER ${CMAKE_CURRENT_LIST_LINE})
add_executable(${TARGET_NAME})

# Source files
file(GLOB_RECURSE TARGET_SOURCES *.c *.S)
list(FILTER TARGET_SOURCES EXCLUDE REGEX "build\/.*")
list(FILTER TARGET_SOURCES EXCLUDE REGEX "Controller\/.*\/Template\/.*")
target_sources(${TARGET_NAME} PRIVATE ${TARGET_SOURCES})

# Linker options
target_link_options(${TARGET_NAME} PRIVATE
	-Wl,-Map=${TARGET_NAME}${CMAKE_MAPFILE_SUFFIX},--cref
)

# Post-Build: register generated mapfile
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	BYPRODUCTS ${TARGET_NAME}${CMAKE_MAPFILE_SUFFIX}
)

# Post-Build: print section sizes
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_SIZE_UTIL} ${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}
)

# Post-Build: generate listings
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -d -S ${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX} > ${TARGET_NAME}${CMAKE_LISTING_SUFFIX}
	BYPRODUCTS ${TARGET_NAME}${CMAKE_LISTING_SUFFIX}
)

# Post-Build: generate HEX file
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX} ${TARGET_NAME}${CMAKE_HEXFILE_SUFFIX}
)

# Post-Build: status message
math(EXPR TARGET_DEF_LINE "${TARGET_ADD_EXECUTABLE_MARKER} + 1")
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	COMMAND echo ${CMAKE_SOURCE_DIR}/CMakeLists.txt:${TARGET_DEF_LINE}:1: info: Finished build for target ${TARGET_NAME}.
)