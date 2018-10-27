#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

SDL_Window* window = NULL;
SDL_Event event;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Texture* loadTexture(std::string path){
  SDL_Texture* newTexture = NULL;

  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL){
    printf("SDL_image Error: %s\n", IMG_GetError());
  } else {
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if(newTexture == NULL){
      printf("SDL_Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
  }
  return newTexture;
}


bool init(){
  bool success = true;
  if(SDL_Init(SDL_INIT_VIDEO) > 0){
    printf("SDL_Error: %s\n", SDL_GetError());
    success = false;
  } else {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
      printf("Warning: Linear texture filtering not enabled");
    }
    window = SDL_CreateWindow("WWW", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(window == NULL){
      printf("SDL_Error: %s\n", SDL_GetError());
      success = false;
    } else {
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
      if(renderer == NULL){
        printf("SDL_Error: %s\n", SDL_GetError());
        success = false;
      } else {
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
          printf("IMG_Error: %s\n", IMG_GetError());
          success = false;
        }
      }
    }
  }
  return success;
}

bool loadMedia(){
  bool success = true;

  return success;
}

void close(){
  SDL_DestroyTexture(texture);
  texture = NULL;
  SDL_DestroyRenderer(renderer);
  renderer = NULL;
  SDL_DestroyWindow(window);
  window = NULL;
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* argv[]){
  if(!init()){
    printf("INIT ERROR");
  } else {
    if(!loadMedia()){
      printf("Failed to load media\n");
    } else {
      bool quit = false;
      while(!quit){
        while(SDL_PollEvent(&event) != 0){
          if(event.type == SDL_QUIT){
            quit = true;
          }
        }
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        SDL_Rect topRightViewport;
        topRightViewport.x = SCREEN_WIDTH/2;
        topRightViewport.y = 0;
        topRightViewport.w = SCREEN_WIDTH/2;
        topRightViewport.h = SCREEN_HEIGHT/2;
        if(SDL_RenderSetViewport(renderer, &topRightViewport) != 0){
          printf("Error: %s\n", SDL_GetError());
        }
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderFillRect(renderer, &topRightViewport);

        SDL_Rect topLeftViewport;
        topLeftViewport.x = 0;
        topLeftViewport.y = 0;
        topLeftViewport.w = SCREEN_WIDTH/2;
        topLeftViewport.h = SCREEN_HEIGHT/2;
        SDL_RenderSetViewport(renderer, &topLeftViewport);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
        //SDL_RenderFillRect(renderer, &topLeftViewport);

        SDL_Rect bottomViewport;
        bottomViewport.x = 0;
        bottomViewport.y = SCREEN_HEIGHT/2;
        bottomViewport.w = SCREEN_WIDTH;
        bottomViewport.h = SCREEN_HEIGHT/2;
        SDL_RenderSetViewport(renderer, &bottomViewport);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &bottomViewport);
        SDL_RenderDrawRect(renderer, &bottomViewport);

        SDL_Rect fillRect = {SCREEN_WIDTH/2, SCREEN_HEIGHT/4, 
          SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 122);
        SDL_RenderFillRect(renderer, &fillRect);

        SDL_Rect outlineRect = {bottomViewport.x/6, bottomViewport.y/6,
          bottomViewport.w*2/3, bottomViewport.h*2/3};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &outlineRect);

        SDL_SetRenderDrawColor(renderer, 0,0, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 0, 
            bottomViewport.y*2, bottomViewport.x);

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for(int i = 0; i < bottomViewport.w; i+=4){
          SDL_RenderDrawPoint(renderer, bottomViewport.w/2, i);
        }

        SDL_RenderPresent(renderer);
      }
    }
  }
  return 0;
}
