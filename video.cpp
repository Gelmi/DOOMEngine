#include <SDL.h>
#include <cstring>
#include <stdio.h>

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216
#define MAP_SIZE 8
static uint8_t MAPDATA[MAP_SIZE * MAP_SIZE] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
};

typedef struct vfloat { float x, y; } vfloat;
typedef struct vint32_t { int32_t x, y;} vint32_t;

class state {
  public:
    SDL_Window * window;
    SDL_Texture * texture;
    SDL_Renderer * renderer;
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
    vfloat pos, dir, plane;
};

int main() {
  state gameState;
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  gameState.window = SDL_CreateWindow("DOOMEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
  if (gameState.window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  gameState.renderer = SDL_CreateRenderer(gameState.window, -1, SDL_RENDERER_ACCELERATED);
  if (gameState.renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  gameState.texture = SDL_CreateTexture(gameState.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
  if (gameState.renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  while(!gameState.quit) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)){
      switch (ev.type) {
        case SDL_QUIT:
          gameState.quit = true;
          break;
      }
    }
    
    memset(gameState.pixels, 0, sizeof(gameState.pixels));
    gameState.pixels[(10 * SCREEN_WIDTH) + 5] = 0xFFFF00FF;
    SDL_UpdateTexture(gameState.texture, NULL, gameState.pixels, SCREEN_WIDTH * 4);
    SDL_RenderCopyEx(
            gameState.renderer,
            gameState.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL);
        SDL_RenderPresent(gameState.renderer);
  }

  SDL_DestroyTexture(gameState.texture);
  SDL_DestroyRenderer(gameState.renderer);
  SDL_DestroyWindow(gameState.window);
  SDL_Quit();
  return 0;
}
