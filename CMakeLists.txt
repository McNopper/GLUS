#
# GLUS CMake file
#
# Note: Set OpenGL=ES, OpenGL=ES31 or OpenGL=ES2 for Windows OpenGL ES 3.0, 3.1 or 2.0 simulation.
#       Set OpenGL=VG for OpenVG 1.1. Set SoC=fbdev for the Linux framebuffer
#       windowing (e.g. Vivante on STM32MP1), SoC=Linux for generic arm Linux.
#       Raspberry Pi and fbdev default to OpenGL ES 2.0.
#
# (c) Norbert Nopper
# 

cmake_minimum_required(VERSION 3.14)

project(GLUS VERSION 1.1.6)

# Build policy + static analysis. Registered ONLY when GLUS is the top-level
# project: the example projects vendor GLUS through add_subdirectory() and
# FetchContent, and a second `cppcheck` custom target would make CMake fail
# that build outright. None of this is required to build GLUS - with the tools
# absent and every option at its default the build is unchanged.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
	include(cppcheck)
	include(warnings)
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS X

	# Support only desktop

ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# Linux

	IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		# Raspberry Pi and fbdev windowing
		
		IF(NOT OpenGL)
			SET(OpenGL "ES2")
		ENDIF()
		
		
		IF(${SoC} MATCHES "fbdev")
			# fbdev windowing

			# Everything is in /usr/include

		ELSE()
			# Raspberry Pi

			include_directories(/opt/vc/include /opt/vc/include/interface/vcos/pthreads /opt/vc/include/interface/vmcs_host/linux)		

		ENDIF()
		
	ELSE()
		# Support only desktop
					
	ENDIF()
	
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# Windows
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
	IF(${OpenGL} MATCHES "ES31")
		# Desktop OpenGL ES 3.1

		add_definitions(-DGLUS_ES31)
	
		include_directories(C:\\Imagination\\PowerVR_Graphics\\PowerVR_SDK\\SDK_2016_R1.2\\Builds\\Include)	

	ELSEIF(${OpenGL} MATCHES "ES2")
		# Desktop OpenGL ES 2.0

		add_definitions(-DGLUS_ES2)
	
		include_directories(C:\\Imagination\\PowerVR_Graphics\\PowerVR_SDK\\SDK_2016_R1.2\\Builds\\Include)	

	ELSEIF(${OpenGL} MATCHES "ES")
		# Desktop OpenGL ES 3.0

		add_definitions(-DGLUS_ES)
	
		include_directories(C:\\Imagination\\PowerVR_Graphics\\PowerVR_SDK\\SDK_2016_R1.2\\Builds\\Include)	

	ELSEIF(${OpenGL} MATCHES "VG")
		# OpenVG 1.1. The VG/ and EGL/ headers are provided by the consuming project.

		add_definitions(-DGLUS_VG11)

	ELSE()
		# Desktop
			
	ENDIF()
		
ENDIF()
		
# Flavor define for the GL/glus.h dispatch. Set on every platform, unlike the
# simulation defines above, so the ES and VG flavors also work on the desktop.
IF(${OpenGL} MATCHES "ES31")
	add_definitions(-DGLUS_ES31)
ELSEIF(${OpenGL} MATCHES "ES2")
	add_definitions(-DGLUS_ES2)
ELSEIF(${OpenGL} MATCHES "ES")
	add_definitions(-DGLUS_ES)
ELSEIF(${OpenGL} MATCHES "VG")
	add_definitions(-DGLUS_VG11)
ENDIF()

# Files only valid for desktop OpenGL. glus_ibl.c needs
# GL_TEXTURE_CUBE_MAP_ARRAY, which no OpenGL ES version provides.
list(APPEND ONLY_GL_C_FILES		${GLUS_SOURCE_DIR}/src/glus_window_glfw.c
								${GLUS_SOURCE_DIR}/src/glus_programpipeline.c
								${GLUS_SOURCE_DIR}/src/glus_program.c
								${GLUS_SOURCE_DIR}/src/glus_shape_adjacency.c
								${GLUS_SOURCE_DIR}/src/glus_time.c
								${GLUS_SOURCE_DIR}/src/glus_gltf.c
								${GLUS_SOURCE_DIR}/src/glus_ibl.c
)

# Files currently not used
list(APPEND NOT_USED_C_FILES	${GLUS_SOURCE_DIR}/src/glus_memory_nodm.c
)

# Source files
file(GLOB C_FILES ${GLUS_SOURCE_DIR}/src/*.c)
file(GLOB ES_C_FILES ${GLUS_SOURCE_DIR}/src/*_es.c)
file(GLOB ES2_C_FILES ${GLUS_SOURCE_DIR}/src/*_es2.c)
file(GLOB ES31_C_FILES ${GLUS_SOURCE_DIR}/src/*_es31.c)
file(GLOB VG_C_FILES ${GLUS_SOURCE_DIR}/src/*_vg.c)

# Header files
file(GLOB_RECURSE H_FILES ${GLUS_SOURCE_DIR}/src/*.h)

list(REMOVE_ITEM C_FILES ${NOT_USED_C_FILES})

IF(${OpenGL} MATCHES "ES")

	list(REMOVE_ITEM C_FILES ${VG_C_FILES})

	IF(${OpenGL} MATCHES "ES31")
		# OpenGL ES 3.1

		list(REMOVE_ITEM C_FILES ${ONLY_GL_C_FILES} ${ES2_C_FILES})

	ELSEIF(${OpenGL} MATCHES "ES2")
		# OpenGL ES 2.0

		list(REMOVE_ITEM C_FILES ${ONLY_GL_C_FILES} ${ES31_C_FILES})

	ELSE()
		# OpenGL ES 3.0

		list(REMOVE_ITEM C_FILES ${ONLY_GL_C_FILES} ${ES31_C_FILES} ${ES2_C_FILES})

	ENDIF()

ELSEIF(${OpenGL} MATCHES "VG")
	# OpenVG 1.1: the window/EGL and OS files are shared with the ES flavors.
	# Shader programs, draw helpers and the GL twins of the *_vg.c files are
	# outside the API surface of VG/glus.h and are not compiled.

	list(REMOVE_ITEM C_FILES ${ONLY_GL_C_FILES} ${ES2_C_FILES} ${ES31_C_FILES}
		${GLUS_SOURCE_DIR}/src/glus_program_es.c
		${GLUS_SOURCE_DIR}/src/glus_animation.c
		${GLUS_SOURCE_DIR}/src/glus_extension.c
		${GLUS_SOURCE_DIR}/src/glus_log.c
		${GLUS_SOURCE_DIR}/src/glus_screenshot.c
		${GLUS_SOURCE_DIR}/src/glus_version.c
		${GLUS_SOURCE_DIR}/src/glus_shape.c
		${GLUS_SOURCE_DIR}/src/glus_shape_texgen.c
		${GLUS_SOURCE_DIR}/src/glus_shape_wavefront.c
		${GLUS_SOURCE_DIR}/src/glus_line.c
		${GLUS_SOURCE_DIR}/src/glus_line_wavefront.c
		${GLUS_SOURCE_DIR}/src/glus_wavefront.c
	)

ELSE()
	# OpenGL 3 and 4

	list(REMOVE_ITEM C_FILES ${ES_C_FILES} ${ES2_C_FILES} ${ES31_C_FILES} ${VG_C_FILES})

ENDIF()

# Platform OS backends for the ES and VG flavors. The files are not guarded
# internally, so exactly one per platform is kept.
IF(${OpenGL} MATCHES "ES|VG")

	IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		# Raspberry Pi and fbdev windowing

		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_android_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_windows_es.c)

		IF(${SoC} MATCHES "fbdev")
			# fbdev windowing (e.g. Vivante on STM32MP1)

			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_es.c)
			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_raspberrypi_es.c)

		ELSEIF(${SoC} MATCHES "Linux")
			# Generic arm Linux: X11 window via glus_os_linux_es.c

			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_fbdev_es.c)
			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_raspberrypi_es.c)

		ELSE()
			# Raspberry Pi

			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_es.c)
			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_fbdev_es.c)

		ENDIF()

ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		# Desktop Windows simulation

		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_android_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_fbdev_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_raspberrypi_es.c)

	ELSE()
		# Desktop Linux

		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_android_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_windows_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_fbdev_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_raspberrypi_es.c)

	ENDIF()

ENDIF()

add_library(GLUS ${C_FILES} ${H_FILES})

# No-op at the default settings; raises the level and/or makes it fatal when
# ENABLE_STRICT_WARNINGS / ENABLE_WERROR are turned on. PRIVATE, so consumers
# of GLUS never inherit these flags.
if(COMMAND project_enable_warnings)
	project_enable_warnings(GLUS)
endif()
target_include_directories (GLUS PUBLIC ${GLUS_SOURCE_DIR}/src)

# Desktop OpenGL needs GLEW and GLFW. When GLUS is built standalone these are
# fetched automatically. When GLUS is added by a parent project that already
# provides them (e.g. the McNopper/OpenGL examples), the existing targets are reused.
IF(NOT (${OpenGL} MATCHES "ES|VG"))

	include(FetchContent)

	if(NOT TARGET glfw)
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
		set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

		FetchContent_Declare(
			glfw
			GIT_REPOSITORY https://github.com/glfw/glfw.git
			GIT_TAG        3.4
		)
		FetchContent_MakeAvailable(glfw)
	endif()

	if(NOT TARGET libglew_static)
		set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
		set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)
		set(ONLY_LIBS ON CACHE BOOL "" FORCE)

		# glew-cmake still declares cmake_minimum_required(VERSION 2.8.12).
		# CMake 4 refuses anything below 3.5 and warns below 3.10, so it is
		# rewritten before the project is configured. The single-argument
		# FetchContent_Populate() that used to do this is deprecated (CMP0169)
		# and becomes an error with CMake >= 3.30, so the patch now runs through
		# PATCH_COMMAND - which is also how the example projects fetch this
		# dependency. Every replacement below is idempotent (the replacement
		# text does not contain its own search text), so re-running the patch on
		# each configure is safe.
		set(GLEW_PATCH_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/patch_glew.cmake)
		file(WRITE ${GLEW_PATCH_SCRIPT}
			"file(READ \"\${GLEW_CMAKELISTS}\" GLEW_CMAKE_CONTENT)\n"
			"string(REPLACE \"cmake_minimum_required(VERSION 2.8.12)\" \"cmake_minimum_required(VERSION 3.10)\" GLEW_CMAKE_CONTENT \"\${GLEW_CMAKE_CONTENT}\")\n"
			"string(REPLACE \"find_library(AGL_LIBRARY AGL)\" \"# AGL is not part of modern macOS SDKs; GL symbols resolve through the OpenGL framework\" GLEW_CMAKE_CONTENT \"\${GLEW_CMAKE_CONTENT}\")\n"
			"file(WRITE \"\${GLEW_CMAKELISTS}\" \"\${GLEW_CMAKE_CONTENT}\")\n"
		)

		FetchContent_Declare(
			glew
			GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
			GIT_TAG        glew-cmake-2.2.0
			PATCH_COMMAND ${CMAKE_COMMAND} -DGLEW_CMAKELISTS=<SOURCE_DIR>/CMakeLists.txt -P ${GLEW_PATCH_SCRIPT}
		)

		FetchContent_MakeAvailable(glew)

		target_include_directories(GLUS PUBLIC ${glew_SOURCE_DIR}/include)
	endif()

	target_link_libraries(GLUS PUBLIC libglew_static glfw)

	# cgltf v1.14 - single-header glTF 2.0 parser, and stb_image.h for image
	# decoding. Used by the desktop-only glTF loader (glus_gltf.c). The include
	# directories are exposed publicly so consumers can read glTF extension data
	# directly from cgltf when they need to.
	FetchContent_Declare(
		cgltf
		GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
		GIT_TAG        v1.14
	)
	FetchContent_MakeAvailable(cgltf)

	FetchContent_Declare(
		stb
		GIT_REPOSITORY https://github.com/nothings/stb.git
		# Pinned to a commit hash rather than the moving `master` ref: a release
		# has to be reproducible, and stb_image parses untrusted input here.
		GIT_TAG        2c980bb59875b0d32144a71867fbdebb2f77cd20
	)
	FetchContent_MakeAvailable(stb)

	target_include_directories(GLUS PUBLIC ${cgltf_SOURCE_DIR} ${stb_SOURCE_DIR})

	# Absolute path to the GLUS shader directory, so applications can load the
	# glTF / IBL shaders straight from this source tree without a copied folder.
	target_compile_definitions(GLUS PUBLIC GLUS_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shader")

ENDIF()