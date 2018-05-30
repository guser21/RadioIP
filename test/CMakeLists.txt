file(GLOB client_test
        "client/*.cpp"

        )

file(GLOB server_test
        "server/*.cpp"
        )

link_libraries(gtest gtest_main pthread)

add_executable(clientTest ${client_test} ${client_src} ${common_src} )
add_executable(serverTest ${server_test})
# Link test executable against gtest & gtest_main
