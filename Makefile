CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET = ascii
SOURCES = src/argparse.cpp src/image.cpp src/main.cpp src/print_image.cpp
HEADERS = include/argparse.hpp include/image.hpp include/print_image.hpp include/stb_image.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Iinclude $(SOURCES) -o $(TARGET)

debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

release: CXXFLAGS += -O3 -march=native
release: $(TARGET)

clean:
	rm -f $(TARGET)

deps:
	@if [ ! -f "include/stb_image.h" ]; then \
		echo "Downloading stb_image.h..."; \
		wget -q -O include/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h; \
	else \
		echo "stb_image.h already exists"; \
	fi

.PHONY: debug release clean deps

#####################################################
# 	Download dependency first (if needed)			#
# 	=> make deps									#
#													#
# 	Build the program (default)						#
# 	=> make											#
#													#
# 	Build debug version								#
# 	=> make debug									#
#													#
# 	Build with maximum optimization					#
# 	=> make release									#
#													#
# 	Clean up										#
# 	=> make clean									#
#													#
# 	Run the program									#
# 	=> ./ascii image.jpg						#
#													#
# 	Run with options								#
# 	=> ./ascii image.jpg -mw 120 --retro-colors	#
#####################################################