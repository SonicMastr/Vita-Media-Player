#ifndef GFX_HPP
#define GFX_HPP

#include <common.hpp>
#include <vita2d_sys.h>
#include <decoder.hpp>



namespace vmp { namespace gfx {

    class Graphics {
        private:
            vita2d_texture *_play, *_pause, *_stop, *_backwards, *_forwards, *_subtitles, *_subtitles_disabled, *_video, *_music, *_folder, *_loading;
            SceInt32 _width, _height;
            vita2d_pgf *_default;
            void loadTextures(void);
        public:
            /** Graphics Utils **/
            SceUInt32 abgr2rgba(SceUInt abgr);
            vita2d_pvf *loadFontFromFile(std::string fileName, SceFloat32 height = 1, SceFloat32 width = 1);
            vita2d_pvf *loadSystemFont(const vita2d_system_pvf_config *configs, SceFloat32 height, SceFloat32 width);
            SceInt32 getDefaultTextWidth(std::string text);
            SceInt32 getDefaultTextHeight(std::string text);
            SceInt32 getTextWidth(vita2d_pvf *font, std::string text, SceInt32 fontSize = 1);
            SceInt32 getTextHeight(vita2d_pvf *font, std::string text, SceInt32 fontSize = 1);
            SceInt32 getScreenWidth(void) const { return _width; };
            SceInt32 getScreenHeight(void) const { return _height; };

            /** Menu Specific Drawing **/
            void drawStatus(vmp::decoder::DecoderState state);
            void drawOverlay(SceUInt64 currentTime, SceUInt64 streamDuration);
            void drawLoading(void);
            void drawFileIcon(std::string extension);

            /** Normal Drawing Functions **/
            void beginFrame(void);
            void endFrame(void);
            vita2d_texture *createTexture(SceInt32 width, SceInt32 height, SceGxmTextureFormat texFormat = SCE_GXM_TEXTURE_FORMAT_A8B8G8R8, const void* texData = NULL);
            void drawDefaultText(SceInt32 x, SceInt32 y, SceUInt32 color, std::string text);
            void drawText(vita2d_pvf *font, SceInt32 x, SceInt32 y, SceUInt32 color, std::string text, SceFloat32 fontSize = 1.0f);
            void drawTexture(vita2d_texture *texture, SceFloat32 x, SceFloat32 y, SceFloat32 scaleX = 1.0f, SceFloat32 scaleY = 1.0f, SceFloat32 rad = 0.0f);
            void drawRectangle(SceFloat32 x, SceFloat32 y, SceFloat32 width, SceFloat32 height, SceUInt32 color);
            void drawCircle(SceFloat32 x, SceFloat32 y, SceFloat32 radius, SceUInt32 color);
            void setResolution(SceInt32 width, SceInt32 height);
            void updateTexture(vita2d_texture *vita2dTex, const void* texData);

            Graphics(SceInt32 width = 960, SceInt32 height = 544);
            ~Graphics(void);
    };

}};

#endif /* GFX_HPP */