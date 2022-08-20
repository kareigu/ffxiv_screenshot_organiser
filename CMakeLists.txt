cmake_minimum_required(VERSION 3.23)

project(ffxiv_screenshot_organiser VERSION 0.1)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")


set(SOURCE_FILES
  "src/main.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/bin"
)

add_subdirectory(vendor/fmt EXCLUDE_FROM_ALL)
add_subdirectory(vendor/ftxui EXCLUDE_FROM_ALL)

target_include_directories(${PROJECT_NAME} 
  PUBLIC "vendor/fmt/include"
  PUBLIC "vendor/ftxui/include"
)

target_link_directories(${PROJECT_NAME} 
  PRIVATE "vendor/fmt/src"
  PRIVATE "vendor/ftxui/src"
)

target_link_libraries(${PROJECT_NAME}
  fmt
  ftxui::screen
  ftxui::dom
  ftxui::component
)