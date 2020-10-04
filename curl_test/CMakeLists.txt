cmake_minimum_required(VERSION 3.1)
project(curl_test_client)
add_executable(curl_test_client client.c)
target_link_libraries(curl_test_client z curl)
