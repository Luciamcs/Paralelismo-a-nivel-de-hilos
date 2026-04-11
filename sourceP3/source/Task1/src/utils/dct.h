#ifndef __DCT__H__
#define __DCT__H__
#include "image.h"

namespace dct{
    void direct(float **dct, const Block<float> &matrix, int channel);
    void inverse(Block<float> &idctMatrix, float **dctMatrix, int channel, float min=0.0, float max=1.0);
    void normalize(float **DCTMatrix, int size);
    void assign(float **DCTMatrix, Block<float> &block, int channel);
    float **create_matrix(int x_size, int y_size);
    void delete_matrix(float **m);
}

#endif
