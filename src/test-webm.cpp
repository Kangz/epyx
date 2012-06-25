#include "api.h"
#include "webm/videodev.h"
#include "webm/vpxdecoder.h"
#include "webm/vpxencoder.h"
#include "webm/videoframe.h"

int main() {
    try {
        unsigned int width = 640, height = 480, frame_rate = 30, video_bitrate = 400;
        Epyx::API epyx;
        Epyx::webm::VideoDev vdev(width, height, frame_rate);
        Epyx::webm::VpxEncoder encoder(width, height, video_bitrate);
        Epyx::webm::VpxDecoder decoder;

        vpx_image_t rawImage;
        vpx_img_alloc(&rawImage, IMG_FMT_YV12, width, height, 1);
        EPYX_ASSERT(vdev.start_capture());

        Epyx::webm::FramePacket *fpkt;

        Epyx::webm::VideoFrame vframe(width, height, "Epyx");
        EPYX_ASSERT(vframe.init());

        while (!vframe.checkSDLQuitAndEvents()) {
            // Get frame
            if (vdev.get_frame(&rawImage)) {
                // Get time
                struct timeval tv;
                int gettimeofday_status = gettimeofday(&tv, NULL);
                EPYX_ASSERT(gettimeofday_status == 0);
                long int utime = tv.tv_sec * 1000 + tv.tv_usec;

                encoder.encode(rawImage, utime, 0);
                while ((fpkt = encoder.getPacket()) != NULL) {
                    Epyx::log::info << "Packet size " << fpkt->size << Epyx::log::endl;
                    decoder.decode(*fpkt);
                    delete fpkt;
                }

                vpx_image_t *img = decoder.getFrame();
                if (img != NULL) {
                    vframe.showFrame(img);
                }

                //vframe.showFrame(&rawImage);
            }
            usleep(10);
        }

        EPYX_ASSERT(vdev.stop_capture());
        vpx_img_free(&rawImage);
    } catch (Epyx::Exception e) {
        std::cout << e << std::endl;
    }
    return 0;
}
