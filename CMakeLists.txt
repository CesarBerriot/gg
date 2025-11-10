cmake_minimum_required(VERSION 3.31)
project(gg)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(fetch_github_content GIT_REPOSITORY https://github.com/CesarBerriot/fetch_github_content.git GIT_TAG 1.0.0)
FetchContent_MakeAvailable(fetch_github_content)
include(${fetch_github_content_SOURCE_DIR}/fetch_github_content.cmake)

block()
  fetch_github_content(
    dial CesarBerriot 2.2.0
    discover_sources CesarBerriot 1.1.0 INCLUDE_DEFAULT
    cmrc vector-of-bool 2.0.1
    string_theory CesarBerriot 3.9
  )
endblock()

function(add_mingw_link_options target)
  if(MINGW)
    target_link_options(${target} PRIVATE -static)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_link_options(${target} PRIVATE -s)
    endif()
  endif()
endfunction()

function(setup_resource_library target)
  set(resource_library_name ${target}_resource_library)
  cmrc_add_resource_library(
    ${resource_library_name}
    NAMESPACE resources
    ${ARGN}
  )
  target_link_libraries(${target} PRIVATE ${resource_library_name})
endfunction()

add_subdirectory(application)
add_subdirectory(installer)