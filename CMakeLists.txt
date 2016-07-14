project(fw)
set(CMAKE_MIN_VERSION 2.8)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

SET(CMAKE_CXX_FLAGS "-std=c++11")
# set(CMAKE_BUILD_TYPE Release)

if (CMAKE_BUILD_TYPE MATCHES Release)
	message(STATUS "fw target is Release")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "fw target is Debug")
else()
	message(STATUS "fw target is default, using Debug")
	set(CMAKE_BUILD_TYPE Debug)
endif()

add_definitions(-Wall)
add_definitions(-DPROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug)

if (UNIX)
	set(_SP_X11 1)
else()
	message(FATAL_ERROR "Unsupported target platform")
endif()


### MODELS (download and extract from internet)

set(MODEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/models)

set(MODELS
	# ${MODEL_DIR}/dragon
)

set(MODEL_URLS
	# http://graphics.cs.williams.edu/data/meshes/dragon.zip
)

list(LENGTH MODEL_URLS n_urls)
math(EXPR max_i "${n_urls} - 1")

foreach(i RANGE ${max_i})
	list(GET MODELS ${i} model)
	list(GET MODEL_URLS ${i} model_url)

	add_custom_command(
		OUTPUT ${model}
		COMMAND wget -O ${model}.zip ${model_url}
		COMMAND unzip -d ${model} ${model}.zip
		COMMAND rm ${model}.zip
	)

endforeach()

### END MODELS


set(EXT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(${EXT_PATH}/glfw)

find_package(OpenGL REQUIRED)

include(FindOpenAL)

include(cmake-modules/FindALUT.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${EXT_PATH}
	${EXT_PATH}/glfw/include
	${EXT_PATH}/glew-cmake/include
	${EXT_PATH}/glm
	${EXT_PATH}/conglomerate/include
	${EXT_PATH}/assimp/include
	${EXT_PATH}/Simple-OpenGL-Image-Library/src
	${EXT_PATH}/imgui
	${OPENGL_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${ALUT_INCLUDE_DIR}
)

add_subdirectory(${EXT_PATH}/conglomerate)

add_subdirectory(${EXT_PATH}/glew-cmake/build/cmake)

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
add_subdirectory(${EXT_PATH}/assimp)

add_subdirectory(${EXT_PATH}/Simple-OpenGL-Image-Library)

file(GLOB ImGui_sources ${EXT_PATH}/imgui/*.cpp)
add_library(ImGui STATIC ${ImGui_sources})
target_link_libraries(ImGui GL glfw glew_s ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})


file(GLOB_RECURSE all_shaders
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.tesc
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.tese
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.geom
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.comp)
list(REMOVE_DUPLICATES all_shaders)

set(PROJECT_LIB fworks)
set(PROJECT_EXE fireworks)
set(PROJECT_MAIN_CC ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp)

file(GLOB_RECURSE all_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_DUPLICATES all_sources)
list(REMOVE_ITEM all_sources ${PROJECT_MAIN_CC})

file(GLOB_RECURSE all_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
list(REMOVE_DUPLICATES all_headers)

set(EXT_LIBS
	GL
	pthread
	${OPENGL_LIBRARIES}
	glew_s
	glfw
	${GLFW_LIBRARIES}
	conglomerate
	assimp
	SOIL
	ImGui
	${OPENAL_LIBRARY}
	${ALUT_LIBRARIES}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(${PROJECT_LIB} ${all_sources} ${all_headers} ${all_shaders})
target_link_libraries(${PROJECT_LIB} ${EXT_LIBS})

add_executable(${PROJECT_EXE} ${PROJECT_MAIN_CC} ${MODELS})
target_link_libraries(${PROJECT_EXE} ${PROJECT_LIB})









