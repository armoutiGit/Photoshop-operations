#ifndef _IMG_PROCESS_H_
#define _IMG_PROCESS_H_
#include <stdio.h>

/* This is the primary functino of the file.
 * function to initialize file pointers and images and provides some I/O error checks.
 * @param argc is number of command line arguments
 * @param argv is user input
 */
int img_processing(int argc, char *argv[]);

/* function to determine which operation the user wants
 * to execute and conduct some error checks specific to that operation.
 * @param argc is number of command line arguments
 * @param argv is user input
 * @param im is the user inputted image
 * @param fp is the file pointer to that user inputted image
 */
int operation(int argc, char *argv[], Image *im, FILE *fp);

/* Grayscale NTSC standard
 * function to perform grayscale operation on image.
 * @param im is the user inputted image
 */
void grayscale(Image *im);

/* Binarize operation
 * function to binarize image using threshold value.
 * @param im is the user inputted image
 * @param threshold is the value inputted by user to compare against pixels
 */
void binarize(Image *im, int threshold);

/* crop operation
 * function to crop image by considering specified co-ordinates.
 * @param im is the user inputted image
 * @param x1 column index of top left corner of new output image
 * @param y1 row index of top left corner of new output image
 * @param x2 column index of bottom right corner of new output image
 * @param y2 row index of bottom right corner of new output image
 * @param fp file pointer to input ppm file
 */
int crop(Image *im, int x1, int y1, int x2, int y2, FILE *fp);

/* transpose operation
 * function to flip dimension of image.
 * @param im is the user inputted image
 */
int transpose(Image *im);

/* Gradient operation
 * function to compute image gradient (essentially edge detection).
 * @param im is the user inputted image
 */
void gradient(Image *im);

/* seam operation
 * function to conduct (greedy minimization) seam carving on image iteratively.
 * @param im is the user inputted image
 * @param scaleCol is the column scale factor
 * @param scaleRow is the row scale factor
 */
void seam(Image *im, float scaleCol, float scaleRow);

/* helper method to identify the seam to be removed.
 * @param im is the gradient of the user inputted image
 * @param copy is a copy of the original, unedited user inputted image
 */
void seamCarve(Image *im, Image *copy);

/* helper method to identify direction of seam.
 * @param argc is number of command line arguments
 * @param argv is user input
 * @param im is the user inputted image
 */
int helpAssign(int mid, int right, int left, char position);

/* Another helper method to identify direction of
 * seam (i.e. position of next pixel in the seam).
 * @param im is the gradient image
 * @param a is the row being inspected
 * @param b is the column being inspected
 */
int assignDIR(Image *im, int a, int b);

/* helper method to actually remove an identified seam.
 * @param im is the user inputted image
 * @param seamArr is an array of seams, from which we will use one
 * @param targetIndex is the index of the first pixel of the seam in the image
 */
void removeSeam(Image *im, int *seamArr, int targetIndex);

#endif // _IMG_PROCESS_H_
