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

SDL_Rect gSpriteClips[4];

//Scene textures
LTexture gFooTexture;
LTexture gBackgroundTexture;
LTexture gSpriteSheetTexture;

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
  //Load Foo' texture
  if(!gFooTexture.loadFromFile("10_color_keying/foo.png")){
    printf("Feiled to load Foo' texture image!\n");
    success = false;
  }

  if(!gSpriteSheetTexture.loadFromFile("11_clip_rendering_and_sprite_sheets/dots.png")){
    printf("Failed to load sprite sheet texture!\n");
    success = false;
  } else {
    //Set top left sprite
    gSpriteClips[0].x = 0;
    gSpriteClips[0].y = 0;
    gSpriteClips[0].w = 100;
    gSpriteClips[0].h = 100;

    //Set top right sprite
    gSpriteClips[1].x = 100;
    gSpriteClips[1].y = 0;
    gSpriteClips[1].w = 100;
    gSpriteClips[1].h = 100;

    //Set bottom left sprite
    gSpriteClips[2].x = 0;
    gSpriteClips[2].y = 100;
    gSpriteClips[2].w = 100;
    gSpriteClips[2].h = 100;

    //Set bottom right sprite
    gSpriteClips[3].x = 100;
    gSpriteClips[3].y = 100;
    gSpriteClips[3].w = 100;
    gSpriteClips[3].h = 100;
  }

  //Load background texture
  if(!gBackgroundTexture.loadFromFile("10_color_keying/background.png")){
    printf("Failed to load background texture image!\n");
    success = false;
  }
  return success;
}

void close(){
  gFooTexture.free();
  gBackgroundTexture.free();
  gSpriteSheetTexture.free();

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
        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderClear(renderer);

        //Render background texture to screen
        gBackgroundTexture.render(0, 0);

        //Render Foo' to the screen
        gFooTexture.render(240,100);

        //render top left sprite
        gSpriteSheetTexture.render(0,0, &gSpriteClips[0]);

        //Render top right sprite
        gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

        //Render bottom left sprite
        gSpriteSheetTexture.render(0, SCREEN_HEIGHT -gSpriteClips[2].h, &gSpriteClips[2]);

        //Render bottom right sprite
        gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[2].w, 
            SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[3]);
  
        SDL_RenderPresent(renderer);
      }
    }
  }

  //Render background texture to screen
  gBackgroundTexture.render(0, 0);

  //Render Foo' to the screen
  gFooTexture.render(240,190);
  return 0;
}
