
## How to use?

```bash
# To use the tool, first:
make 
```
To clean build artifacts:
```bash
make clean
```

## Usage

```bash
./ascii-view <path/to/image> [OPTIONS]
```

### Options

- `-mw <width>`: Maximum width in characters (default: terminal width OR 64)
- `-mh <height>`: Maximum height in characters (default: terminal height OR 48)
- `-et <threshold>`: Edge detection threshold, range: 0.0 - 4.0 (default 4.0, disabled)
- `-cr <ratio>`: Height-to-width ratio for characters (default 2.0)
- `--retro-colors`: Uses 3-bit colors for pixels.

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
