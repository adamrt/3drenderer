#include "array.h"
#include "display.h"
#include "swap.h"
#include "triangle.h"

// draw_filled_triangle uses the flat-top/flat-bottom method.
void draw_filled_triangle(int x0, int y0, float z0, float w0,
                          int x1, int y1, float z1, float w1,
                          int x2, int y2, float z2, float w2,
                          uint32_t color) {
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);

    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    // Do again in case y1 > y2 swapped them back
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }


    Vec4 point_a = { x0, y0, z0, w0 };
    Vec4 point_b = { x1, y1, z1, w1 };
    Vec4 point_c = { x2, y2, z2, w2 };

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) {
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    }
    if (y2 - y0 != 0) {
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }

    // flat-top render (bottom)
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) {
        inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    }
    if (y2 - y0 != 0) {
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

Vec3 barycentric_weights(Vec2 a, Vec2 b, Vec2 c, Vec2 p) {
    Vec2 ab = vec2_sub(b, a);
    Vec2 bc = vec2_sub(c, b);
    Vec2 ac = vec2_sub(c, a);
    Vec2 ap = vec2_sub(p, a);
    Vec2 bp = vec2_sub(p, b);

    float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;
    float gamma = 1 - alpha - beta;

    Vec3 weights = { alpha, beta, gamma };
    return weights;
}

void draw_texel(int x, int y,
                uint32_t *texture,
                Vec4 point_a, Vec4 point_b, Vec4 point_c,
                Tex2 a_uv, Tex2 b_uv, Tex2 c_uv) {

    Vec2 p = { x, y };
    Vec2 a = vec2_from_vec4(point_a);
    Vec2 b = vec2_from_vec4(point_b);
    Vec2 c = vec2_from_vec4(point_c);

    Vec3 weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // modulo is hacky fix here
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth is less than the existing one
    if (interpolated_reciprocal_w < z_buffer[(window_width * y) + x]) {
        draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
        z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
    }
}

void draw_triangle_pixel(int x, int y,
                         uint32_t color,
                         Vec4 point_a, Vec4 point_b, Vec4 point_c) {

    Vec2 p = { x, y };
    Vec2 a = vec2_from_vec4(point_a);
    Vec2 b = vec2_from_vec4(point_b);
    Vec2 c = vec2_from_vec4(point_c);

    Vec3 weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth is less than the existing one
    if (interpolated_reciprocal_w < z_buffer[(window_width * y) + x]) {
        draw_pixel(x, y, color);
        z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
    }
}

void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0,
                            int x1, int y1, float z1, float w1, float u1, float v1,
                            int x2, int y2, float z2, float w2, float u2, float v2,
                            uint32_t* texture) {
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);

    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    // Do again in case y1 > y2 swapped them back
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // Flip v to account for inverted UV (v goes downward)
    v0 = 1 - v0;
    v1 = 1 - v1;
    v2 = 1 - v2;

    Vec4 point_a = { x0, y0, z0, w0 };
    Vec4 point_b = { x1, y1, z1, w1 };
    Vec4 point_c = { x2, y2, z2, w2 };
    Tex2 a_uv = { u0, v0 };
    Tex2 b_uv = { u1, v1 };
    Tex2 c_uv = { u2, v2 };

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) {
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    }
    if (y2 - y0 != 0) {
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y,
                           texture,
                           point_a, point_b, point_c,
                           a_uv, b_uv, c_uv);
            }
        }
    }

    // flat-top render (bottom)
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) {
        inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    }
    if (y2 - y0 != 0) {
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y,
                           texture,
                           point_a, point_b, point_c,
                           a_uv, b_uv, c_uv);
            }
        }
    }
}
