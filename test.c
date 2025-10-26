#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define WINDOW_DIMS 512.0f
#define SRC_COUNT 13u

int main(int argc, char **argv) {
    
    
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_CreateWindowAndRenderer("test", WINDOW_DIMS, WINDOW_DIMS, 0, &window, &renderer);
    SDL_SetRenderDrawColorFloat(renderer, 0.5f, 0.5f, 0.5f, 1.0f);

    SDL_Texture *texture = IMG_LoadTexture(renderer, "custom_sprite_sheet.png");   

    if (texture) {
        float texture_width, texture_height;
        SDL_GetTextureSize(texture, &texture_width, &texture_height);
        float src_width = texture_width / 4.0f;
        float src_height = texture_width / 4.0f;

        SDL_FRect const src_rects[SRC_COUNT] = {
            {                                                                              },
            {.x = 0.0f            , .y = src_height * 0.0f, .w = src_width, .h = src_height},
            {.x = src_width       , .y = src_height * 0.0f, .w = src_width, .h = src_height},
            {.x = src_width * 2.0 , .y = src_height * 0.0f, .w = src_width, .h = src_height},
            {.x = src_width * 3.0 , .y = src_height * 0.0f, .w = src_width, .h = src_height},
            {.x = 0.0f            , .y = src_height * 1.0f, .w = src_width, .h = src_height},
            {.x = src_width       , .y = src_height * 1.0f, .w = src_width, .h = src_height},
            {.x = 0.0f            , .y = src_height * 2.0f, .w = src_width, .h = src_height},
            {.x = src_width       , .y = src_height * 2.0f, .w = src_width, .h = src_height},
            {.x = src_width * 2.0 , .y = src_height * 2.0f, .w = src_width, .h = src_height},
            {.x = src_width * 3.0 , .y = src_height * 2.0f, .w = src_width, .h = src_height},
            {.x = 0.0f            , .y = src_height * 3.0f, .w = src_width, .h = src_height},
            {.x = src_width, .y = src_height * 3.0f, .w = src_width, .h = src_height},
        };

        bool quit = false;
        bool is_big = false;
        unsigned current = 0;
        bool show_all = false;

        while (!quit) {
            SDL_RenderClear(renderer);
            SDL_RenderTexture(
                renderer,
                texture, 
                show_all ? (SDL_FRect const *)0 : &src_rects[current],
                &(SDL_FRect){WINDOW_DIMS / 4.0f, WINDOW_DIMS / 4.0f, WINDOW_DIMS / 2.0f, WINDOW_DIMS / 2.0f}
            );
            SDL_RenderPresent(renderer);

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    SDL_Log("Mouse up: %f, %f\n", e.button.x, e.button.y);
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        if (!show_all) {
                            current = (current + 1) % SRC_COUNT;
                        }
                    } else if (e.button.button = SDL_BUTTON_RIGHT) {
                        show_all = !show_all;
                    }
                    break;
                }
            }
        }
    } else {
        SDL_Log("Could not load image. SDL_Error: %s", SDL_GetError());
    }

    SDL_DestroyWindow(window);  // Destroys renderer as part of impl, which in turn destroys textures
}