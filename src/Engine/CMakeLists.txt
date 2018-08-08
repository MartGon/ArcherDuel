# Add files in this folder
file(GLOB Engine_src "*.cpp"
					  "Components/*.cpp"
					  "GUI/*.cpp" 
					  "Managers/*.cpp"
					  "Networking/*.cpp"
					  "Util/*.cpp")

add_library(Engine ${Engine_src})
target_include_directories (Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} Components GUI Managers Networking Util)
