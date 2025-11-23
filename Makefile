CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET = ascii.exe
SOURCES = src/argparse.cpp src/image.cpp src/main.cpp src/print_image.cpp
HEADERS = include/argparse.hpp include/image.hpp include/print_image.hpp include/stb_image.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Iinclude $(SOURCES) -o $(TARGET)

clean:
	@if exist $(TARGET) del $(TARGET) 2>nul

#####################################################
# 	Build the program (default)						#
# 	=> make											#
#													#
# 	Clean up										#
# 	=> make clean									#
#													#
# 	Run the program	(Default options)				#
# 	=> ./ascii image.jpg							#
#													#
# 	Run with options (Custom options)				#
# 	=> ./ascii image.jpg -mw 120 --retro-colors		#
#####################################################
