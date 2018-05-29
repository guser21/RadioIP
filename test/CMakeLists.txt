file(GLOB client_test
        "client/*.cpp"
        )

file(GLOB server_test
        "server/*.cpp"
        )

link_libraries(gtest gtest_main pthread)

add_executable(clientTest ${client_test})
add_executable(serverTest ${server_test})
add_test(check serverTest clientTest)
# Link test executable against gtest & gtest_main
