#ifndef MY_IMAGE_LIB
#define MY_IMAGE_LIB

#include <string>
#include <vector>

class Image {
   public:
    size_t width;
    size_t height;
    size_t channels;
    std::vector<double> data;

    // Constructors
    Image() : width(0), height(0), channels(0) {}

    Image(size_t w, size_t h, size_t c, std::vector<double>&& d = {}) : width(w), height(h), channels(c), data(std::move(d)) {}

    // Move constructor and assignment
    Image(Image&& other) = default;
    Image& operator=(Image&& other) = default;

    // Disallow copying (use references instead to avoid expensive copies)
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    // Helper methods
    bool empty() const { return data.empty(); }
};

// Image loading and processing functions
Image load_image(const std::string& file_path);

// Pixel access functions
double* get_pixel(Image& image, size_t x, size_t y);
const double* get_pixel(const Image& image, size_t x, size_t y);
void set_pixel(Image& image, size_t x, size_t y, const std::vector<double>& new_pixel);

// Image transformation functions
Image make_resized(const Image& original, size_t max_width, size_t max_height, double character_ratio);
Image make_grayscale(const Image& original);

// Region analysis
void get_average(const Image& image, std::vector<double>& average, size_t x1, size_t x2, size_t y1, size_t y2);

// Convolution operations
void get_convolution(const Image& image, const std::vector<double>& kernel, std::vector<double>& out);
void get_sobel(const Image& image, std::vector<double>& out_x, std::vector<double>& out_y);

// Utility function for convolution calculations
double calculate_convolution_value(const Image& image, const std::vector<double>& kernel, size_t x, size_t y, size_t c);

#endif  // MY_IMAGE_LIB