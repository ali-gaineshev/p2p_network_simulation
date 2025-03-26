# Collect all the source files
file(GLOB P2P_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/code/p2p/*.cc)

# Create a library from your source files
add_library(p2p-lib ${P2P_SOURCES})

# Link this library to your scratch program
target_link_libraries(scratch_network-sim p2p-lib)
