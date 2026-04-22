cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED repo OR NOT DEFINED tag OR NOT DEFINED destination)
  message(FATAL_ERROR "repo, tag, and destination must be provided")
endif()

if(NOT DEFINED git_executable)
  set(git_executable git)
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E rm -rf "${destination}"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: ${destination}")
endif()

if(EXISTS "${repo}/.git")
  execute_process(
    COMMAND "${git_executable}" clone
            --branch "${tag}"
            "${repo}"
            "${destination}"
    RESULT_VARIABLE error_code
  )
else()
  execute_process(
    COMMAND "${git_executable}" clone
            --filter=blob:none
            --depth 1
            --branch "${tag}"
            "${repo}"
            "${destination}"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: ${repo}")
endif()

execute_process(
  COMMAND "${git_executable}" -C "${destination}" submodule update --recursive --init
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: ${destination}")
endif()
