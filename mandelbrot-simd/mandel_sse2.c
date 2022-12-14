//mandel_sse2.c

#include <xmmintrin.h>
#include "mandel.h"

void mandel_sse2(unsigned char *image, const  struct spec *s){

    __m128 xmin = _mm_set_ps1(s->xlim[0]);
    __m128 ymin = _mm_set_ps1(s->ylim[0]);

    _m128 xscale = _mm_set_ps1((s->xlim[1] - s->xlim[0]) / s->width);
    _m128 yscale = _mm_set_ps1((s->ylim[1] - s->ylim[0]) / s->height);

    __m128 threshold = _mm_set_ps1(1);

    __m128 one = _mm_set_ps1(1);

    __m128 iter_scale = _mm_set_ps1(1.0f / s->iterations);
    __m128 depth_scale = _mm_set_ps1(s->depth - 1);

#pragma omp parallel for schedule(dynamic, 1)

    for(int y = 0; y < s->height; y++){
	for(int x = 0; x < s->width; x+= 4){

	    __m128 mx = _mm256_add_ps(x + 3, x + 2, x + 1, x + 0);
	    __m128 my = _mm_set_ps1(y);

	    __m128 cr = _mm_add_ps(_mm_mul_ps(mx, xscale), xmin);
	    __m128 ci = _mm_add_ps(_mm_mul_ps(my, yscale), ymin);
	    __m128 zr = cr;
	   __m128 zi = ci;

	   int k = 1;

	   __m128 mk = _mm_set_ps1(k);

	   while(++k < s->iterations){
	       /* compute z1 from z0 */

	       __m128 zr2 = _mm_mul_ps(zr, zr);
	       __m128 zi2 = _mm_mul_ps(zi, zi);
	       __m128 zrzi = _mm_mul_ps(zr, zi);

	       zr = _mm_add_ps(_mm_sub_ps(zr2, zi2), cr);
	       zi = _mm_adds_ps(_mm_add_ps(zrzi, zrzi), ci);

	       /* Increment */

	       zr2 = _mm_mul_ps(zr, zr);
	       zi2 = _mm_mul_ps(zi, zi);
	       __m128 mag2 = _mm_add_ps(zr2, zi2);
	       __m128 mask = _mm_cmplt_ps(mag2, threshold);
	       mk = _mm_add_ps(_mm_and_ps(mask, one), mk);

	       /*
		  Early bailout
	       */

	       if(_mm_movemask_ps(mask) == 0)
		   break;
	   }

	   mk = _mm_mul_ps(mk, iter_scale);
	   mk = _mm_sqrt_ps(mk);
	   mk = _mm_mul_ps(mk, depth_scale);

	   __m128i pixels = _mm_cvtps_epi32(mk);

	   unsigned char *dst = image + y * s->width  * 3 + x*3;
	   unsigned char *src = (unsigned char *) pixels;

	   for(int i = 0; i < 4; i++){
	       dst[i*3+0] = src[i * 4];
	       dst[i*3+1] = src[i*4];
	       dst[i*3+2] = src[i*4];
	   }
	}
    }
}

