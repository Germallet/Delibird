TARGET_LIB = libutils.a
LIBS = -lcommons -lpthread 
CC = gcc
CFLAGS = -Wall -g

.PHONY: default all clean

default: $(TARGET_LIB)
all: clean default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET_LIB): $(OBJECTS)
	ar rcs $(TARGET_LIB) $(OBJECTS)

clean:
	-rm -f *.o
	-rm -f $(TARGET_LIB)

compilarSiNoExiste:
	if [ -f ./libutils.a ]; then echo libUtils ya compilado!; else echo Compilando libUtils...; make default; fi