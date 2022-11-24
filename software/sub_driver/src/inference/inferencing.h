/**
 * @file inferencing.h
 * @author Daniel Kim
 * @brief image classification inferencing
 * @version 0.1
 * @date 2022-11-24
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */

#ifndef INFERENCING_H
#define INFERENCING_H

#include <OceanAI_inferencing.h>
#include <edge-impulse-sdk/dsp/image/image.hpp>

#include <cstddef>
#include <stdint.h>

#include "../core/debug.h"
#include "image.h"

constexpr int RAW_FRAME_BUFFER_COLS = 640;
constexpr int RAW_FRAME_BUFFER_ROWS = 480;
constexpr std::size_t RAW_FRAME_BYTES_SIZE = 2;

#define ALIGN_PTR(p, a) ((p & (a - 1))) ? (((uintptr_t)p + a) & ~(a - 1)) : (uintptr_t)p

#define FRAC_BITS 14
#define FRAC_VAL (1 << FRAC_BITS)
#define FRAC_MASK (FRAC_VAL - 1)

namespace ei_inferencing
{
    struct Resolution
    {
        int32_t width;
        int32_t height;
    };

    class ImageManipulation
    {
    public:
        ImageManipulation() = default;
        static void cropImage(Resolution source, uint8_t *srcImage, int startX, int startY, Resolution destination, uint8_t *dstImage, int iBpp);
        static void resizeImage(Resolution source, uint8_t *srcImage, Resolution destination, uint8_t *dstImage, int iBpp);
        static bool jpeg_to_rgb888(const uint8_t *srcImage, std::size_t src_len, uint8_t &dstImage);
    };

    class Inferencing : public ImageManipulation
    {
    public:
        Inferencing() : ImageManipulation() {}
        ~Inferencing();

        bool capture(Resolution resolution, uint8_t *img_data);
        int ei_camera_cutout_get_data(std::size_t offset, std::size_t length, float *out_ptr);
        static int calculate_resize_dimensions(Resolution out_resolution, Resolution &resize_resolution, bool &do_resize);

    private:
        uint8_t *m_ei_camera_capture_out = NULL;
        Resolution m_resize_sz;
        bool m_do_resize = false;
        bool m_do_crop = false;


    };
};




#endif