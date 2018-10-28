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
    //Set blending
    void setBlendMode(SDL_BlendMode blending);
    //Set alpha modulation
    void setAlpha(Uint8 alpha);
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

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int WALKING_ANIMATION_FRAMES = 4;

SDL_Window* window = NULL;
SDL_Event event;
SDL_Renderer* renderer = NULL;

SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;

//Scene textures
LTexture gModulatedTexture;
LTexture gBackgroundTexture;

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
    SDL_SetColorKey(loadedSurface, SDL_TRUE, 
        SDL_MapRGB(loadedSurface->format, 0, 255, 255));
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

void LTexture::setBlendMode(SDL_BlendMode blending){
  //Set blending function
  SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
  //Modulate texture alpha
  SDL_SetTextureAlphaMod(mTexture, alpha);
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
      renderer = SDL_CreateRenderer(window, -1, 
          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
  if(!gSpriteSheetTexture.loadFromFile("14_animated_sprites_and_vsync/foo.png")){
    printf("Failed to load walking animation texture!\n");
  } else {
    gSpriteClips[0].x = 0;
    gSpriteClips[0].y = 0;
    gSpriteClips[0].w = 64;
    gSpriteClips[0].h = 205;

    gSpriteClips[1].x = 64;
    gSpriteClips[1].y = 0;
    gSpriteClips[1].w = 64;
    gSpriteClips[1].h = 205;

    gSpriteClips[2].x = 128;
    gSpriteClips[2].y = 0;
    gSpriteClips[2].w = 64;
    gSpriteClips[2].h = 205;

    gSpriteClips[3].x = 196;
    gSpriteClips[3].y = 0;
    gSpriteClips[3].w = 64;
    gSpriteClips[3].h = 205;
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

      int frame = 0;
      while(!quit){
        while(SDL_PollEvent(&event) != 0){
          if(event.type == SDL_QUIT){
            quit = true;
          } 
        }
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderClear(renderer);
        
        SDL_Rect* currentClip = &gSpriteClips[frame/8];
        gSpriteSheetTexture.render((SCREEN_WIDTH - currentClip->w)/2,
            (SCREEN_HEIGHT - currentClip->h)/2, currentClip);
                 
        SDL_RenderPresent(renderer);
        ++frame;
        if(frame/8 >= WALKING_ANIMATION_FRAMES)
        {
          frame = 0;
        }
      }
    }
  }
  return 0;
}
