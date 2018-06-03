//
// Created by guser on 6/1/18.
//
#include <gtest/gtest.h> // googletest header file

#include <string>
#include <common/const.h>
#include <server/streaming_service.h>
#include <server/control_daemon.h>
#include <server/server_options.h>
#include <fcntl.h>

TEST(DiffTest, INTEGRATION) {
    int in_fd, out_fd;
    const char *in_path = "./in.txt", *out_path = "./out.txt";
    in_fd = open(in_path, 0);
    out_fd = open(out_path,O_CREAT | O_WRONLY );


    ServerOptionParser serverOptionParser;
    auto server_options = serverOptionParser.parse(1, (char **) "/");

    StreamingService streamingService(server_options, nullptr);

    streamingService.setup();

    streamingService.setInput_fd(in_fd);
    streamingService.setDiag_fd(out_fd);

    streamingService.start();

    close(in_fd);
    close(out_fd);

};