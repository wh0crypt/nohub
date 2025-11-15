//===----------------------------------------------------------------------===//
//
// Part of the NoHub Project.
// See LICENSE for license information.
//
//===----------------------------------------------------------------------===//
///
/// \file program.h
/// Functions for command-line argument handling.
///
//===----------------------------------------------------------------------===//

#ifndef PROGRAM_H
#define PROGRAM_H

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace program {

/// \brief Enumeration for program modes.
typedef enum { MODE_UNDEFINED = 0, MODE_CLIENT, MODE_SERVER } ProgramMode;

/// \brief Structure to hold parsed program options.
///
/// This structure contains the mode, host, port, error messages,
/// error codes, and a flag to indicate if help should be shown.
struct ProgramOptions {
    ProgramMode   mode       = MODE_UNDEFINED;
    std::string   host       = std::string();
    std::uint16_t port       = 0;
    std::string   error_msg  = std::string();
    int           error_code = EXIT_SUCCESS;
    bool          show_help  = false;
};

/// \brief Parse command-line arguments.
///
/// This function processes the command-line arguments and populates
/// a `ProgramOptions` structure with the parsed values.
///
/// \param argc Argument count.
/// \param argv Argument vector.
/// \param options Reference to a `ProgramOptions` structure to populate.
void parse_arguments(int argc, char **argv, ProgramOptions &options);

/// \brief Print usage information for the nohub CLI tool.
///
/// \param progname Name of the program (argv[0]).
void print_usage(const std::string_view progname);

/// \brief Print detailed help information for the nohub CLI tool.
///
/// \param progname Name of the program (argv[0]).
void print_help(const std::string_view progname);

/// \brief Read configuration from a file.
///
/// This function reads key-value pairs from the specified configuration file
/// and returns them as an unordered map.
///
/// \param filepath Path to the configuration file.
/// \return An unordered map containing the configuration key-value pairs.
std::unordered_map<std::string, std::string>
read_config_file(const std::string_view filepath);

/// \brief Load configuration from a file into ProgramOptions.
///
/// \param filepath Path to the configuration file.
/// \param options Reference to a `ProgramOptions` structure to populate.
void load_config_file(const std::string_view filepath, ProgramOptions &options);

} // namespace program

#endif // PROGRAM_H
