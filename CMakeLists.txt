cmake_minimum_required(VERSION 3.24)
project(DynDNS)

set(CMAKE_CXX_STANDARD 23)
set(SYSTEMD_UNIT_FILE_PATH "/usr/lib/systemd/user/")

find_package(fmt)

include_directories(/usr/include/libxml2)
add_executable(DynDNS src/main.cpp src/libs/domain.cpp src/libs/domain.h src/libs/parseArgs.cpp src/libs/parseConfig.cpp src/libs/parseArgs.h src/libs/parseConfig.h)
target_link_libraries(DynDNS PRIVATE fmt::fmt curl curlcpp xml2 jsoncpp)


set(INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
configure_file(dyndns.service.in dyndns.service @ONLY)
configure_file(dyndns.timer.in dyndns.timer.in @ONLY)
install(TARGETS DynDNS RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dyndns.service ${CMAKE_CURRENT_BINARY_DIR}/dyndns.timer DESTINATION ${SYSTEMD_UNIT_FILE_PATH})
install(CODE "execute_process(COMMAND /usr/bin/systemctl --user daemon-reload)")