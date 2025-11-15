//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file program.cpp
/// Implementation of functions for command-line argument handling.
///
//===----------------------------------------------------------------------===//

#include "program.h"

#include <vector>

namespace program {

void parse_arguments(int argc, char **argv, ProgramOptions &options) {
    std::vector<std::string_view> args(argv + 1, argv + argc);

    for (auto it = args.begin(); it != args.end(); ++it) {
        if (*it == "--help" || *it == "-h") {
            options.show_help = true;
            return;
        }

        if (options.mode == MODE_UNDEFINED) {
            if (*it == "client") {
                options.mode = MODE_CLIENT;
                continue;
            }

            if (*it == "server") {
                options.mode = MODE_SERVER;
                continue;
            }

            options.error_msg  = "Invalid mode: " + std::string(*it);
            options.error_code = 1;
            return;
        }

        if (options.host.empty()) {
            options.host = std::string(*it);
            continue;
        }

        if (options.port == 0) {
            int port = std::stoi(std::string(*it));
            if (port < 0 || port > 65535) {
                options.error_msg  = "Invalid port: " + std::string(*it);
                options.error_code = 1;
                return;
            }

            options.port = static_cast<std::uint16_t>(port);
            continue;
        }

        options.error_msg  = "Unknown argument: " + std::string(*it);
        options.error_code = 1;
        return;
    }

    if (!options.show_help) {
        options.error_msg  = "No mode provided.";
        options.error_code = 1;
    }
}

void print_usage(const std::string_view progname) {
    std::printf("Usage: %s [-h | --help] <mode> <ip> <port>\n",
                progname.data());
}

void print_help(const std::string_view progname) {
    print_usage(progname);
    std::printf("\nOptions:\n"
                "-h, --help\t\tShow this help message and exit.\n"
                "<mode>\tSet the program mode (client or server).\n"
                "<ip>\t\tSet the IP address to bind/connect to.\n"
                "<port>\t\tSet the port number to bind/connect to.\n");
    std::printf("\nExamples:\n"
                "\t%sserver 4444\n"
                "\t%sclient 127.0.0.1 4444\n",
                progname.data(),
                progname.data());
}

} // namespace program
