#ifndef Camera_h
#define Camera_h

#include <esp_camera.h>

namespace Camera
{
    bool init();
    camera_fb_t capture();
    void returnFrameBuffer(camera_fb_t& fb);
};

















#endif