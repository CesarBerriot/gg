block()
  fetch_github_content(sub_process CesarBerriot 1.0.0)
endblock()

add_executable(application WIN32)
set_target_properties(application PROPERTIES OUTPUT_NAME "gg")
target_include_directories(application PRIVATE source)
target_compile_definitions(application PRIVATE MODULE_NAME="GG")
target_link_libraries(application PRIVATE string_theory sub_process)
discover_sources(application)
dial_link_target(application)
add_mingw_link_options(application)
add_custom_command(
  TARGET application
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:application> ${CMAKE_SOURCE_DIR}/installer
)
block()
  file(GLOB_RECURSE resources CONFIGURE_DEPENDS resource/*)
  setup_resource_library(application ${resources})
endblock()
