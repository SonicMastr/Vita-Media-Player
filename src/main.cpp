#include <kernel.h>
#include <stdio.h>
#include <gfx.hpp>
#include <player.hpp>
#include <menu.hpp>
#include <subtitles.hpp>

unsigned int sceLibcHeapSize = 40*1024*1024;

using namespace std;
using namespace vmp::gfx;
using namespace vmp::player;
using namespace vmp::decoder;
using namespace vmp::subs;
using namespace vmp::menu;

int main(void)
{
    sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DEFAULT); // Remember this shit. Please. Jesus Please
    MenuManager::getInstance();
        // int dead = 0;
        // Graphics *graphics = Graphics::getInstance();
        // Player::getInstance()->createDecoder("ux0:/Toaru Kagaku No Railgun T - 01.mp4");
        // SubtitleFactory *subs = new SubtitleFactory("ux0:/Toaru Kagaku No Railgun T - 01.mp4", true, false);
        // Decoder *decoder = Player::getInstance()->getDecoder();
        // vita2d_pvf *font = graphics->loadFontFromFile("app0:OpenSans-Bold.ttf", (graphics->getScreenHeight()*0.0290f), (graphics->getScreenHeight()*0.0290f));
        // DecoderVideoData data;

        // vita2d_texture *texture = NULL;

        // while (decoder->getStatus() == DECODER_LOADING)
        // {
        //     sceKernelDelayThread(1000);
        // }

        // if (decoder->getStatus() == DECODER_READY) {
        //     if(decoder->start())
        //         dead = 1;
        // } else {
        //     dead = 1;
        // }

        // SubtitleParser *parser = subs->getParser(SUBTITLE_SRT);

        // std::vector<SubtitleItem*> subItems = parser->getSubtitles();

        // SceInt32 frame = 0;
        // SceInt32 done = 0;
        // for(;;)
        // {
        //     graphics->beginFrame();
        //     if (!dead)
        //     {
        //         SubtitleItem* item = NULL;
        //         float scale, startX;
        //         scale = graphics->getScreenHeight()/(float)data.height;
        //         startX = -((((float)data.width*scale)-graphics->getScreenWidth())/2);
        //         if(decoder->getVideoData(&data))
        //         {
        //             if (texture == NULL)
        //                 texture = graphics->createTexture(data.width, data.height, SCE_GXM_TEXTURE_FORMAT_YVU420P2_CSC1, data.frameData);
        //             else
        //                 graphics->updateTexture(texture, data.frameData);
        //         }
        //         item = getCurrentSub(subItems, decoder->getCurrentTime());
        //         if (texture != NULL)
        //             graphics->drawTexture(texture, startX, 0, scale, scale);
        //         if (decoder->getSubtitleData()[0] != '\0') {
        //             graphics->drawText(font, 20, 400, RGBA8(0, 100, 255, 255), decoder->getSubtitleData(), 1.0f);
        //         }
        //     }
        //     graphics->endFrame();
        //     if (frame >= 600 && !done) { // ~10 seconds
        //         // Test Changing Resolution
        //         graphics->setResolution(960, 544);
        //         // Next, reload the font as we need to change the Glyph size
        //         font = graphics->loadFontFromFile("app0:OpenSans-Bold.ttf", (graphics->getScreenHeight()*0.0290f), (graphics->getScreenHeight()*0.0290f));
        //         done++;
        //     }
        //     frame++;
        // }

    sceKernelExitProcess(0);
	return 0;
}
