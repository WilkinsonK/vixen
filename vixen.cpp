#include <fstream>
#include <iostream>

#include "include/vixen.hpp"

using namespace std;

#define VIXEN_NAME    "Vixen"
#define VIXEN_VERSION "0.4.2"

struct VixenNamespace {
    std::string cinput;
    std::string exec;
    std::string file;
    bool        help;
    bool        version;
};

void usage(VixenNamespace vxn) {
    std::cout
        << "usage: " << vxn.exec << " [file?] [OPTIONS]\n"
           "Options:\n"
           "-c           Interperate input.\n"
           "-h/--help    Print help and exit.\n"
           "-V/--version Print exec version."
        << std::endl;
};

void print_error(VixenNamespace vxn, const std::string message) {
    std::cerr << vxn.exec << ": error: " << message << std::endl;
}

void panic(
    VixenNamespace vxn,
    const std::string message,
    int exit_code = 1,
    bool show_help = false) {

    print_error(vxn, message);
    if (show_help)
        usage(vxn);
    exit(exit_code);
}

std::string_view parse_option(
    const std::vector<std::string_view>& args, 
    const std::string_view& option_name) {
    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
        if (*it == option_name)
            if (it + 1 != end)
                return *(it + 1);
    }
    
    return "";
}

void parse(VixenNamespace& vxn, int argc, const char* argv[]) {
    vxn.cinput  = std::string();
    vxn.exec    = std::string(argv[0]);
    vxn.file    = std::string();
    vxn.help    = false;
    vxn.version = false;

    std::vector<std::string_view> args(argv + 1, argv + argc);
    std::string short_opts("Vch");
    bool skipping = false;
    for (const auto& arg : args) {
        if (skipping) {
            skipping = !skipping;
            continue;
        }

        // Apply our options here before attempting
        // to parse positionals. Order of parsing
        // doesn't matter, unless we need to exit
        // quickly.
        if (arg == "-h" || arg == "--help") {
            vxn.help = true;
            break;
        }
        if (arg == "-V" || arg == "--version") {
            vxn.version = true;
            break;
        }
        if (arg == "-c" || arg == "--cinput") {
            vxn.cinput = parse_option(args, arg);
            skipping = true;
            continue;
        }
        // If the argument looks like an option
        // but we cannot identify it, bail.
        if (arg.find('-') >= 0 && arg.find('-') < 2) {
            panic(vxn, "Unknown option: '" + std::string(arg) + "'.");
        }

        // Vixen currently only accepts one
        // positional arg, 'file'.
        if (!vxn.file.length()) {
            vxn.file = arg;
            continue;
        } else if (vxn.file.length()) {
            panic(vxn, "Unknown argument '" + std::string(arg) + "'.");
        }
    }

    if (vxn.help) {
        usage(vxn);
        exit(0);
    } else if (vxn.version) {
        std::cout << VIXEN_NAME " " VIXEN_VERSION << std::endl;
        exit(0);
    }

    if (!vxn.file.length() && !vxn.cinput.length())
        panic(vxn, "No input was provided.", 1, true);
    else if (vxn.file.length() && vxn.cinput.length())
        panic(vxn, "Cannot handle more than one input source.");
}

int main(int argc, const char* argv[]) {
    vixen::tokens::Lexer lexer;
    VixenNamespace vxn;
    parse(vxn, argc, argv);

    if (vxn.file.length()) {
        ifstream file(vxn.file);
        if (!file.is_open())
            panic(vxn, "Cannot open file '" + vxn.file + "'.");
        lexer = vixen::tokens::Lexer(file, vxn.file);
        file.close();
    } else {
        lexer = vixen::tokens::Lexer(vxn.cinput);
    }

    vixen::tokens::Token token;
    while (!lexer.end()) {
        token = lexer.next();
        std::cout << token << std::endl;
    }

    return 0;
}
