//
// Created by guser on 5/28/18.
//
#include <boost/exception/diagnostic_information.hpp>
#include <server/server_options.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <common/const.h>

namespace po = boost::program_options;

ServerOptions ServerOptionParser::parse(int argc, char **argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            (",h", "Available params")
            (",a", po::value<std::string>()->default_value(MCAST_ADDR), "set multicast ip address")
            (",P", po::value<uint16_t>()->default_value(DATA_PORT), "set data data_port")
            (",C", po::value<uint16_t>()->default_value(CTRL_PORT), "set control data_port")
            (",p", po::value<int>()->default_value(PSIZE), "set packet size ")
            (",f", po::value<int>()->default_value(FSIZE), "set fifo buffer size ")
            (",R", po::value<int>()->default_value(RTIME_SERVER), "retransmission period in milliseconds")
            (",n", po::value<std::string>()->default_value(STATION_NAME), "station name");


    ServerOptions serverOptions;
    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("-h")) {
            std::cout << desc << std::endl;
            exit(0);
        }

        serverOptions.mcast_addr = vm["-a"].as<std::string>();
        serverOptions.data_port = vm["-P"].as<uint16_t>();
        serverOptions.ctrl_port = vm["-C"].as<uint16_t>();
        serverOptions.packet_size = vm["-p"].as<int>();
        serverOptions.fifo_size = vm["-f"].as<int>();
        serverOptions.rtime = vm["-R"].as<int>();
        serverOptions.station_name = vm["-n"].as<std::string>();

    } catch (std::exception &err) {
        std::cout << err.what() << std::endl;
        exit(1);
    }

    return serverOptions;
}
