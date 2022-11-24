/**
 * @file inferencing.cpp
 * @author Daniel Kim
 * @brief image classification inferencing
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 *
 */

#include "inferencing.h"

namespace ei_inferencing
{
    /**
     * @brief Crops an image. Optimized for 32-but MCUs.
     * Supports 8 and 16 pixel images
     * Assumes that the destination buffer is dword aligned
     *
     * @param source source image resolution
     * @param srcImage source image data
     * @param startX start position of the crop in the x direction
     * @param startY start position of the crop in the y direction
     * @param destination destination image resolution
     * @param dstImage destination image data
     * @param iBpp pixel depth
     */
    void ImageManipulation::cropImage(Resolution source, uint8_t *srcImage, int startX, int startY, Resolution destination, uint8_t *dstImage, int iBpp)
    {
        uint32_t *s32, *d32;
        int x, y;
        // Do we need to crop?
        if (startX < 0 || startX >= source.width || startY < 0 || startY >= source.height || (startX + destination.width) > source.width || (startY + destination.height) > source.height)
        {
            return;
        }
        if (iBpp != 8 && iBpp != 16)
        {
            return;
        }

        if (iBpp == 8)
        {
            uint8_t *s, *d;
            for (y = 0; y < destination.height; y++)
            {
                s = &srcImage[source.width * (y + startY) + startX];
                d = &dstImage[destination.width * y];
                x = 0;
                if ((intptr_t)s & 3 || (intptr_t)d & 3)
                {
                    for (; x < destination.width; x++)
                    {
                        *d++ = *s++; // have to do it byte by byte
                    }
                }
                else
                {
                    // move 4 bytes at a time if aligned or alignment not enforced
                    s32 = (uint32_t *)s;
                    d32 = (uint32_t *)d;
                    for (; x < destination.width - 3; x += 4)
                    {
                        *d32++ = *s32++;
                    }
                    // any remaining bytes?
                    s = (uint8_t *)s32;
                    d = (uint8_t *)d32;
                    for (; x < destination.width; x++)
                    {
                        *d++ = *s++;
                    }
                }
            }
        }
        else
        {
            uint16_t *s, *d;
            for (y = 0; y < destination.height; y++)
            {
                s = (uint16_t *)&srcImage[2 * source.width * (y + startY) + startX * 2];
                d = (uint16_t *)&dstImage[(source.height * y * 2)];
                x = 0;
                if ((intptr_t)s & 2 || (intptr_t)d & 2)
                {
                    for (; x < destination.width; x++)
                    {
                        *d++ = *s++; // have to do it 16 bits at a time
                    }
                }
                else
                {
                    // move 4 bytes at a time if aligned or alignment not enforced
                    s32 = (uint32_t *)s;
                    d32 = (uint32_t *)d;
                    for (; x < destination.width - 1; x += 2)
                    {
                        *d32++ = *s32++;
                    }
                    // any remaining bytes?
                    s = (uint16_t *)s32;
                    d = (uint16_t *)d32;
                    for (; x < destination.width; x++)
                    {
                        *d++ = *s++;
                    }
                }
            }
        }
    }

    /**
     * @brief Resizes an image. Optimized for 32-bit MCUs.
     * @details Assumes that the destination buffer is dword aligned
     * @details Can be used to resize the image smaller or larger
     * @details If resizing much smaller than 1/3 size, then a more robust algorithm should average all the pixels
     * @details This algorithm uses bilinear interpolation - average a 2x2 block of pixels to generate a new pixel
     * @details Optimized for 32-bit MCUs
     * @details Supports 8 and 16 pixel images
     * 
     * @param source source resolution
     * @param srcImage source image data
     * @param destination destination resolution
     * @param dstImage destination image data
     * @param iBpp 8 or 16 pixels
     */
    void ImageManipulation::resizeImage(Resolution source, uint8_t *srcImage, Resolution destination, uint8_t *dstImage, int iBpp)
    {
        uint32_t src_x_accum, src_y_accum;
        uint32_t x_frac, nx_frac, y_frac, ny_frac;
        int x, y, ty;

        // Image is 8 or 16 pixels?
        if (iBpp != 8 && iBpp != 16)
        {
            return;
        }
        src_y_accum = FRAC_VAL / 2; // start at 1/2 pixel in to account for integer downsampling which might miss pixels

        const uint32_t src_x_frac = (source.width * FRAC_VAL) / destination.width;
        const uint32_t src_y_frac = (source.height * FRAC_VAL) / destination.height;

        constexpr uint32_t r_mask = 0xf800f800;
        constexpr uint32_t g_mask = 0x07e007e0;
        constexpr uint32_t b_mask = 0x001f001f;

        uint8_t *s, *d;
        uint16_t *s16, *d16;
        uint32_t x_frac2, y_frac2;

        for (y = 0; y < destination.height; y++)
        {
            ty = src_y_accum >> FRAC_BITS; // src y
            y_frac = src_y_accum & FRAC_MASK;
            src_y_accum += src_y_frac;
            ny_frac = FRAC_VAL - y_frac;        // y fraction and 1.0 - y fraction
            y_frac2 = ny_frac | (y_frac << 16); // for M4/M4 SIMD
            s = &srcImage[ty * source.width];
            s16 = (uint16_t *)&srcImage[ty * source.width * 2];
            d = &dstImage[y * destination.width];
            d16 = (uint16_t *)&dstImage[y * destination.width * 2];
            src_x_accum = FRAC_VAL / 2; // start at 1/2 pixel in to account for integer downsampling which might miss pixels

            if (iBpp == 8)
            {
                for (x = 0; x < destination.width; x++)
                {
                    uint32_t tx, p00, p01, p10, p11;
                    tx = src_x_accum >> FRAC_BITS;
                    x_frac = src_x_accum & FRAC_MASK;
                    nx_frac = FRAC_VAL - x_frac; // x fraction and 1.0 - x fraction
                    x_frac2 = nx_frac | (x_frac << 16);
                    src_x_accum += src_x_frac;
                    p00 = s[tx];
                    p10 = s[tx + 1];
                    p01 = s[tx + source.width];
                    p11 = s[tx + source.width + 1];
                #ifdef __ARM_FEATURE_SIMD32
                    p00 = __SMLAD(p00 | (p10 << 16), x_frac2, FRAC_VAL / 2) >> FRAC_BITS; // top line
                    p01 = __SMLAD(p01 | (p11 << 16), x_frac2, FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                    p00 = __SMLAD(p00 | (p01 << 16), y_frac2, FRAC_VAL / 2) >> FRAC_BITS; // combine
                #else                                                                                     // generic C code
                    p00 = ((p00 * nx_frac) + (p10 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // top line
                    p01 = ((p01 * nx_frac) + (p11 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                    p00 = ((p00 * ny_frac) + (p01 * y_frac) + FRAC_VAL / 2) >> FRAC_BITS; // combine top + bottom
                #endif                                                                                    // Cortex-M4/M7

                    *d++ = (uint8_t)p00;
                }
            }
            else
            {
                for (x = 0; x < destination.width; x++)
                {
                    uint32_t tx, p00, p01, p10, p11;
                    uint32_t r00, r01, r10, r11, g00, g01, g10, g11, b00, b01, b10, b11;
                    tx = src_x_accum >> FRAC_BITS;
                    x_frac = src_x_accum & FRAC_MASK;
                    nx_frac = FRAC_VAL - x_frac; // x fraction and 1.0 - x fraction
                    x_frac2 = nx_frac | (x_frac << 16);
                    src_x_accum += src_x_frac;
                    p00 = __builtin_bswap16(s16[tx]);
                    p10 = __builtin_bswap16(s16[tx + 1]);
                    p01 = __builtin_bswap16(s16[tx + source.width]);
                    p11 = __builtin_bswap16(s16[tx + source.width + 1]);
                #ifdef __ARM_FEATURE_SIMD32
                    {
                        p00 |= (p10 << 16);
                        p01 |= (p11 << 16);
                        r00 = (p00 & r_mask) >> 1;
                        g00 = p00 & g_mask;
                        b00 = p00 & b_mask;
                        r01 = (p01 & r_mask) >> 1;
                        g01 = p01 & g_mask;
                        b01 = p01 & b_mask;
                        r00 = __SMLAD(r00, x_frac2, FRAC_VAL / 2) >> FRAC_BITS;               // top line
                        r01 = __SMLAD(r01, x_frac2, FRAC_VAL / 2) >> FRAC_BITS;               // bottom line
                        r00 = __SMLAD(r00 | (r01 << 16), y_frac2, FRAC_VAL / 2) >> FRAC_BITS; // combine
                        g00 = __SMLAD(g00, x_frac2, FRAC_VAL / 2) >> FRAC_BITS;               // top line
                        g01 = __SMLAD(g01, x_frac2, FRAC_VAL / 2) >> FRAC_BITS;               // bottom line
                        g00 = __SMLAD(g00 | (g01 << 16), y_frac2, FRAC_VAL / 2) >> FRAC_BITS; // combine
                        b00 = __SMLAD(b00, x_frac2, FRAC_VAL / 2) >> FRAC_BITS;               // top line
                        b01 = __SMLAD(b01, x_frac2, FRAC_VAL / 2) >> FRAC_BITS;               // bottom line
                        b00 = __SMLAD(b00 | (b01 << 16), y_frac2, FRAC_VAL / 2) >> FRAC_BITS; // combine
                    }
                    #else  // generic C code
                    {
                        r00 = (p00 & r_mask) >> 1;
                        g00 = p00 & g_mask;
                        b00 = p00 & b_mask;
                        r10 = (p10 & r_mask) >> 1;
                        g10 = p10 & g_mask;
                        b10 = p10 & b_mask;
                        r01 = (p01 & r_mask) >> 1;
                        g01 = p01 & g_mask;
                        b01 = p01 & b_mask;
                        r11 = (p11 & r_mask) >> 1;
                        g11 = p11 & g_mask;
                        b11 = p11 & b_mask;
                        r00 = ((r00 * nx_frac) + (r10 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // top line
                        r01 = ((r01 * nx_frac) + (r11 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                        r00 = ((r00 * ny_frac) + (r01 * y_frac) + FRAC_VAL / 2) >> FRAC_BITS; // combine top + bottom
                        g00 = ((g00 * nx_frac) + (g10 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // top line
                        g01 = ((g01 * nx_frac) + (g11 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                        g00 = ((g00 * ny_frac) + (g01 * y_frac) + FRAC_VAL / 2) >> FRAC_BITS; // combine top + bottom
                        b00 = ((b00 * nx_frac) + (b10 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // top line
                        b01 = ((b01 * nx_frac) + (b11 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                        b00 = ((b00 * ny_frac) + (b01 * y_frac) + FRAC_VAL / 2) >> FRAC_BITS; // combine top + bottom
                    }
                    #endif // Cortex-M4/M7
                    r00 = (r00 << 1) & r_mask;
                    g00 = g00 & g_mask;
                    b00 = b00 & b_mask;
                    p00 = (r00 | g00 | b00);                   // re-combine color components
                    *d16++ = (uint16_t)__builtin_bswap16(p00); // store new pixel
                }                                              // for x
            }
        }
    }

    bool ImageManipulation::jpeg_to_rgb888(const uint8_t *srcImage, std::size_t src_len, uint8_t &dstImage)
    {


    }

    /**
     * @brief Determine wheter to resize and to which dimensions
     *
     * @param out_resolution width and height of the output image
     * @param resize_col_sz pointer to frame buffer's column/width value
     * @param resize_row_sz pointer to frame buffer's row/height value
     * @param do_resize returns wheter to resize or not
     */
    int Inferencing::calculate_resize_dimensions(Resolution out_resolution, Resolution &resize_resolution, bool &do_resize)
    {
        constexpr std::size_t list_size = 2;
        const Resolution list[list_size] = {{42, 32}, {128, 96}};

        //(default) conditions
        resize_resolution.width = RAW_FRAME_BUFFER_COLS;
        resize_resolution.height = RAW_FRAME_BUFFER_ROWS;
        do_resize = false;

        for (std::size_t ix = 0; ix < list_size; ix++)
        {
            if ((out_resolution.width <= list[ix].width) && (out_resolution.height <= list[ix].height))
            {
                resize_resolution.width = list[ix].width;
                resize_resolution.height = list[ix].height;
                do_resize = true;
                break;
            }
        }

        return 0;
    }

    bool Inferencing::capture(Resolution resolution, uint8_t *img_data)
    {
       bool do_resize = false;

       
    }

    /**
     * @brief Convert RGB56 to RGB8888
     * 
     * @param offset pixel offset of raw buffer
     * @param length number of pixels to convert
     * @param out_ptr pointer to store output image
     */
    int Inferencing::ei_camera_cutout_get_data(std::size_t offset, std::size_t length, float *out_ptr)
    {
        std::size_t pixel_ix = offset * 2; 
        std::size_t bytes_left = length;
        std::size_t out_ptr_ix = 0;

        // read byte for byte
        while (bytes_left != 0) {
            // grab the value and convert to r/g/b
            uint16_t pixel = (m_ei_camera_capture_out[pixel_ix] << 8) | m_ei_camera_capture_out[pixel_ix+1];
            uint8_t r, g, b;
            r = ((pixel >> 11) & 0x1f) << 3;
            g = ((pixel >> 5) & 0x3f) << 2;
            b = (pixel & 0x1f) << 3;

            // then convert to out_ptr format
            float pixel_f = (r << 16) + (g << 8) + b;
            out_ptr[out_ptr_ix] = pixel_f;

            // and go to the next pixel
            out_ptr_ix++;
            pixel_ix+=2;
            bytes_left--;
        }

        // and done!
        return 0;
    }

};