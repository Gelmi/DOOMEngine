#include <SDL.h>
#include <SDL_ttf.h>
#include <cstring>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216
#define MAP_SIZE 8
static uint8_t MAPDATA[MAP_SIZE * MAP_SIZE] = {
  1, 1, 1, 1, 1, 1, 1, 4,
  2, 0, 0, 0, 0, 0, 0, 4,
  2, 0, 0, 0, 0, 0, 0, 4,
  2, 0, 0, 0, 0, 0, 0, 4,
  2, 0, 0, 0, 0, 0, 0, 4,
  2, 0, 0, 0, 0, 0, 0, 4,
  2, 0, 0, 0, 0, 0, 0, 4,
  2, 3, 3, 3, 3, 3, 3, 3,
};

typedef struct vfloat { float x, y; } vfloat;
typedef struct vint32_t { int32_t x, y;} vint32_t;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class state {
  public:
    SDL_Window * window;
    SDL_Texture * texture;
    SDL_Renderer * renderer;
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;
    vfloat pos, dir, plane;
    float rotSpeed, posSpeed;
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

  gameState.pos = {3, 3};
  gameState.dir = {1, 0};
  gameState.plane = {0, 1};
  gameState.rotSpeed = 0.1;
  gameState.posSpeed = 0.5;

  while(!gameState.quit) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)){
      switch (ev.type) {
        case SDL_QUIT:SDL_UpdateWindowSurface:
          gameState.quit = true;
          break;
        case SDL_KEYDOWN:
          switch( ev.key.keysym.sym) {
            case SDLK_LEFT: {
              vfloat oldDir = gameState.dir;
              gameState.dir.x = oldDir.x * cos(-gameState.rotSpeed) - oldDir.y * sin(-gameState.rotSpeed);
              gameState.dir.y = oldDir.x * sin(-gameState.rotSpeed) + oldDir.y * cos(-gameState.rotSpeed);
              vfloat oldPlane = gameState.plane;
              gameState.plane.x = oldPlane.x * cos(-gameState.rotSpeed) - oldPlane.y * sin(-gameState.rotSpeed);
              gameState.plane.y = oldPlane.x * sin(-gameState.rotSpeed) + oldPlane.y * cos(-gameState.rotSpeed);
              break;
            }
            case SDLK_RIGHT: {
              vfloat oldDir = gameState.dir;
              gameState.dir.x = oldDir.x * cos(gameState.rotSpeed) - oldDir.y * sin(gameState.rotSpeed);
              gameState.dir.y = oldDir.x * sin(gameState.rotSpeed) + oldDir.y * cos(gameState.rotSpeed);
              vfloat oldPlane = gameState.plane;
              gameState.plane.x = oldPlane.x * cos(gameState.rotSpeed) - oldPlane.y * sin(gameState.rotSpeed);
              gameState.plane.y = oldPlane.x * sin(gameState.rotSpeed) + oldPlane.y * cos(gameState.rotSpeed);
              break; 
            }
            case SDLK_w: {
              if(gameState.pos.x + gameState.dir.x*gameState.posSpeed > 0 && 
                 gameState.pos.y + gameState.dir.y*gameState.posSpeed > 0 &&
                 gameState.pos.x + gameState.dir.x*gameState.posSpeed < MAP_SIZE &&
                 gameState.pos.y + gameState.dir.y*gameState.posSpeed < MAP_SIZE
              ) {
                gameState.pos.x = gameState.pos.x + gameState.dir.x*gameState.posSpeed;
                gameState.pos.y = gameState.pos.y + gameState.dir.y*gameState.posSpeed;
              }
              break;
            }
            case SDLK_s: {
              gameState.pos.x = gameState.pos.x - sgn(gameState.dir.x)*gameState.posSpeed;
              gameState.pos.y = gameState.pos.y - sgn(gameState.dir.y)*gameState.posSpeed;
              break;
            }
          }
      }
    }
    memset(gameState.pixels, 0, sizeof(gameState.pixels));
    //CALCULER CHOSES ICI
    for(int x = 0; x < SCREEN_WIDTH; x++){
      float camera = 2 * x / float(SCREEN_WIDTH) - 1;
      vfloat rayDir = {gameState.dir.x + gameState.plane.x * camera, gameState.dir.y + gameState.plane.y * camera};

      vint32_t mapPos = {int(gameState.pos.x), int(gameState.pos.y)};
      vfloat sideDist;
      // Possivel erro
      vfloat deltaDist = {(rayDir.x == 0) ? FLT_MAX : std::abs(1 / rayDir.x), (rayDir.y == 0) ? FLT_MAX : std::abs(1 / rayDir.y)};

      float perpWallDist;

      vint32_t step;

      int32_t hit = 0;
      int32_t side;

      if(rayDir.x < 0){
        step.x = -1;
        sideDist.x = (gameState.pos.x - mapPos.x) * deltaDist.x;
      } else {
        step.x = 1;
        sideDist.x = (1 + gameState.pos.x - mapPos.x) * deltaDist.x;
      }
      if(rayDir.y < 0){
        step.y = -1;
        sideDist.y = (gameState.pos.y - mapPos.y) * deltaDist.y;
      } else {
        step.y = 1;
        sideDist.y = (1 + gameState.pos.y - mapPos.y) * deltaDist.y;
      }
      
      while(hit == 0){
        if(sideDist.x < sideDist.y){
          sideDist.x += deltaDist.x;
          mapPos.x += step.x;
          side = 0;
        } else {
          sideDist.y += deltaDist.y;
          mapPos.y += step.y;
          side = 1;
        }
        if(MAPDATA[mapPos.x + mapPos.y * MAP_SIZE] > 0) { 
          hit = 1;
          //printf("MAPDATA: %d\n", MAPDATA[mapPos.x + mapPos.y * MAP_SIZE]);
        }
      }

      if(side == 0) perpWallDist = sideDist.x - deltaDist.x;
      else perpWallDist = sideDist.y - deltaDist.y; 

      int32_t lineHeight = (int32_t)(SCREEN_HEIGHT / perpWallDist);
      int32_t drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
      if(drawStart < 0) drawStart = 0;
      int32_t drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
      if(drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;  
      uint32_t color;

      switch (MAPDATA[mapPos.x + mapPos.y * MAP_SIZE]) {
        case 1: 
          color = 0xFF0000; 
          //printf("1\n");
          break;
        case 2: 
          color = 0x00FF00; 
          //printf("2\n");
          break;
        case 3: 
          color = 0x0000FF; 
          //printf("3\n");
          break;
        case 4: 
          color = 0xFFFFFF; 
          //printf("4\n");
          break;
      }

      //printf("%x\n", color);
      
      for(int y = 0; y < SCREEN_HEIGHT; y++){
        if(y > drawStart && y <= drawEnd) {
          if(side == 0 ) gameState.pixels[x + (y * SCREEN_WIDTH)] = color;
          else gameState.pixels[x + (y * SCREEN_WIDTH)] = color;
        }
        else gameState.pixels[x + (y * SCREEN_WIDTH)] = 0;
      }
    } 
    SDL_UpdateTexture(gameState.texture, NULL, gameState.pixels, SCREEN_WIDTH * 4);
    SDL_RenderCopyEx(
            gameState.renderer,
            gameState.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL); 
    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }
    TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", 24);
    //std::cout << "ERROR:" << TTF_GetError() << std::endl;
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }
    std::string score_text = "pos: " + std::to_string(gameState.pos.x) + ", " + std::to_string(gameState.pos.y) + " dir: " + std::to_string(gameState.dir.x) + ", " + std::to_string(gameState.dir.y);        
    SDL_Color textColor = { 255, 255, 255, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, score_text.c_str(), textColor);
    SDL_Texture* text = SDL_CreateTextureFromSurface(gameState.renderer, textSurface);
    int text_width = textSurface->w;
    int text_height = textSurface->h;
    SDL_Rect renderQuad = { 20, 20, text_width, text_height };
    SDL_RenderCopy(gameState.renderer, text, NULL, &renderQuad);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(text);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_RenderPresent(gameState.renderer);
  }
  SDL_DestroyTexture(gameState.texture);
  SDL_DestroyRenderer(gameState.renderer);
  SDL_DestroyWindow(gameState.window);
  SDL_Quit();
  return 0;
}
