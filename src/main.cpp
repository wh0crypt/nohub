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

    return EXIT_SUCCESS;
}
