#include <gfx.hpp>

using namespace vmp::gfx;
using namespace vmp::decoder;

extern SceUChar8 _binary_resources_play_png_start;
extern SceUChar8 _binary_resources_pause_png_start;
extern SceUChar8 _binary_resources_stop_png_start;
extern SceUChar8 _binary_resources_backwards_png_start;
extern SceUChar8 _binary_resources_forwards_png_start;
extern SceUChar8 _binary_resources_subtitles_png_start;
extern SceUChar8 _binary_resources_subtitlesd_png_start;
extern SceUChar8 _binary_resources_video_png_start;
extern SceUChar8 _binary_resources_music_png_start;
extern SceUChar8 _binary_resources_folder_png_start;
extern SceUChar8 _binary_resources_loading_png_start;
extern SceUChar8 _binary_resources_play_png_size;
extern SceUChar8 _binary_resources_pause_png_size;
extern SceUChar8 _binary_resources_stop_png_size;
extern SceUChar8 _binary_resources_backwards_png_size;
extern SceUChar8 _binary_resources_forwards_png_size;
extern SceUChar8 _binary_resources_subtitles_png_size;
extern SceUChar8 _binary_resources_subtitlesd_png_size;
extern SceUChar8 _binary_resources_video_png_size;
extern SceUChar8 _binary_resources_music_png_size;
extern SceUChar8 _binary_resources_folder_png_size;
extern SceUChar8 _binary_resources_loading_png_size;

static SceFloat32 _rad = 0;

static SceInt32 tex_format_to_bytespp(SceGxmTextureFormat format) // Borrowing from Vita2d
{
	switch (format & 0x9f000000U) {
	case SCE_GXM_TEXTURE_BASE_FORMAT_U8:
	case SCE_GXM_TEXTURE_BASE_FORMAT_S8:
	case SCE_GXM_TEXTURE_BASE_FORMAT_P8:
		return 1;
	case SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4:
	case SCE_GXM_TEXTURE_BASE_FORMAT_U8U3U3U2:
	case SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5:
	case SCE_GXM_TEXTURE_BASE_FORMAT_U5U6U5:
	case SCE_GXM_TEXTURE_BASE_FORMAT_S5S5U6:
	case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8:
	case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8:
		return 2;
	case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8:
	case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8:
		return 3;
	case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8:
	case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8:
	case SCE_GXM_TEXTURE_BASE_FORMAT_F32:
	case SCE_GXM_TEXTURE_BASE_FORMAT_U32:
	case SCE_GXM_TEXTURE_BASE_FORMAT_S32:
	default:
		return 4;
	}
}

static vita2d_texture *Texture_LoadImageBilinear(ScePVoid buffer, unsigned long size)
{
	vita2d_texture *texture = vita2d_load_PNG_buffer(buffer, size);
	return texture;
}

void Graphics::loadTextures(void)
{
	_play = Texture_LoadImageBilinear(&_binary_resources_play_png_start, (unsigned long)&_binary_resources_play_png_size);
    _pause = Texture_LoadImageBilinear(&_binary_resources_pause_png_start, (unsigned long)&_binary_resources_pause_png_size);
    _stop = Texture_LoadImageBilinear(&_binary_resources_stop_png_start, (unsigned long)&_binary_resources_stop_png_size);
    _backwards = Texture_LoadImageBilinear(&_binary_resources_backwards_png_start, (unsigned long)&_binary_resources_backwards_png_size);
    _forwards = Texture_LoadImageBilinear(&_binary_resources_forwards_png_start, (unsigned long)&_binary_resources_forwards_png_size);
    _subtitles = Texture_LoadImageBilinear(&_binary_resources_subtitles_png_start, (unsigned long)&_binary_resources_subtitles_png_size);
    _subtitles_disabled = Texture_LoadImageBilinear(&_binary_resources_subtitlesd_png_start, (unsigned long)&_binary_resources_subtitlesd_png_size);
    _video = Texture_LoadImageBilinear(&_binary_resources_video_png_start, (unsigned long)&_binary_resources_video_png_size);
    _music = Texture_LoadImageBilinear(&_binary_resources_music_png_start, (unsigned long)&_binary_resources_music_png_size);
    _folder = Texture_LoadImageBilinear(&_binary_resources_folder_png_start, (unsigned long)&_binary_resources_folder_png_size);
    _loading = Texture_LoadImageBilinear(&_binary_resources_loading_png_start, (unsigned long)&_binary_resources_loading_png_size);
}

SceUInt32 Graphics::abgr2rgba(SceUInt abgr)
{
    return (((abgr & 0xFF000000) >> 24) | ((abgr & 0x00FF0000) >>  8) | ((abgr & 0x0000FF00) <<  8) | ((abgr & 0x000000FF) << 24));
}

void Graphics::drawStatus(DecoderState state)
{
    float scale = (_width*.05f)/200.0f;
    switch (state) {
        case DECODER_PLAYING:
            vita2d_draw_texture_scale_rotate(_play, _width/2, _height*.9f, scale, scale, 0.0f);
            break;
        case DECODER_PAUSED:
            vita2d_draw_texture_scale_rotate(_pause, _width/2, _height*.9f, scale, scale, 0.0f);
            break;
        case DECODER_STOPPED:
            vita2d_draw_texture_scale_rotate(_stop, _width/2, _height*.9f, scale, scale, 0.0f);
            break;
        case DECODER_BUFFERING:
            vita2d_draw_texture_scale_rotate(_loading, _width/2, _height*.9f, scale, scale, 0.0f);
            break;
        default:
            break;
    }
}

void Graphics::drawOverlay(SceUInt64 currentTime, SceUInt64 streamDuration)
{
    float percentageDone = currentTime/(float)streamDuration;
    vita2d_draw_rectangle(0,0,_width,_height,RGBA8(0,0,0,120));
    vita2d_draw_rectangle(_width*.12f,_height*.8f,_width*.76f, _height*.027f, RGBA8(255,255,255,255));
    vita2d_draw_rectangle(_width*.12f,_height*.8f,(_width*.76f)*percentageDone,_height*.027f, RGBA8(120, 120, 255, 200));
}

void Graphics::drawLoading(void)
{
	SceFloat32 scale = (_width*.15f)/200.0f;
	vita2d_draw_texture_scale_rotate(_loading, (_width/2.0f), (_height/2.0f), scale, scale, _rad);
    _rad += 0.1f;
}

vita2d_pvf* Graphics::loadFontFromFile(std::string fileName, SceFloat32 height, SceFloat32 width)
{
    return vita2d_load_custom_pvf(fileName.c_str(), height, width);
}

vita2d_texture* Graphics::createTexture(SceInt32 width, SceInt32 height, SceGxmTextureFormat texFormat, const void* texData)
{
    vita2d_texture *texture = vita2d_create_empty_texture_format(width, height, texFormat);
    vita2d_texture_set_filters(texture, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
    if (texData != NULL) 
        updateTexture(texture, texData);
    return texture;
}

void Graphics::drawDefaultText(SceInt32 x, SceInt32 y, SceUInt32 color, std::string text)
{
    vita2d_pgf_draw_text(_default, x, y, color, (_height/544.0f), text.c_str());
}

void Graphics::drawText(vita2d_pvf *font, SceInt32 x, SceInt32 y, SceUInt32 color, std::string text, SceFloat32 fontSize)
{
    vita2d_pvf_draw_text(font, x, y, color, fontSize, text.c_str());
}

void Graphics::drawTexture(vita2d_texture *texture, SceFloat32 x, SceFloat32 y, SceFloat32 scaleX, SceFloat32 scaleY, SceFloat32 rad)
{
    vita2d_texture_set_filters(texture, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
    vita2d_draw_texture_scale(texture, x, y, scaleX, scaleY);
}

void Graphics::drawRectangle(SceFloat32 x, SceFloat32 y, SceFloat32 width, SceFloat32 height, SceUInt32 color)
{
    vita2d_draw_rectangle(x, y, width, height, color);
}

void Graphics::drawCircle(SceFloat32 x, SceFloat32 y, SceFloat32 radius, SceUInt32 color)
{
    vita2d_draw_fill_circle(x, y, radius, color);
}

void Graphics::beginFrame(void)
{
    vita2d_start_drawing();
	vita2d_clear_screen();
}

void Graphics::endFrame(void)
{
    vita2d_end_drawing();
    vita2d_wait_rendering_done();
	vita2d_end_shfb();
}

void Graphics::setResolution(SceInt32 width, SceInt32 height)
{
    _width = width;
    _height = height;
    vita2d_display_set_resolution(width, height);
}

void Graphics::updateTexture(vita2d_texture *vita2dTex, const void* texData)
{
    sceGxmTextureInitLinear(&vita2dTex->gxm_tex, texData, vita2d_texture_get_format(vita2dTex), vita2d_texture_get_width(vita2dTex), vita2d_texture_get_height(vita2dTex), 0);
}

Graphics::Graphics(SceInt32 width, SceInt32 height)
{
    _width = width;
    _height = height;
    sceKernelLoadStartModule("app0:modules/libvita2d_sys.suprx", 0, SCE_NULL, 0, SCE_NULL, SCE_NULL);
    vita2d_init_param params;
    vita2d_display_set_max_resolution(1920, 1088);
    memset(&params, 0, sizeof(vita2d_init_param));
    vita2d_init(&params);
    setResolution(width, height);
    vita2d_set_clear_color(RGBA8(0, 0, 0, 255));
    vita2d_set_vblank_wait(1);
    loadTextures();

    vita2d_system_pgf_config configs[] = {
		{SCE_FONT_LANGUAGE_LATIN, NULL},
	};
    _default = vita2d_load_system_pgf(1, configs);
}

Graphics::~Graphics(void)
{
    vita2d_fini();
}
