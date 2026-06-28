#
# GLUS CMake file
#
# Note: Set OpenGL=ES, OpenGL=ES31 or OpenGL=ES2 for Windows OpenGL ES 3.0, 3.1 or 2.0 simulation.
#       Raspberry Pi and i.MX6 is default OpenGL ES 2.0.
#		Set SoC=iMX6 for i.MX6.
#
# (c) Norbert Nopper
# 

cmake_minimum_required(VERSION 3.14)

project(GLUS VERSION 1.0.1)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS X

	# Support only desktop

ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# Linux

	IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		# Raspberry Pi and i.MX6
		
		SET(OpenGL "ES2")
		
		add_definitions(-DGLUS_ES2)

		IF(${SoC} MATCHES "iMX6")
			# i.MX6

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

	ELSE()
		# Desktop
			
	ENDIF()
		
ENDIF()
		
# Files only valid for desktop OpenGL
list(APPEND ONLY_GL_C_FILES		${GLUS_SOURCE_DIR}/src/glus_window_glfw.c
								${GLUS_SOURCE_DIR}/src/glus_programpipeline.c
								${GLUS_SOURCE_DIR}/src/glus_program.c
								${GLUS_SOURCE_DIR}/src/glus_shape_adjacency.c
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
	
	IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		# Raspberry Pi and i.MX6

		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_android_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_windows_es.c)
		
		IF(${SoC} MATCHES "iMX6")			
			# i.MX6
			
			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_raspberrypi_es.c)
			
		ELSE()
			# Raspberry Pi
		
			list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_imx6_es.c)
			
		ENDIF()
		
	ELSE()
		# Windows simulation
	
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_android_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_imx6_es.c)
		list(REMOVE_ITEM C_FILES ${GLUS_SOURCE_DIR}/src/glus_os_linux_raspberrypi_es.c)		
		
	ENDIF()	
	
ELSE()
	# OpenGL 3 and 4
	
	list(REMOVE_ITEM C_FILES ${ES_C_FILES} ${ES2_C_FILES} ${ES31_C_FILES} ${VG_C_FILES})
	
ENDIF()

add_library(GLUS ${C_FILES} ${H_FILES})
target_include_directories (GLUS PUBLIC ${GLUS_SOURCE_DIR}/src)

# Desktop OpenGL needs GLEW and GLFW. When GLUS is built standalone these are
# fetched automatically. When GLUS is added by a parent project that already
# provides them (e.g. the McNopper/OpenGL examples), the existing targets are reused.
IF(NOT (${OpenGL} MATCHES "ES"))

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

		FetchContent_Declare(
			glew
			GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
			GIT_TAG        glew-cmake-2.2.0
		)

		FetchContent_GetProperties(glew)
		if(NOT glew_POPULATED)
			FetchContent_Populate(glew)
			# Patch the CMakeLists.txt to use a modern CMake version
			file(READ "${glew_SOURCE_DIR}/CMakeLists.txt" GLEW_CMAKE_CONTENT)
			string(REPLACE "cmake_minimum_required(VERSION 2.8.12)" "cmake_minimum_required(VERSION 3.5)" GLEW_CMAKE_CONTENT "${GLEW_CMAKE_CONTENT}")
			file(WRITE "${glew_SOURCE_DIR}/CMakeLists.txt" "${GLEW_CMAKE_CONTENT}")
			add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR})
		endif()

		target_include_directories(GLUS PUBLIC ${glew_SOURCE_DIR}/include)
	endif()

	target_link_libraries(GLUS PUBLIC libglew_static glfw)

ENDIF()