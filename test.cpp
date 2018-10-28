#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

class LTexture {
  public:
    //Init variables
    LTexture();
    //Deallocate memory
    ~LTexture();
    //Loads image at specified path
    bool loadFromFile(std::string path);
    //Deallocates texture
    void free();
    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    //Renders texture at given point
    void render(int x, int y, SDL_Rect* clip = NULL);
    //Gets image dimensions
    int getWidth();
    int getHeight();

  private:
    //The actual hardware texture
    SDL_Texture* mTexture;

    //Image dimensions
    int mWidth;
    int mHeight;
};

SDL_Window* window = NULL;
SDL_Event event;
SDL_Renderer* renderer = NULL;

//Scene textures
LTexture gModulatedTexture;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

LTexture::LTexture() {
  //Init
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture(){
  //Deallocate 
  free();
}

bool LTexture::loadFromFile(std::string path){
  //Get rid of preexisting texture
  free();
  //The final texture
  SDL_Texture* newTexture = NULL;
  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL){
    printf("Unable to load image %s! SDL_image_Error: %s\n", path.c_str(), IMG_GetError());
  } else {
    //Color key image
    //SDL_SetColorKey(loadedSurface, SDL_TRUE, 
    //    SDL_MapRGB(loadedSurface->format, 0, 255, 255));
    //Create Texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if(newTexture==NULL){
      printf("Unable to create texture from %s! SDL_Error: %s\n", 
          path.c_str(), SDL_GetError());
    } else {
      //Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }
    //Get rid of old loaded Surface
    SDL_FreeSurface(loadedSurface);
  }
  //Return success
  mTexture = newTexture;
  return mTexture != NULL;
}

void LTexture::free(){
  if(mTexture != NULL){
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
  //Modulate texture
  SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::render(int x, int y, SDL_Rect* clip){
  //Set rendering space and render to xcreen
  SDL_Rect renderQuad = {x, y, mWidth, mHeight};
  //Set clip rendering dimensions
  if(clip != NULL){
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }
  //Render to screen
  SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth(){
  return mWidth;
}

int LTexture::getHeight(){
  return mHeight;
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
  if(!gModulatedTexture.loadFromFile("12_color_modulation/colors.png")){
    printf("Failed to load file for texture");
    success = false;
  }
  return success;
}

void close(){
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

      Uint8 r = 255;
      Uint8 g = 255;
      Uint8 b = 255;
      while(!quit){
        while(SDL_PollEvent(&event) != 0){
          if(event.type == SDL_QUIT){
            quit = true;
          } else if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym){
              //Increase red
              case SDLK_q:
                r+=32;
                break;
              //Increase green
              case SDLK_w:
                g+=32;
                break;
              //Increase blue
              case SDLK_e:
                b+=32;
                break;
              //decrease red
              case SDLK_a:
                r-=32;
                break;
              //decrease green
              case SDLK_s:
                g-=32;
                break;
              //decrease blue
              case SDLK_d:
                b-=32;
                break;
            }
          }
        }
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderClear(renderer);
        
        gModulatedTexture.setColor(r, g, b);
        gModulatedTexture.render(0,0);
  
        SDL_RenderPresent(renderer);
      }
    }
  }
  return 0;
}
