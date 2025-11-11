add_library(shared)
discover_sources(shared)
target_include_directories(shared PUBLIC include PRIVATE source)
target_link_libraries(shared PRIVATE SDL3-static)