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

        if (*it == "--mode" || *it == "-m") {
            if (++it != args.end()) {
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

            options.error_msg =
                "No mode specified after " + std::string(*(it - 1));
            options.error_code = 1;
            return;
        }

        if (*it == "--ip" || *it == "-i") {
            if (++it != args.end()) {
                options.host = std::string(*it);
                continue;
            }

            options.error_msg =
                "No IP address specified after " + std::string(*(it - 1));
            options.error_code = 1;
            return;
        }

        if (*it == "--port" || *it == "-p") {
            if (++it != args.end()) {
                try {
                    options.port = static_cast<std::uint16_t>(
                        std::stoul(std::string(*it)));
                    continue;
                } catch (const std::exception &e) {
                    options.error_msg  = "Invalid port: " + std::string(*it);
                    options.error_code = 1;
                    return;
                }
            }

            options.error_msg =
                "No port specified after " + std::string(*(it - 1));
            options.error_code = 1;
            return;
        }

        options.error_msg  = "Unknown argument: " + std::string(*it);
        options.error_code = 1;
        return;
    }

    if (!options.show_help && options.mode == MODE_UNDEFINED) {
        options.error_msg  = "No mode provided.";
        options.error_code = 1;
    }
}

void print_usage(const std::string_view progname) {
    std::printf("Usage: %s [-h | --help] -m <mode> -i <ip> -p <port>\n",
                progname.data());
}

void print_help(const std::string_view progname) {
    print_usage(progname);
    std::printf("\nOptions:\n"
                "-h, --help\t\tShow this help message and exit.\n"
                "-m, --mode <mode>\tSet the program mode (client or server).\n"
                "-i, --ip <ip>\t\tSet the IP address to bind/connect to.\n"
                "-p, --port <port>\tSet the port number to bind/connect to.\n");
}

} // namespace program
