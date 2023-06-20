set(DownloadingStatus -1)

message("Boost library downloading:")

execute_process (
    COMMAND bash -c "sudo apt-get install libboost-all-dev"
    OUTPUT_VARIABLE DownloadingStatus
)

if(DownloadingStatus MATCHES 0)
  message("Boost downloading successed")
else()
  message(FATAL_ERROR "Boost downloading error")
endif()
