find_file(
  COMPILE_COMMANDS
  compile_commands.json
  HINTS ${CMAKE_BINARY_DIR}
  )
if(COMPILE_COMMANDS)
  file(
    COPY ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION ${PROJECT_SOURCE_DIR}
    )
  message(STATUS "Copying compile_commands.json from directory ${CMAKE_BINARY_DIR} to directory ${PROJECT_SOURCE_DIR}")
endif()
