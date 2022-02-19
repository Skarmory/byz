#include "core/noise.h"

#include "core/log.h"

#include <math.h>

static void _random_gradient(int x, int y, float* out_x, float* out_y)
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;

    unsigned a = x;
    unsigned b = y;

    a *= 3284157443;
    b ^= a << s | a >> w - s;

    b *= 1911520717;
    a ^= b << s | b >> w - s;

    a *= 2048419325;

    float random = a * (3.14159265 / ~(~0u >> 1));

    *out_x = sin(random);
    *out_y = cos(random);
}

static float _interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

static float _perlin(float x, float y)
{
    // Get the for corners of the square that (x, y) lies in
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    //log_format_msg(LOG_DEBUG, "x0: %d", x0);
    //log_format_msg(LOG_DEBUG, "y0: %d", y0);
    //log_format_msg(LOG_DEBUG, "x1: %d", x1);
    //log_format_msg(LOG_DEBUG, "y1: %d", y1);
    //log_msg(LOG_DEBUG, " ");

    // Create pseudorandom gradient vectors from each corner
    float g_x0y0_x;
    float g_x0y0_y;
    _random_gradient(x0, y0, &g_x0y0_x, &g_x0y0_y);

    float g_x1y0_x;
    float g_x1y0_y;
    _random_gradient(x1, y0, &g_x1y0_x, &g_x1y0_y);

    float g_x0y1_x;
    float g_x0y1_y;
    _random_gradient(x0, y1, &g_x0y1_x, &g_x0y1_y);

    float g_x1y1_x;
    float g_x1y1_y;
    _random_gradient(x1, y1, &g_x1y1_x, &g_x1y1_y);

    //log_format_msg(LOG_DEBUG, "gradient x0y0: (%f, %f)", g_x0y0_x, g_x0y0_y);
    //log_format_msg(LOG_DEBUG, "gradient x1y0: (%f, %f)", g_x1y0_x, g_x1y0_y);
    //log_format_msg(LOG_DEBUG, "gradient x0y1: (%f, %f)", g_x0y1_x, g_x0y1_y);
    //log_format_msg(LOG_DEBUG, "gradient x1y1: (%f, %f)", g_x1y1_x, g_x1y1_y);
    //log_msg(LOG_DEBUG, " ");

    // Create distance vectors from corners to (x, y)
    float d_x0 = x - (float)x0; // This also is the x coordinate relative to the grid square
    float d_x1 = x - (float)x1;
    float d_y0 = y - (float)y0; // This also is the y coordinate relative to the grid square
    float d_y1 = y - (float)y1;

    //log_format_msg(LOG_DEBUG, "distance x - x0: %f", d_x0);
    //log_format_msg(LOG_DEBUG, "distance y - y0: %f", d_y0);
    //log_format_msg(LOG_DEBUG, "distance x - x1: %f", d_x1);
    //log_format_msg(LOG_DEBUG, "distance y - y1: %f", d_y1);
    //log_msg(LOG_DEBUG, " ");

    // Dot product the gradient and the distance
    float dotx0y0 = (d_x0 * g_x0y0_x) + (d_y0 * g_x0y0_y);
    float dotx1y0 = (d_x1 * g_x1y0_x) + (d_y0 * g_x1y0_y);
    float dotx0y1 = (d_x0 * g_x0y1_x) + (d_y1 * g_x0y1_y);
    float dotx1y1 = (d_x1 * g_x1y1_x) + (d_y1 * g_x1y1_y);

    // Interpolate between the values
    float x_interpolate_1 = _interpolate(dotx0y0, dotx1y0, d_x0);
    float x_interpolate_2 = _interpolate(dotx0y1, dotx1y1, d_x0);
    float y_interpolate   = _interpolate(x_interpolate_1, x_interpolate_2, d_y0);

    //log_format_msg(LOG_DEBUG, "%f", y_interpolate);

    return y_interpolate;
}

float perlin(float x, float y)
{
    return _perlin(x, y);
}
