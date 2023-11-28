CC = g++
CFLAGS = -Wall
TARGET = gal

FILE_NAMES_PATHS = src/gal structures/DataReader structures/Node structures/Vehicle structures/Request libs/pugixml 
FILE_NAMES = gal DataReader pugixml Node Vehicle Request
sources = $(FILE_NAMES_PATHS:=.cpp)
objects = $(FILE_NAMES:=.o)

.PHONY: all clean pack

all: $(TARGET)

$(TARGET): $(objects)
		$(CC) $(CFLAGS) -o $(TARGET) $^
		$(MAKE) clean

$(objects): $(sources)
		$(CC) $(CFLAGS) -c $^

clean: $(objects)
		rm -f $^

#pack: all
#	zip gal.zip Makefile $(sources)
