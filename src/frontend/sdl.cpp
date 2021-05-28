#include "frontend.h"
#ifdef SDL_FRONTEND
#include <SDL.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *font_texture;

Frontend::Frontend()
{
#ifdef __WIN32
    //On Vista or newer windows, let the OS know we are DPI aware, so we won't have odd scaling issues.
    if (void* user_dll = SDL_LoadObject("USER32.DLL"))
    {
        if (auto SetProcessDPIAware = reinterpret_cast<int(__stdcall *)(void)>(SDL_LoadFunction(user_dll, "SetProcessDPIAware")))
            SetProcessDPIAware();
    }
#endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer);
    auto font_surface = SDL_LoadBMP("resource/font.bmp");
    SDL_SetColorKey(font_surface, 1, 0);
    font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
    SDL_FreeSurface(font_surface);
}

uint32_t Frontend::getInput()
{
    while(1)
    {
        SDL_Event event;
        SDL_WaitEvent(&event);
        if (event.type == SDL_TEXTINPUT && strlen(event.text.text) == 1)
            return event.text.text[0];
        if (event.type == SDL_QUIT)
            return INPUT_QUIT;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            return 0;
    }
}

Vector2i Frontend::beginDrawing()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    scale = 1;
    Vector2i size;
    while(1)
    {
        size.x = ((w + 7) / 8 + scale - 1) / scale;
        size.y = ((h + 7) / 8 + scale - 1) / scale;
        if (size.x < 60 || size.y < 60)
            break;
        scale += 1;
    }

    buffer.resize(size);
    return size;
}

void Frontend::draw(int x, int y, char c, Color forground_color, Color background_color)
{
    if (x < 0 || y < 0) return;
    if (x >= buffer.size().x || y >= buffer.size().y) return;

    buffer(x, y) = Tile{c, forground_color, background_color};
}

void Frontend::present()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int xoffset = (w - buffer.size().x * scale * 8) / 2;
    int yoffset = (h - buffer.size().y * scale * 8) / 2;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    auto size = buffer.size();
    for(int y=0; y<size.y; y++)
    {
        for(int x=0; x<size.x; x++)
        {
            Tile t = buffer(x, y);
            SDL_SetTextureColorMod(font_texture, t.forground_color.r, t.forground_color.g, t.forground_color.b);
            SDL_Rect src{(t.c & 0x0F) * 8, ((t.c >> 4) & 0x0F) * 8, 8, 8};
            SDL_Rect dst{x * 8 * scale + xoffset, y * 8 * scale + yoffset, 8 * scale, 8 * scale};
            SDL_SetRenderDrawColor(renderer, t.background_color.r, t.background_color.g, t.background_color.b, 0xFF);
            SDL_RenderFillRect(renderer, &dst);
            SDL_RenderCopy(renderer, font_texture, &src, &dst);
        }
    }
    SDL_RenderPresent(renderer);
}

#endif
