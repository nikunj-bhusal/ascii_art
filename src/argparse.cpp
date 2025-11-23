#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <io.h>
#include <windows.h>

#else
#include <sys/ioctl.h>
#endif

#include <unistd.h>

#include "../include/argparse.hpp"

using namespace std;

constexpr size_t DEFAULT_MAX_WIDTH = 64;
constexpr size_t DEFAULT_MAX_HEIGHT = 48;
constexpr double DEFAULT_CHARACTER_RATIO = 2.0;
constexpr double DEFAULT_EDGE_THRESHOLD = 4.0;

void print_help(const string& exec_alias) {
    cout << "USAGE:\n";
    cout << "\t" << exec_alias << " <path/to/image> [OPTIONS]\n\n";

    cout << "ARGUMENTS:\n";
    cout << "\t<path/to/image>\t\tPath to image file\n\n";

    cout << "OPTIONS:\n";
    cout << "\t-mw <width>\t\tMaximum width in characters (default: terminal width OR " << DEFAULT_MAX_WIDTH << ")\n";
    cout << "\t-mh <height>\t\tMaximum height in characters (default: terminal height OR " << DEFAULT_MAX_HEIGHT << ")\n";
    cout << "\t-et <threshold>\t\tEdge detection threshold, range: 0.0 - 4.0 (default: " << DEFAULT_EDGE_THRESHOLD << ", disabled)\n";
    cout << "\t-cr <ratio>\t\tHeight-to-width ratio for characters (default: " << DEFAULT_CHARACTER_RATIO << ")\n";
    cout << "\t--retro-colors\t\tUse 3-bit retro color palette (8 colors) instead of 24-bit truecolor\n";
}

bool try_get_terminal_size(size_t& width, size_t& height) {
#ifdef _WIN32
    // Windows implementation
    if (!_isatty(0)) return false;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return false;

    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return true;
#else
    // POSIX implementation
    if (!isatty(0)) return false;
    struct winsize ws;

    if (ioctl(0, TIOCGWINSZ, &ws) == 0) {
        width = static_cast<size_t>(ws.ws_col);
        height = static_cast<size_t>(ws.ws_row);
        return true;
    }
#endif
    return false;
}

Args parse_args(int argc, char* argv[]) {
    Args args;

    // Try to get terminal size for defaults
    size_t term_width, term_height;
    if (try_get_terminal_size(term_width, term_height)) {
        args.max_width = term_width;
        args.max_height = term_height;
    } else {
        args.max_width = DEFAULT_MAX_WIDTH;
        args.max_height = DEFAULT_MAX_HEIGHT;
    }

    // If no file given
    if (argc == 1) {
        print_help(argv[0]);
        return args;
    }

    // Get file path
    if (strcmp(argv[1], "-h") == 0) {
        print_help(argv[0]);
        return args;
    } else {
        args.file_path = argv[1];
    }

    // Get optional parameters
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-mw" && i + 1 < argc) {
            args.max_width = static_cast<size_t>(atoi(argv[++i]));
        } else if (arg == "-mh" && i + 1 < argc) {
            args.max_height = static_cast<size_t>(atoi(argv[++i]));
        } else if (arg == "-et" && i + 1 < argc) {
            args.edge_threshold = atof(argv[++i]);
        } else if (arg == "-cr" && i + 1 < argc) {
            args.character_ratio = atof(argv[++i]);
        } else if (arg == "--retro-colors") {
            args.use_retro_colors = true;
        } else {
            cerr << "Warning: Ignoring invalid or incomplete argument '" << argv[i] << "'" << endl;
        }
    }

    return args;
}