# <your name> and <your JHED>
# __Add your name and JHED above__
# Midterm Project - Solution Makefile

CC=gcc -g
CFLAGS=-std=c99 -pedantic -Wall -Wextra

## TODO: Write your own Makefile commands to link and compile your project as well as the cleanup


## Below are commands to link and compile the checkerboard program
# Links together files needed to create the checkerboard executable
checkerboard: checkerboard.o ppm_io.o
	$(CC) -o $@ checkerboard.o ppm_io.o

project: project.o ppm_io.o img_processing.o
	$(CC) -o project project.o ppm_io.o img_processing.o

project.o: project.c ppm_io.h img_processing.h
	$(CC) $(CFLAGS) -c project.c

# Compile the ppm i/o source code
ppm_io.o: ppm_io.c ppm_io.h
	$(CC) $(CFLAGS) -c ppm_io.c

# Compile the image processing source code
img_processing.o: img_processing.c img_processing.h
	$(CC) $(CFLAGS) -c img_processing.c

# Removes all object files and the executable named project, so we can start fresh
clean:
	rm -f *.o checkerboard project
