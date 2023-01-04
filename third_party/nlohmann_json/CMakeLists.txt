set(DownloadingStatus -1)

message("Json library downloading:")

file(DOWNLOAD 
  https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp  
  ${CMAKE_CURRENT_BINARY_DIR}/json.hpp
  SHOW_PROGRESS
  TIMEOUT 60
  INACTIVITY_TIMEOUT 10
  STATUS DownloadingStatus
  EXPECTED_HASH SHA256=665fa14b8af3837966949e8eb0052d583e2ac105d3438baba9951785512cf921
)

if(DownloadingStatus MATCHES 0)
  message("Json library downloading successed")
else()
  message(FATAL_ERROR "Json library downloading error")
endif()
