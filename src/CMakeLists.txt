cmake_minimum_required(VERSION 3.2)
project(web_services)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(/usr/local/include/mongocxx/v_noabi)
include_directories(/usr/local/include/libmongoc-1.0)
include_directories(/usr/local/include/bsoncxx/v_noabi)
include_directories(/usr/local/include/libbson-1.0)

set(SOURCE_FILES main.cpp mail.cpp)

link_directories(/usr/local/lib)
add_library(MailServiceFastCGI SHARED ${SOURCE_FILES})
target_link_libraries(MailServiceFastCGI mongocxx)
target_link_libraries(MailServiceFastCGI bsoncxx)
target_link_libraries(MailServiceFastCGI fastcgi-daemon2)

install(TARGETS MailServiceFastCGI LIBRARY DESTINATION lib
                                   RUNTIME DESTINATION lib)
