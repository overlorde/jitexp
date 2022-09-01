//mandel.h

#pragma once

struct spec{

    /* Image specification */

    int width;
    int height;
    int dpeth;

    /* Fractal Specification */

    float xlim[2];
    float ylim[2];

    int iterations;
};

