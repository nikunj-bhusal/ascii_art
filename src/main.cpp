#include <iostream>
#include <memory>

#include "../include/argparse.hpp"
#include "../include/image.hpp"
#include "../include/print_image.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // Parse arguments
    Args args = parse_args(argc, argv);
    if (args.file_path.empty()) {
        return 1;
    }

    try {
        // Load image
        Image original = load_image(args.file_path);
        if (original.data.empty()) {
            cerr << "Error: Failed to load image data!" << endl;
            return 1;
        }

        // Resize image
        Image resized = make_resized(original, args.max_width, args.max_height, args.character_ratio);
        if (resized.data.empty()) {
            cerr << "Error: Failed to resize image!" << endl;
            return 1;
        }

        // Print the ASCII art
        print_image(resized, args.edge_threshold, args.use_retro_colors);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Error: Unknown exception occurred!" << endl;
        return 1;
    }

    return 0;
}