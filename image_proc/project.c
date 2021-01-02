// Abdullah Al Armouti, JHED: aalarmo1
// Chad Oliver, JHED: colive21
/*****************************************************************************
 * Midterm Project - A program to run the image processing operations
 * Note: for naming convention, we try to follow Google C++ style guide:
 *       https://google.github.io/styleguide/cppguide.html
 * It is not compulsory, but you are highly encouraged to follow a convention.
 *
 * Summary: This file implements a program for image processing operations.
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

// TODO: include required headers for your projects.  
// We recommend to put your image processing operations in 
//  img_processing.h for declarations and
//  img_processing.c for their definitions
// Then you should include the below header:
//#include "img_processing.h" // Image processing header

int main(int argc, char *argv[]) {

  return img_processing(argc, argv);
}
