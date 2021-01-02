#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include "ppm_io.h"
#include "img_processing.h"

int img_processing(int argc, char *argv[]) {
    FILE *fp = NULL;

    //argc is always at least 4
    if (argc < 4) {return printError(1, fp);}
    //check that user input file exists.
    if (access(argv[1], F_OK) == -1) {return printError(1, fp);}

    fp = fopen(argv[1], "rb");
    if (!fp) {return printError(2, fp);}

    Image *im = ReadPPM(fp);
    if (!im) {return printError(4, fp);}

    //perform operation
    int op = operation(argc, argv, im, fp);
    //return 0 if operation was successful
    if (op == -1) {
        int written = writePPMfile(argv, im);
        destroy(im);
        if (written == -1) {return printError(8, fp);}
        fclose(fp);
        return 0;
    }
    
    destroy(im);
    return op;    
}

int operation(int argc, char *argv[], Image *im, FILE *fp) {

    //find out which operation the user wants to execute
    if (!strcmp(argv[3], "grayscale")) {
        //grayscale should have no extra arguments
        if (argc > 4) {return printError(6, fp);}
        grayscale(im);
        return -1;
    } else if (!strcmp(argv[3], "crop")) {
        // crop should have additional 4 args (2 sets of co-ordinates, top left and bottom right)
        if (argc != 8) {return printError(6, fp);}
        int x1 = atoi(argv[4]), y1 = atoi(argv[5]), x2 = atoi(argv[6]), y2 = atoi(argv[7]);
        if (x1 < 0 || y1 < 0 || x2 >= im->cols || y2 >= im->rows) {return printError(7, fp);}
        int cropRows = y2 - y1, cropCols = x2 - x1;
        //new image dimensions should make sense
        if ((cropRows < 0) || (cropCols < 0)) {return printError(7, fp);}
        return crop(im, x1, y1, x2, y2, fp);
    } else if (!strcmp(argv[3], "binarize")) {
        //binarize op should have 1 additional argument
        if (argc != 5) {return printError(6, fp);}
        if (!isdigit(*argv[4])) {return printError(7, fp);}
        int threshold = atoi(argv[4]);
        //check threshold is in correct range
        if ((threshold < 0) || (threshold > 255)) {return printError(7, fp);}
        binarize(im, threshold);
        return -1;
    } else if (!strcmp(argv[3], "transpose")) {
        //transpose should have no extra arguments
        if (argc > 4) {return printError(6, fp);}
        //reach here means all good, carry out transpose op
        if (transpose(im) == 8) {printError(8, fp);}
        return -1;
    } else if (!strcmp(argv[3], "gradient")) {
        //gradient should have no extra arguments
        if (argc > 4) {return printError(6, fp);}
        gradient(im);
        return -1;
    } else if (!strcmp(argv[3], "seam")) {
        //seam should have two extra arguments between 0 and 1, inclusive
        if (argc != 6) {return printError(6, fp);}
        //ensure user input is numeric
        if ((!isdigit(*argv[4])) || (!isdigit(*argv[5]))) {return printError(7, fp);}
        double scaleCol = atof(argv[4]), scaleRow = atof(argv[5]);
        if ((scaleCol > 1) || (scaleCol < 0) || (scaleRow > 1) || (scaleRow < 0)) {return printError(7, fp);}
        seam(im, scaleCol, scaleRow);
        return -1;
    }

    return printError(5, fp);
}

void grayscale(Image *im) {

    unsigned char intensity;
    for (int r = 0; r < im->rows; r++) {
        for (int c = 0; c < im->cols; c++) {
            intensity = 0.3*im->data[(r * im->cols) + c].r + 0.59*im->data[(r * im->cols) + c].g + 0.11*im->data[(r * im->cols) + c].b;
            //update pixel intensity
            im->data[(r * im->cols) + c].r = intensity;
            im->data[(r * im->cols) + c].g = intensity;
            im->data[(r * im->cols) + c].b = intensity;
        }
    }
}

void binarize(Image *im, int threshold) {

    //intensity will be either 0 or 255 for each pixel
    unsigned char intensity;
    for (int r = 0; r < im->rows; r++) {
        for (int c = 0; c < im->cols; c++) {
            intensity = 0.3*im->data[(r * im->cols) + c].r + 0.59*im->data[(r * im->cols) + c].g + 0.11*im->data[(r * im->cols) + c].b;
            if (intensity < threshold) {
                intensity = 0;
            } else {intensity = 255;}

            im->data[(r * im->cols) + c].r = intensity;
            im->data[(r * im->cols) + c].g = intensity;
            im->data[(r * im->cols) + c].b = intensity;
        }
    }

}

int crop(Image *im, int x1, int y1, int x2, int y2, FILE *fp) {
    //check if memory allocated successfully
    Pixel *cropPix = malloc(sizeof(Pixel) * (y2 - y1) * (x2 - x1));
    if (!cropPix) {return printError(8, fp);}

    //dimensions of new image
    int cropRows = y2 - y1;
    int cropCols = x2 - x1;

    for (int r = 0; r < cropRows; r++) {
        for (int c = 0; c <cropCols; c++) {
            cropPix[(r * cropCols) + c] = im->data[((y1 + r) * im->cols) + x1 + c];
        }
    }

    free(im->data);
    im->data = cropPix;
    im->rows = cropRows;
    im->cols = cropCols;
    return -1;
}

int transpose(Image *im) {

    Pixel *transposePix = malloc(sizeof(Pixel) * im->rows * im->cols);
    //check if memory allocated successfully
    if (!transposePix) {return 8;}
    
    //new(x,y) gets old(y,x)
    for (int r = 0; r < im->rows; r++) {
        for (int c = 0; c <im->cols; c++) {
            transposePix[(c * im->rows) + r] = im->data[(r * im->cols) + c];
        }
    }

    //update image
    free(im->data);
    im->data = transposePix;
    int tempRow = im->rows;
    im->rows = im->cols;
    im->cols = tempRow;
    return 0;
}

void gradient(Image *im) {
    
    grayscale(im);
    Pixel *gradPix = malloc(sizeof(Pixel) * im->rows * im->cols);
    int gradx;
    int grady;
    //grad is absolute sum of gradx and grady
    int grad;

    for (int r = 0; r < im->rows; r++) {
        for (int c = 0; c < im->cols; c++) {
            //boundary pixels get energy zero
            if (c == 0 || c == (im->cols - 1) || r == 0 || r == (im->rows - 1)) {
                grad = 0;
            } else {
                gradx = (im->data[(r * im->cols) + c + 1].r - im->data[(r * im->cols) + c - 1].r) / 2;
                grady = (im->data[(r * im->cols) + c + im->cols].r - im->data[(r * im->cols) + c - im->cols].r) / 2;
                grad = abs(gradx) + abs(grady);
            }
            gradPix[(r * im->cols) + c].r = grad;
            gradPix[(r * im->cols) + c].g = grad;
            gradPix[(r * im->cols) + c].b = grad;
        }
    }

    free(im->data);
    im->data = gradPix;
}

void seam(Image *im, float scaleCol, float scaleRow) {
    Image* copy = malloc(sizeof(Image));
    copyIm(im, copy);

    int numColRemove = im->cols * (1 - scaleCol);
    int numRowRemove = im->rows * (1 - scaleRow);

    //minimum output image size should be 2x2
    if (im->cols - numColRemove < 2) {
        numColRemove = im->cols - 2;
    }
    if (im->rows - numRowRemove < 2) {
        numRowRemove = im->rows - 2;
    }
    //remove columns
    for (int i = 0; i < numColRemove; i++) {
        gradient(im);
        seamCarve(im, copy);
    }
    
    transpose(im);
    transpose(copy);
    //now remove rows
    for (int j = 0; j < numRowRemove; j++) {
        gradient(im);
        seamCarve(im, copy);
    }
    transpose(im);
    
    destroy(copy);
}

void seamCarve(Image *im, Image *copy) {

    //for an image with c# columns, create array of size c#. Each element holds cumulative energy of each seam
    int *seamEnergy = calloc(im->cols, sizeof(int));
    //2D array. Each row of seamArr contains a seam. The seam is stored as the index of each pixel.
    int *seamArr = calloc((im->cols * im->rows), sizeof(int));
    int target;
    int a = 0;
    int b = 0;
    for (b = 0; b < im->cols; b++) {
        a = 0;
        seamArr[(b * im->rows)] = b;
        target = b + assignDIR(im, a, b);
        for (a = 1; a < im->rows; a++) {
            seamArr[(b * im->rows) + a] = target;
            seamEnergy[b] += (int) im->data[(a * im->cols) + target].r;
            target += assignDIR(im, a, target);
        }
    }

    //which seam has the lowest energy?
    int minEnergyIndex = 0;
    for (int j = 1; j < im->cols; j++) {
        if (seamEnergy[j] < seamEnergy[minEnergyIndex]) {minEnergyIndex = j;}
    }

    //now remove it!
    removeSeam(copy, seamArr, minEnergyIndex);

    //clean up memory
    free(im->data);
    copyIm(copy, im);

    free(seamArr);
    free(seamEnergy);
}


int helpAssign(int mid, int right, int left, char position) {

    switch (position)
    {
        //are we at left boundary?
        case 'l':
            if (mid <= right) {
                return 0;
            } else {return 1;}
        //are we at right boundary?
        case 'r':
            if (mid <= left) {
                return 0;
            } else {return -1;}
        //we are somewhere between
        default:
            if (right == left) {
                if (mid <= right) {
                    return 0;
                } else {return -1;}
            } else {
               if (left < right) {
                    if (mid <= left) {
                        return 0;
                    } else {return -1;}
                } else { //i.e. right < left
                    if (mid <= right) {
                        return 0;
                    } else {return 1;}
                }
            }

    }

    return 0;
}

int assignDIR(Image *im, int a, int b) {
    //mid, left and right are the target pixel's neighbors on the next row
    int mid = 0;
    int left = 0;
    int right = 0;
    if (a < im->rows - 1) {
        mid = im->data[((a + 1) * im->cols) + b].r;
        if (b > 1) {left = im->data[((a + 1) * im->cols) + b - 1].r;}
        if (b < im->cols - 2) {right = im->data[((a + 1) * im->cols) + b + 1].r;}
    }
    
    //are we at first row of image?
    if (a == 0) {
        if (b == 0) {
            return 1;
        } else if (b == im->cols - 1) {
            return -1;
        } else if (b == im->cols - 2) {
            return helpAssign(mid, right, left, 'r');
        } else if (b == 1) {
            return helpAssign(mid, right, left, 'l');
        } else {
            return helpAssign(mid, right, left, 'm');
        } 
    } else if (a > 0) {
        if (b == 1) {
            return helpAssign(mid, right, left, 'l');
        } else if (b == im->cols - 2) {
            return helpAssign(mid, right, left, 'r');
        } else {
            return helpAssign(mid, right, left, 'm');
        } 
    }
    return 0;
}

void removeSeam(Image *im, int *seamArr, int targetIndex) {

    targetIndex = im->rows * targetIndex;
    Pixel *newPix = malloc(sizeof(Pixel) * im->rows * (im->cols - 1));
    for (int r = 0; r < im->rows; r++) {
        for (int c = 0; c < im->cols - 1; c++) {
            //do not copy if current element is pixel that is to be removed
            if (c < seamArr[targetIndex]) {
                newPix[(r * (im->cols - 1)) + c] = im->data[(r * im->cols) + c];
            } else {
                newPix[(r * (im->cols - 1)) + c] = im->data[(r * im->cols) + c + 1];
            }
        }
            targetIndex += 1;
    }
    // output image has one less column
    im->cols -= 1;
    
    free(im->data);
    im->data = newPix;
    
}