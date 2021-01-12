/*****************************************************************************
 * This file implements a program for image processing operations.
 *          Different operations take different input arguments. In general,
 *            ./project <input> <output> <operation name> [operation params]
 *          The program will return 0 and write an output file if successful.
 *          Otherwise, the below error codes should be returned:
 *            1: Wrong usage (i.e. mandatory arguments are not provided)
 *            2: Input file I/O error
 *            3: Output file I/O error
 *            4: The Input file cannot be read as a PPM file
 *            5: Unsupported image processing operations
 *            6: Incorrect number of arguments for the specified operation
 *            7: Invalid arguments for the specified operation
 *            8: Other errors 
 *****************************************************************************/
#include <stdio.h>
#include "ppm_io.h" // PPM I/O header
#include "img_processing.h"

int main(int argc, char *argv[]) {
  
  return img_processing(argc, argv);
  
}
