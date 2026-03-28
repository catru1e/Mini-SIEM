cmake_minimum_required(VERSION 3.16)
project(mini_siem_usb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Outputs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)

# Includes
include_directories(
  ${CMAKE_SOURCE_DIR}/common/include
  ${CMAKE_SOURCE_DIR}/third_party
)

# ---- server ----
add_executable(mini_siem_server
  server/src/main.cpp
  server/src/db_sqlite.cpp
  server/src/db_sqlite.hpp
  server/src/detect.cpp
  server/src/detect.hpp
  server/src/correlate.cpp
  server/src/correlate.hpp
)

target_link_libraries(mini_siem_server PRIVATE sqlite3)

# ---- agent  ----
add_executable(mini_siem_agent
  agent/src/main.cpp
  agent/src/http_client.cpp
  agent/src/http_client.hpp
  agent/src/log_reader.cpp
  agent/src/state_store.cpp
  agent/src/auth_parser.cpp
)

