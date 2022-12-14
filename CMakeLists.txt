cmake_minimum_required(VERSION 3.18)
project(taip)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2GFX REQUIRED SDL2_gfx)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11 -pedantic -g")

set(SRCS
  main.c
  game.c
  render.c
  logic.c
  util.c
  config.c
)

set(HEADERS
  game.h
  render.h
  logic.h
  util.h
  config.h
)

add_executable(taip ${SRCS} ${HEADERS})
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(taip ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES} -lm)
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    target_link_libraries(taip -L/usr/local/lib ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})
endif()
