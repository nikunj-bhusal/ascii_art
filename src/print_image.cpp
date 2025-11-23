#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/image.hpp"

using namespace std;

// Characters to print
const string VALUE_CHARS = " .-=+*x#$&X@";
const size_t N_VALUES = VALUE_CHARS.size();

// Color ANSI codes
const string RESET = "\x1b[0m";

struct HSV {
    double hue;
    double saturation;
    double value;
};

double get_max(double a, double b, double c) { return max({a, b, c}); }

double get_min(double a, double b, double c) { return min({a, b, c}); }

HSV rgb_to_hsv(double red, double green, double blue) {
    HSV hsv;

    double max_val = get_max(red, green, blue);
    double min_val = get_min(red, green, blue);

    hsv.value = max_val;
    double chroma = hsv.value - min_val;

    // Calculate saturation
    if (abs(hsv.value) < 1e-4) {
        hsv.saturation = 0.0;
    } else {
        hsv.saturation = chroma / hsv.value;
    }

    // Calculate hue
    if (chroma < 1e-4) {
        hsv.hue = 0.0;
    } else if (max_val == red) {
        hsv.hue = 60.0 * fmod((green - blue) / chroma, 6.0);
        if (hsv.hue < 0.0) hsv.hue += 360.0;
    } else if (max_val == green) {
        hsv.hue = 60.0 * (2.0 + (blue - red) / chroma);
    } else {
        hsv.hue = 60.0 * (4.0 + (red - green) / chroma);
    }

    return hsv;
}

void hsv_to_rgb(const HSV& hsv, double& r, double& g, double& b) {
    double c = hsv.value * hsv.saturation;
    double h_prime = hsv.hue / 60.0;
    double x = c * (1.0 - abs(fmod(h_prime, 2.0) - 1.0));

    double r1, g1, b1;

    if (h_prime >= 0.0 && h_prime < 1.0) {
        r1 = c;
        g1 = x;
        b1 = 0.0;
    } else if (h_prime >= 1.0 && h_prime < 2.0) {
        r1 = x;
        g1 = c;
        b1 = 0.0;
    } else if (h_prime >= 2.0 && h_prime < 3.0) {
        r1 = 0.0;
        g1 = c;
        b1 = x;
    } else if (h_prime >= 3.0 && h_prime < 4.0) {
        r1 = 0.0;
        g1 = x;
        b1 = c;
    } else if (h_prime >= 4.0 && h_prime < 5.0) {
        r1 = x;
        g1 = 0.0;
        b1 = c;
    } else {
        r1 = c;
        g1 = 0.0;
        b1 = x;
    }

    double m = hsv.value - c;
    r = r1 + m;
    g = g1 + m;
    b = b1 + m;
}

void get_retro_rgb(const HSV& hsv, int& out_r, int& out_g, int& out_b) {
    // For retro colors: quantize hue and saturation for 8-color palette
    HSV quantized_hsv = hsv;

    // Set value to full brightness (character controls apparent brightness)
    quantized_hsv.value = 1.0;

    // Quantize hue to nearest multiple of 60 degrees (6 hues: R, Y, G, C, B, M)
    quantized_hsv.hue = round(quantized_hsv.hue / 60.0) * 60.0;
    if (quantized_hsv.hue >= 360.0) {
        quantized_hsv.hue = 0.0;
    }

    // Quantize saturation: either 0% (grayscale) or 100% (full color)
    quantized_hsv.saturation = (quantized_hsv.saturation < 0.25) ? 0.0 : 1.0;

    // Convert back to RGB
    double r, g, b;
    hsv_to_rgb(quantized_hsv, r, g, b);

    // Convert to 0-255 range
    out_r = static_cast<int>(r * 255);
    out_g = static_cast<int>(g * 255);
    out_b = static_cast<int>(b * 255);
}

double calculate_grayscale_from_hsv(const HSV& hsv) {
    // Use value * value for increased contrast
    return hsv.value * hsv.value;
}

char get_ascii_char(double grayscale) {
    grayscale = max(0.0, min(1.0, grayscale));  // Clamp to [0, 1]
    size_t index = static_cast<size_t>(grayscale * N_VALUES);

    // Clamp
    if (index >= N_VALUES) {
        index = N_VALUES - 1;
    }

    return VALUE_CHARS[index];
}

char get_sobel_angle_char(double sobel_angle) {
    if ((22.5 <= sobel_angle && sobel_angle <= 67.5) || (-157.5 <= sobel_angle && sobel_angle <= -112.5))
        return '\\';
    else if ((67.5 <= sobel_angle && sobel_angle <= 112.5) || (-112.5 <= sobel_angle && sobel_angle <= -67.5))
        return '_';
    else if ((112.5 <= sobel_angle && sobel_angle <= 157.5) || (-67.5 <= sobel_angle && sobel_angle <= -22.5))
        return '/';
    else
        return '|';
}

void print_image(const Image& image, double edge_threshold, bool use_retro_colors) {
    Image grayscale = make_grayscale(image);
    vector<double> sobel_x(grayscale.width * grayscale.height, 0.0);
    vector<double> sobel_y(grayscale.width * grayscale.height, 0.0);

    if (edge_threshold < 4.0) {
        get_sobel(grayscale, sobel_x, sobel_y);
    }

    for (size_t y = 0; y < image.height; y++) {
        for (size_t x = 0; x < image.width; x++) {
            const double* pixel = get_pixel(image, x, y);

            size_t index = y * image.width + x;
            double sx = sobel_x[index];
            double sy = sobel_y[index];

            double square_sobel_magnitude = sx * sx + sy * sy;
            double sobel_angle = atan2(sy, sx) * 180.0 / M_PI;

            char ascii_char;
            double grayscale;
            int r = 255, g = 255, b = 255;  // Default white for grayscale

            if (image.channels <= 2) {
                // Grayscale image
                grayscale = pixel[0];
                r = g = b = static_cast<int>(pixel[0] * 255);
            } else {
                // RGB image
                HSV hsv = rgb_to_hsv(pixel[0], pixel[1], pixel[2]);

                grayscale = calculate_grayscale_from_hsv(hsv);

                // Set value to full brightness for both modes
                // Character choice controls apparent brightness, not color value
                hsv.value = 1.0;

                if (use_retro_colors) {
                    // Retro mode: quantize hue to 60° and saturation to 0% or 100%
                    get_retro_rgb(hsv, r, g, b);
                } else {
                    // Truecolor mode: convert HSV back to RGB with full brightness
                    double r_d, g_d, b_d;
                    hsv_to_rgb(hsv, r_d, g_d, b_d);
                    r = static_cast<int>(r_d * 255);
                    g = static_cast<int>(g_d * 255);
                    b = static_cast<int>(b_d * 255);
                }
            }

            ascii_char = get_ascii_char(grayscale);

            // If edge
            if (square_sobel_magnitude >= edge_threshold * edge_threshold) {
                ascii_char = get_sobel_angle_char(sobel_angle);
            }

            // Use 24-bit truecolor ANSI escape code
            cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m" << ascii_char;
        }
        cout << endl;
    }

    cout << RESET;
}