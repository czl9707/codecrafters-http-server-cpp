add_executable(server server.cpp helper.cpp http_request.cpp http_response.cpp)
target_link_libraries(server PRIVATE pthread)
