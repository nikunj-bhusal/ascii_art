#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

// Suppress warnings for stb_image
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#pragma GCC diagnostic pop

#include "../include/image.hpp"

using namespace std;

Image load_image(const string& file_path) {
    int width, height, channels;
    unsigned char* raw_data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);

    if (!raw_data) {
        cerr << "Error: Failed to load image '" << file_path << "': " << stbi_failure_reason() << "!" << endl;
        return Image();  // Return empty image on failure
    }

    // Convert to [0., 1.]
    size_t total_size = static_cast<size_t>(width) * height * channels;
    vector<double> data;
    data.reserve(total_size);

    for (size_t i = 0; i < total_size; i++) {
        data.push_back(raw_data[i] / 255.0);
    }

    stbi_image_free(raw_data);

    return Image(static_cast<size_t>(width), static_cast<size_t>(height), static_cast<size_t>(channels), move(data));
}

// Gets pointer to pixel data at index (x, y)
double* get_pixel(Image& image, size_t x, size_t y) {
    if (x >= image.width || y >= image.height) {
        throw out_of_range("Pixel coordinates out of bounds");
    }
    return &image.data[(y * image.width + x) * image.channels];
}

const double* get_pixel(const Image& image, size_t x, size_t y) {
    if (x >= image.width || y >= image.height) {
        throw out_of_range("Pixel coordinates out of bounds");
    }
    return &image.data[(y * image.width + x) * image.channels];
}

// Sets pixel channel values to those of new_pixel
void set_pixel(Image& image, size_t x, size_t y, const vector<double>& new_pixel) {
    if (new_pixel.size() != image.channels) {
        throw invalid_argument("New pixel channel count doesn't match image channels");
    }

    double* pixel = get_pixel(image, x, y);
    for (size_t c = 0; c < image.channels; c++) {
        pixel[c] = new_pixel[c];
    }
}

// Gets average pixel value in rectangular region; writes to `average`
void get_average(const Image& image, vector<double>& average, size_t x1, size_t x2, size_t y1, size_t y2) {
    if (average.size() != image.channels) {
        average.resize(image.channels, 0.0);
    } else {
        fill(average.begin(), average.end(), 0.0);
    }

    // Validate bounds
    x1 = min(x1, image.width);
    x2 = min(x2, image.width);
    y1 = min(y1, image.height);
    y2 = min(y2, image.height);

    // Get total
    for (size_t y = y1; y < y2; y++) {
        for (size_t x = x1; x < x2; x++) {
            const double* pixel = get_pixel(image, x, y);
            for (size_t c = 0; c < image.channels; c++) {
                average[c] += pixel[c];
            }
        }
    }

    // Divide by number of pixels in region
    double n_pixels = static_cast<double>((x2 - x1) * (y2 - y1));
    if (n_pixels > 0) {
        for (size_t c = 0; c < image.channels; c++) {
            average[c] /= n_pixels;
        }
    }
}

Image make_resized(const Image& original, size_t max_width, size_t max_height, double character_ratio) {
    size_t width, height;
    size_t channels = original.channels;

    // Note: Dividing heights by 2 for approximate terminal font aspect ratio
    size_t proposed_height = (original.height * max_width) / (character_ratio * original.width);
    if (proposed_height <= max_height) {
        width = max_width;
        height = proposed_height;
    } else {
        width = (character_ratio * original.width * max_height) / original.height;
        height = max_height;
    }

    // Ensure minimum dimensions
    width = max(width, static_cast<size_t>(1));
    height = max(height, static_cast<size_t>(1));

    vector<double> data(width * height * channels, 0.0);

    // i, j are coordinates in resized image
    for (size_t j = 0; j < height; j++) {
        size_t y1 = (j * original.height) / height;
        size_t y2 = ((j + 1) * original.height) / height;
        for (size_t i = 0; i < width; i++) {
            size_t x1 = (i * original.width) / width;
            size_t x2 = ((i + 1) * original.width) / width;

            vector<double> average(channels, 0.0);
            get_average(original, average, x1, x2, y1, y2);

            // Copy average to result
            size_t data_index = (i + j * width) * channels;
            for (size_t c = 0; c < channels; c++) {
                data[data_index + c] = average[c];
            }
        }
    }

    return Image(width, height, channels, move(data));
}

// Create grayscale version of image. Note: Assumes original is at least RGB.
Image make_grayscale(const Image& original) {
    if (original.channels < 3) {
        throw invalid_argument("Original image must have at least 3 channels for grayscale conversion");
    }

    size_t width = original.width;
    size_t height = original.height;
    size_t channels = 1;

    vector<double> data(width * height, 0.0);

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            const double* pixel = get_pixel(original, x, y);

            // Luminance-weighted grayscale
            double grayscale = 0.2126 * pixel[0] + 0.7152 * pixel[1] + 0.0722 * pixel[2];

            data[x + y * width] = grayscale;
        }
    }

    return Image(width, height, channels, move(data));
}

double calculate_convolution_value(const Image& image, const vector<double>& kernel, size_t x, size_t y, size_t c) {
    double result = 0.0;

    for (int j = -1; j < 2; j++) {
        for (int i = -1; i < 2; i++) {
            size_t image_x = x + i;
            size_t image_y = y + j;

            // Skip out-of-bounds pixels
            if (image_x >= image.width || image_y >= image.height) {
                continue;
            }

            size_t image_index = c + (image_x + image_y * image.width) * image.channels;
            size_t kernel_index = (i + 1) + (j + 1) * 3;

            result += kernel[kernel_index] * image.data[image_index];
        }
    }

    return result;
}

// Calculates convolution with 3x3 kernel. Handles edges safely.
void get_convolution(const Image& image, const vector<double>& kernel, vector<double>& out) {
    if (kernel.size() != 9) {
        throw invalid_argument("Kernel must be 3x3 (9 elements)");
    }

    if (out.size() != image.data.size()) {
        out.resize(image.data.size(), 0.0);
    } else {
        fill(out.begin(), out.end(), 0.0);
    }

    for (size_t y = 1; y < image.height - 1; y++) {
        for (size_t x = 1; x < image.width - 1; x++) {
            for (size_t c = 0; c < image.channels; c++) {
                size_t image_index = c + (x + y * image.width) * image.channels;
                out[image_index] = calculate_convolution_value(image, kernel, x, y, c);
            }
        }
    }
}

// Calculates sobel convolutions
void get_sobel(const Image& image, vector<double>& out_x, vector<double>& out_y) {
    vector<double> Gx = {-1., 0., 1., -2., 0., 2., -1., 0., 1.};
    vector<double> Gy = {1., 2., 1., 0., 0., 0., -1., -2., -1.};

    get_convolution(image, Gx, out_x);
    get_convolution(image, Gy, out_y);
}