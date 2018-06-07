//
// Created by guser on 5/28/18.
//

#include <client/client_options.h>
#include <boost/program_options.hpp>
#include <common/const.h>
#include <iostream>

namespace po=boost::program_options;

ClientOptions ClientOptionsParser::parse(int argc, char **argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            (",h", "Available params")
            (",d", po::value<std::string>()->default_value(DISCOVER_ADDR), "set discover address")
            (",C", po::value<uint16_t>()->default_value(CTRL_PORT), "set control data_port")
            (",U", po::value<uint16_t>()->default_value(UI_PORT), "set ui data_port")
            (",B", po::value<int>()->default_value(BSIZE), "set buffer size ")
            (",n", po::value<std::string>(), "set desired station name ")
            (",R", po::value<int>()->default_value(RTIME_CLIENT), "lookup request period in seconds");

    ClientOptions clientOptions;
    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("-h")) {
            std::cerr << desc << std::endl;
            exit(0);
        }


        if (vm.count("-n")) {
            clientOptions.prefer_station = true;
            clientOptions.prefered_station = vm["-n"].as<std::string>();
        }

        clientOptions.discover_addr = vm["-d"].as<std::string>();
        clientOptions.ctrl_port = vm["-C"].as<uint16_t>();
        clientOptions.ui_port = vm["-U"].as<uint16_t>();
        clientOptions.buffer_size = vm["-B"].as<int>();
        clientOptions.rtime = vm["-R"].as<int>();
    } catch (std::exception &err) {
        std::cerr << err.what() << std::endl;
        exit(1);
    }

    return clientOptions;
}
