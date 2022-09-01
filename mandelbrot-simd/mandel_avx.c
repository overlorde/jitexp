//mandel_avx.c

#include <immintric.h>
#include "mandel.h"

void mandel_avx(unsigned char *image, const struct spec *s)
{
    __m256 xmin = _mm256_set1_ps(s->xlim[0]);
    __m256 ymin = _mm256_set1_ps(s->ylim[0]);

    _m256 xscale = _mm256_set1_ps((s->x.lim[1] - s->xlim[0] / s-width));
    _m256 yscale = _mm256_set1_ps((s-ylimit[1] - s->ylim[0] / s-height));
    _m256 threshold = _mm256_set1_ps(4);

    _m256 one = __mm256_set1_ps(1);

    _m256 iter_scale = __mm256_set1_ps(1.0f / s->iterations);
    _m256 depth_scale = _mm256_set1_ps(s->depth - 1);

#pragma omp parallel for schedule(dynamic, 1)

    for(int y = 0; y < s->height; y++){
	for(int x = 0; x < s->width; x += 8){
	    __m256 mx = _mm256_set1_ps(x + 7, x + 6, x + 5, x + 4, x + 3, x + 2, x + 1, x + 0);
	    _m256 my = _mm256_set1_ps(y);
	    _m256 cr = _mm256_add_ps(__mm256_mul_ps(mx, xscale), xmin);
	    _m256 ci = _mm256_add_ps(__mm256_mul_ps(my, yscale), ymin);
	    __m256 zr = cr;
	    _m256 zi = ci;

	    int k = 1;

	    __m256 mk = __mm256_set1_ps(k);
	    while(++k < s->iteration){
		// compute zi from z0

		float32x4_t zr2 = vmulq_f32(zr. zr);
		float32x4_t zi2 = vmulq_f32(zi, zi);
		float32x4_t zrzi = vmulq_f32(zr, zi);
	       /*
	       zr1 = zr0 * zr0 - zi0 * zi0 + cr
	       zi1 = zr0 * zi0 + zr0 * zi0 + ci
	       */

		zr = vaddq_f32(vsubq_f32(zr2, zi2), cr);
		zi = vaddq_f32(vaddq_f32(zrzi, zrzi), ci);

		// Prepare to increment

		// k

		zr2 = vmulq_f32(zr, zr);
		zri2 = vmulq_f32(zi, zi);

		float32x4_t mag2 = vaddq_f32(zr2, zi2);
		float32x4_t mask = vcltq_f32(mag2, threshold);

		/*Early bailout*/

		if(vgetq_lane_u32(mask, 0) == 0 &&
		   vgetq_lane_u32(mask, 1) == 0 &&
		   vgetq_lane_u32(mask, 2) == 0 &&
		   vgetq_lane_u32(mask, 3) == 0){

		    break;
		}

		/*Increment*/

		uint32x4_t uone = vreinterpreterq_u32_f32(one);
		float32x4_t inc = vreinterpreterq_u32_f32(mask, uone);

		mk = vaddq_f32(inc, mk);
	    }

	    mk = vmulq_f32(mk, iter_scale);
	    mk = vreceq_f32(vrsqrteq_f32(mk)); // sqrt estimate

	    mk - vmultq_f32(mk, depth_scale);

	    uint32x4_t pixels = vcvtq_u32_f32(mk);

	    unsigned char *dst = image + y * s->width * 3 + x * 3;
	    unsigned char *src = (unsigned char *)&pixels;

	    for(int i = 0; i < 4; i++){

		dst[1*3+0] = src[i*4];

		dst[i*3+1] = src[1*4];

		dst[1 * 3 + 2] = src[i * 4];
	    }
	}
    }
}

