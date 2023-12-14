CC = g++
CFLAGS = -Wall -Wextra -g -O2
TARGET = gal

FILE_NAMES_PATHS = src/gal src/genetic src/savings src/util structures/DataReader structures/Node structures/Vehicle structures/Request libs/pugixml
FILE_NAMES = gal genetic savings util DataReader pugixml Node Vehicle Request
sources = $(FILE_NAMES_PATHS:=.cpp)
objects = $(FILE_NAMES:=.o)

.PHONY: all clean pack

all: $(TARGET)

$(TARGET): $(objects)
	$(CC) $(CFLAGS) -o $(TARGET) $^

$(objects): $(sources)
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f $(objects)

pack: all
	zip -r xfiala61.zip Makefile src/ data/ results/ structures/ libs/ documentation.pdf presentation.pdf requirements.txt compare.py compare-data.py gather-data.py plot-path.py README.md
