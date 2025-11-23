#ifndef MY_ARGPARSE
#define MY_ARGPARSE

#include <string>

struct Args {
    std::string file_path;
    size_t max_width;
    size_t max_height;
    double character_ratio;
    double edge_threshold;
    bool use_retro_colors;

    // Constructor with default values
    Args() : file_path(""), max_width(0), max_height(0), character_ratio(2.0), edge_threshold(4.0), use_retro_colors(false) {}
};

Args parse_args(int argc, char* argv[]);

#endif  // MY_ARGPARSE