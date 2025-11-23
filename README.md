# 🎨 ASCII Art Generator
A CLI based tool **developed in C++** that displays image files as ASCII art in terminal.

### Heavily inspired by [Xander Gouws's](https://www.youtube.com/@XanderGouws) video on [ascii-view](https://github.com/gouwsxander/ascii-view) that he developed in C.

## How to use?
```bash
# For the executable:
make

# To clean build artifacts:
make clean
```

## Usage
```bash
# Powershell / Git Bash
./ascii <path_to_the_image> [OPTIONS]

# Command Prompt
ascii.exe <path_to_the_image> [OPTIONS]
```

### Options
- `-mw <width>`: Maximum width in characters (default: terminal width OR 64)
- `-mh <height>`: Maximum height in characters (default: terminal height OR 48)
- `-et <threshold>`: Edge detection threshold, range: 0.0 - 4.0 (default 4.0, disabled)
- `-cr <ratio>`: Height-to-width ratio for characters (default 2.0)
- `--retro-colors`: Uses 3-bit colors for pixels.

<mark>Tip: Decreasing font size (zooming out in the terminal) can help improve the quality. 😊</mark>

### Examples
```bash
# Basic usage with default dimensions
./ascii-view examples/image.jpg

# Specify custom dimensions
./ascii-view examples/image.jpg -mw 120 -mh 60

# Specify edge threshold
./ascii-view examples/image.jpg -et 2.5

# Specify character aspect ratio
./ascii-view examples/image.jpg -cr 1.7
```
