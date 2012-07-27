/**
 * @file webm/vpx_encode.h
 * @brief Abstraction layer to vpx codec encoder
 */

#ifndef EPYX_WEBM_VPX_ENCODE_H
#define EPYX_WEBM_VPX_ENCODE_H

#include <vpx/vpx_encoder.h>
#include "framepacket.h"

namespace Epyx
{
    namespace webm
    {
        class VpxEncoder
        {
        public:
            VpxEncoder(unsigned int display_width, unsigned int display_height,
                    unsigned int video_bitrate = 400);
            ~VpxEncoder();

            /**
             * @brief Encode a frame
             * @param raw
             * @param time_ms
             * @param flags 0, //   NORMAL,
             *   VPX_EFLAG_FORCE_KF, //   KEY,
             *   VP8_EFLAG_FORCE_GF | VP8_EFLAG_NO_UPD_ARF |
             *   VP8_EFLAG_NO_REF_LAST | VP8_EFLAG_NO_REF_ARF, //   GOLD = 2,
             *   VP8_EFLAG_FORCE_ARF | VP8_EFLAG_NO_UPD_GF |
             *   VP8_EFLAG_NO_REF_LAST | VP8_EFLAG_NO_REF_GF  //   ALTREF = 3
             */
            bool encode(const vpx_image_t& raw, unsigned long time_ms, unsigned int flags);

            /**
             * @brief Iterate over packets, until end (and return NULL)
             * @return 
             */
            FramePacket* getPacket();

        private:
            vpx_codec_ctx_t encoder;
            vpx_codec_iter_t iter;

            unsigned int display_width;
            unsigned int display_height;
            unsigned int video_bitrate;
        };
    }
}

#endif /* EPYX_WEBM_VPX_ENCODE_H */

