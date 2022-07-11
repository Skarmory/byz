#ifndef BYZ_NOISE_H
#define BYZ_NOISE_H

float perlin(float x, float y);

/* Generate noise in range [0.0, 1.0] for given coordinate
 *
 *         x: x input coordinate
 *         y: y input coordinate
 * amplitude: Higher amplitude biases values towards the minimum and maximum
 * frequency: Higher frequency adds more noise, less smoothness
 *   octaves: Amount of iterations to do, lowering amplitude and increasing each iteration
 */
float perlin2(float x, float y, float amplitude, float frequency, int octaves);

#endif
