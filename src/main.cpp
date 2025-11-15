//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file main.cpp
/// Entry point for the NoHub CLI tool.
///
//===----------------------------------------------------------------------===//

#include "core/client.h"
#include "core/server.h"
#include "program.h"

#include <cstdlib>

/// \brief Main entry point for the NoHub CLI tool.
///
/// \param argc Argument count.
/// \param argv Argument vector.
/// \return Exit code.
int main(int argc, char **argv) {
    struct program::ProgramOptions options;
    program::parse_arguments(argc, argv, options);

    if (options.show_help) {
        program::print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc < 7) {
        program::print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!options.error_msg.empty()) {
        std::fprintf(stderr, "Error: %s\n", options.error_msg.c_str());
        program::print_usage(argv[0]);
        return options.error_code;
    }

    try {
        if (options.mode == program::MODE_CLIENT) {
            core::Client client(options.host, options.port);
            client.run_interactive();
        } else if (options.mode == program::MODE_SERVER) {
            core::Server server(options.port);
            server.run();
        }
    } catch (const std::exception &e) {
        std::fprintf(stderr, "main: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
