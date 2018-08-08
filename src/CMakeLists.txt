

# Add subfolder
add_subdirectory(Engine)

# Add files in this folder
file(GLOB sdlpong_SRC "*.h" "*.cpp")

add_executable(SDLPong ${sdlpong_SRC})

# Libraries to be linked
target_link_libraries(SDLPong ${SDL2_LIBRARIES} ${SDL2_IMG_LIBRARIES} ${SDL2_NET_LIBRARIES} Engine)