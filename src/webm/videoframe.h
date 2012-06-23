#ifndef EPYX_WEBM_VIDEOFRAME
#define EPYX_WEBM_VIDEOFRAME

#include <string>
#include <vpx/vpx_image.h>
#include <SDL/SDL.h>

namespace Epyx {

namespace webm {

    class VideoFrame {
    public:
        VideoFrame(int width, int height, const std::string& name);
        ~VideoFrame();

        bool init();
        void showFrame(vpx_image_t* image);

    private:
        int width, height;
        std::string window_name;
        SDL_Surface* screen;
        SDL_Overlay* overlay;
    };

}
}

#endif //EPYX_WEBM_VIDEOFRAME
