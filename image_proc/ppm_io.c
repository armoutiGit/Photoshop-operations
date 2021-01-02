// <your name> and <your JHED>
// __Add your name and JHED above__
/*****************************************************************************
 * Midterm Project - PPM file input/output header file
 * Note: for naming convention, we try to follow Google C++ style guide:
 *       https://google.github.io/styleguide/cppguide.html
 * It is not compulsory, but you are highly encouraged to follow a convention.
 *
 * Summary: This file implements the utility functions to read/write PPM file
 *****************************************************************************/
#include "ppm_io.h" // PPM I/O header
#include <stdlib.h> // c functions: malloc, free
#include <assert.h> // c functions: assert
#include <string.h> // c functions: strncmp, memcpy
#include <ctype.h>  // c functions: isspace

/* ReadNum
 * helper function for ReadPPM, takes a filehandle
 * and reads a number, but detects and skips comment lines
 */
int ReadNum(FILE *fp) {
  /* confirm that we received a good file handle */
  assert(fp);

  int ch;
  while((ch = fgetc(fp)) == '#') { // # marks a comment line
    while( ((ch = fgetc(fp)) != '\n') && ch != EOF ) {
      /* discard characters til end of line */
    }
  }
  ungetc(ch, fp); // put back the last thing we found

  int val;
  if (fscanf(fp, "%d", &val) == 1) { // try to get an int
    while(isspace(ch = fgetc(fp))) {
      // drop trailing whitespace
    }
    ungetc(ch, fp);
    return val; // we got a value, so return it
  } else {
    fprintf(stderr, "Error:ppm_io - failed to read number from file\n");
    return -1;
  }
}

/* ReadPPM
 * Read a PPM-formatted image from a file (assumes fp != NULL).
 * Returns the address of the heap-allocated Image struct it
 * creates and populates with the Image data.
 */
Image* ReadPPM(FILE *fp) {
  // check that fp is not NULL
  assert(fp);

  // allocate image (but not space to hold pixels -- yet)
  Image *im = malloc(sizeof(Image));
  if (!im) {
    fprintf(stderr, "Error:ppm_io - failed to allocate memory for image!\n");
    return NULL;
  }

  // initialize fields to error codes, in case we have to bail out early
  im->rows = im->cols = -1;

  // read in tag; fail if not P6
  char tag[20];
  tag[19]='\0';
  fscanf(fp, "%19s\n", tag);
  //printf("%sz\n", tag);
  if (strncmp(tag, "P6", 20)) {
    fprintf(stderr, "Error:ppm_io - not a PPM (bad tag)\n");
    free(im);
    return NULL;
  }

  /* read image dimensions */ 
  //read in columns
  im->cols = ReadNum(fp); // NOTE: cols, then rows (i.e. X size followed by Y size)
  //read in rows
  im->rows = ReadNum(fp);

  //read in colors; fail if not 255
  int colors = ReadNum(fp);
  if (colors != 255) {
    fprintf(stderr, "Error:ppm_io - PPM file with colors different from 255\n");
    free(im);
    return NULL;
  }

  //confirm that dimensions are positive
  if (im->cols <= 0 || im->rows <= 0) {
    fprintf(stderr, "Error:ppm_io - PPM file with non-positive dimensions\n");
    free(im);
    return NULL;
  }

  // allocate the right amount of space for the Pixels
  im->data = malloc(sizeof(Pixel) * (im->rows) * (im->cols));

  if (!im->data) {
    fprintf(stderr, "Error:ppm_io - failed to allocate memory for image pixels!\n");
    free(im);
    return NULL;
  }

  // read in the binary Pixel data
  int num_pixels_read = (int) fread(im->data, sizeof(Pixel), (im->rows) * (im->cols), fp);
  if (num_pixels_read != (im->rows) * (im->cols)) {
    fprintf(stderr, "Error:ppm_io - failed to read data from file with size %d (read %d)!\n", (im->rows) * (im->cols), num_pixels_read);
    destroy(im);
    return NULL;
  }

  //return the image struct pointer
  return im;
}

/* WritePPM
 * Write a PPM-formatted image to a file (assumes fp != NULL),
 * and return the number of pixels successfully written.
 */
int WritePPM(FILE *fp, const Image *im) {
  // check that fp and im are not NULL
  assert(fp); 
  assert(im);

  // write PPM file header, in the following format
  // P6
  // cols rows
  // 255
  fprintf(fp, "P6\n%d %d\n%d\n", im->cols, im->rows, 255);

  // now write the pixel array
  int num_pixels_written = (int) fwrite(im->data, sizeof(Pixel), (im->rows) * (im->cols), fp);

  // check if write was successful or not; indicate failure with -1
  if (num_pixels_written != (im->rows) * (im->cols)) {
    fprintf(stderr, "Error:Uh oh. Pixel data failed to write properly!\n");
    return -1;
  }
  
  // add a newline character before ending the file
  fprintf(fp, "\n");

  // return number of pixels written
  return num_pixels_written;
}

int printError(int err, FILE *fp) {

  switch (err)
  {
    case 1:
        fprintf(stderr, "Mandatory arguments not provided (input file name, output file name, operation)\n");
        return 1;
    case 2:
        fprintf(stderr, "Input file could not be opened for reading\n");
        fclose(fp);
        return 2;
    case 3:
        fprintf(stderr, "Output file could not be opened for writing\n");
        fclose(fp);
        return 3;
    case 4:
        fprintf(stderr, "Input file cannot be read as PPM file\n");
        fclose(fp);
        return 4;
    case 5:
        fprintf(stderr, "Unsupported image processing operations\n");
        fclose(fp);
        return 5;
    case 6:
        fprintf(stderr, "Incorrect number of arguments for the specified operation\n");
        fclose(fp);
        return 6;
      case 7:
        fprintf(stderr, "Invalid arguments for the specified operation\n");
        fclose(fp);
        return 7;
      case 8:
        fprintf(stderr, "Error writing output file\n");
        fclose(fp);
        return 8;
      default:
        break;
  }

  return 0;
}

int writePPMfile(char *argv[], Image *im) {
  FILE *outfp = fopen(argv[2], "wb");
  if (!outfp) {return printError(3, outfp);}

  int written = WritePPM(outfp, im);
  fclose(outfp);
  return written;
}

void destroy(Image *im) {
  free(im->data);
  free(im);
}


void copyIm(Image *im, Image *copy) {
  copy->data = malloc(sizeof(Pixel) * im->cols * im->rows);
  copy->cols = im->cols;
  copy->rows = im->rows;
  for (int r = 0; r < im->rows; r++) {
    for (int c = 0; c < im->cols; c++) {
      copy->data[(r * im->cols) + c] = im->data[(r * im->cols) + c];
    }
  }
}
