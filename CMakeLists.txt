cmake_minimum_required(VERSION 2.8)

project(ludum)

# enable c99 support
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SRC "*.c" "opengl-render-entity/*.c" "entity/*.c" "gameplay/*.c")
message("${SRC}")

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

pkg_search_module(GLEW REQUIRED glew)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

if (UNIX)
  # math
  link_libraries(m)
endif()

message("${SDL2_MIXER_LIBRARIES}")

add_executable(ludum ${SRC})
target_link_libraries(ludum
                      ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} 
                      ${SDL2_MIXER_LIBRARIES}
                      ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

install(TARGETS ludum
	RUNTIME DESTINATION .
)

