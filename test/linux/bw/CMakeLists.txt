
set(SOURCES slaveinfo_bw.c)
add_executable(slaveinfo_bw ${SOURCES})
target_link_libraries(slaveinfo_bw soem)
install(TARGETS slaveinfo_bw DESTINATION bin)
