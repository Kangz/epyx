/**
 * @file webm/videodev.h
 * @brief Acquire video from V4L device, /dev/video0
 */

#ifndef EPYX_WEBM_VIDEODEV_H
#define EPYX_WEBM_VIDEODEV_H

#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>
#include <linux/videodev2.h>

namespace Epyx
{
    namespace webm
    {
        class VideoDev
        {
        public:
            static const unsigned int FRAME_SIZE = 1280 * 720 * 3 / 2;
            static const unsigned int NB_BUFFER = 4;

            VideoDev(unsigned int display_width, unsigned int display_height,
                    unsigned int capture_frame_rate = 30, unsigned int video_bitrate = 400);

            ~VideoDev();

            void close();

            bool start_capture();

            bool stop_capture();

            bool get_frame(vpx_image_t *raw);

        private:
            unsigned int display_width;
            unsigned int display_height;
            unsigned int capture_frame_rate;
            unsigned int video_bitrate;

            // File descriptor
            int fd;

            const void* mem[NB_BUFFER];
            char frame[FRAME_SIZE];

            unsigned int count_captured_frames;
        };
    }
}

#endif /* EPYX_WEBM_VIDEODEV_H */

