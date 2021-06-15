#include "eng.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <limits>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

// struct pad {
//   bool buttons[SDL_CONTROLLER_BUTTON_MAX];
//   int axis[SDL_CONTROLLER_AXIS_MAX];
// };

auto englogger = spdlog::basic_logger_mt("engine_logger", "enginelog.txt");

SDL_Surface *loadmedia(const char filename[]) {
  SDL_Surface *img = NULL;
  if ((img = IMG_Load(filename)) == NULL)
    englogger->warn("Failed to load media {}: {}", filename, SDL_GetError());
  return img;
}

SDL_Texture *loadtex(SDL_Renderer *ren, const char filename[]) {
  SDL_Texture *tex = NULL;
  SDL_Surface *surf = loadmedia(filename);
  tex = SDL_CreateTextureFromSurface(ren, surf);
  if (tex == NULL) 
    englogger->warn("Couldn't load texture: {}", SDL_GetError());

  SDL_FreeSurface(surf);
  return tex;
}

SDL_Texture *texttexture(SDL_Renderer *ren,  TTF_Font *font,
			 const char *str,  SDL_Color color) {
  SDL_Surface *surf = TTF_RenderText_Solid(font, str, color);
  return SDL_CreateTextureFromSurface(ren, surf);
}

void texture::free() {
  SDL_DestroyTexture(sdltexture);
}

void texture::setcolor( Uint8 red,
		        Uint8 green,
		        Uint8 blue) {
  // color->r = red;
  // color->g = green;
  // color->b = blue;
  SDL_SetTextureColorMod(sdltexture, red, green, blue);
}

void texture::setalpha(Uint8 alpha) {
  //color->a = alpha;
  SDL_SetTextureAlphaMod(sdltexture, alpha);
}

void texture::setblend(SDL_BlendMode blending) {
  SDL_SetTextureBlendMode(sdltexture, blending);
}

void texture::render(int x, int y, SDL_Rect *clip, double angle,
		     SDL_Point *center, SDL_RendererFlip flip) {
  SDL_Rect rquad = { x, y, width, height };

   // if (clip != NULL) {
  //   rquad.w = clip->w;
  //   rquad.h = clip->h;
  // }
  SDL_RenderCopyEx(ren, sdltexture, clip, &rquad, angle, center, flip);
  //  englogger->warn("{}", SDL_GetError);
}

void texture::render(SDL_Rect *dest, SDL_Rect *clip, double angle, SDL_Point *center,
		     SDL_RendererFlip flip) {
  SDL_RenderCopyEx(ren, sdltexture, clip, dest, angle, center, flip);
}

bool texture::getrsize(int *w, int *h) {
  return SDL_QueryTexture(sdltexture, NULL, NULL, w, h) == 0;
}

int texture::getwidth() {
  return width;
}

int texture::getheight() {
  return height;
}

void texture::setwidth(int w) {
  width = w;
}

void texture::setheight(int h) {
  height = h;
}

void texture::setwh(int w, int h) {
  width = w;
  height = h;
}

void texture::resetwh() {
  int w, h;
  getrsize(&w, &h);
  width = w;
  height = h;
}

SDL_Renderer *texture::getrenderer() {
  return ren;
}

void texture::setrenderer(SDL_Renderer *renderer) {
  ren = renderer;
}

SDL_Texture *texture::gettexture() {
  return sdltexture;
}

// void texture::settexture( SDL_Texture *texture) {
//   sdltexture = texture;
//   resetwh();
// }

// texture::texture(SDL_Renderer *renderer,
// 		  SDL_Texture *texture) {
//   setrenderer(renderer);
//   settexture(texture);
// }

void texture::setcolor(SDL_Color *newcolor) {
  //color = newcolor;
  SDL_SetTextureColorMod(sdltexture, newcolor->r, newcolor->g, newcolor->b);
}

texture::texture(SDL_Renderer *renderer) {
  setrenderer(renderer);
}

texture::~texture() {
  if (sdltexture)
    SDL_DestroyTexture(sdltexture);
}

SDL_Window *newwin(const std::string name, int width,
		    int height) {
  SDL_Window *win = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED,
				     SDL_WINDOWPOS_UNDEFINED, width, height,
				     SDL_WINDOW_SHOWN);
  if (win == NULL) {
    englogger->critical("Window failed! {}", SDL_GetError());
  }
  return win;
}

SDL_Renderer *newren( SDL_Window *win,  Uint8 r,  Uint8 g,
		      Uint8 b,  Uint8 a) {
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (rend == NULL)
    englogger->critical("Could not create renderer! {}", SDL_GetError());
  else {
    SDL_SetRenderDrawColor(rend, r, g, b, a);
  }
  return rend;
}

/* ========= Img      ========= */

img::img(SDL_Renderer *renderer,
	 const char filename[]) : texture(renderer) {
  setrenderer(renderer);
  loadfile(filename);
  resetwh();
}

bool img::loadfile(const char filename[]) {
  if (sdltexture)
    SDL_DestroyTexture(sdltexture);
  sdltexture = loadtex(ren, filename);
  return (sdltexture != NULL);
}

img::~img() {}

/* ========= Text     ========= */

ttftext::ttftext(SDL_Renderer *renderer,
	   TTF_Font *font,
	   SDL_Color *colr,
	   const std::string inittext) : texture(renderer) {
  setfont(font);
  setcolor(colr);
  rentext(inittext);
  resetwh();
}

ttftext::ttftext(SDL_Renderer *renderer,
	   TTF_Font *font,
	   SDL_Color *colr) : texture(renderer) {
  setfont(font);
  setcolor(colr);
  //resetwh();
}

bool ttftext::rentext(const std::string txt) {
  if (sdltexture != NULL)
    SDL_DestroyTexture(sdltexture);
  SDL_Color *color = new SDL_Color;
  color->a = 255;
  color->r = 255;
  color->g = 255;
  color->b = 255;
  sdltexture = texttexture(ren, font, txt.c_str(), *color);
  resetwh();
  delete color;
  return (sdltexture != NULL);
}

TTF_Font *ttftext::getfont() {
  return font;
}

void ttftext::setfont( TTF_Font *newfont) {
  font = newfont;
}

ttftext::~ttftext() {}

/* ========= spritesheet ========== */

anirow::anirow(uint16_t xpos, uint16_t ypos, uint16_t w, uint16_t h, uint8_t f) {
  x = xpos;
  y = ypos;
  width = w;
  height = h;
  frames = f;
}

anirow::anirow() {}

spritesheet::spritesheet(SDL_Renderer *renderer,
			 const char filename[],
			 struct anirow *data, uint8_t size) : img(renderer, filename) {
  setdata(data, size);
  // setwidth(animationdata->at(frame)->width);
  // setheight((animationdata->at(frame)->height));
}

spritesheet::spritesheet(SDL_Renderer *renderer,
			 const char filename[],
			 const char datafile[]) : img(renderer, filename) {
  loaddatafile(datafile);
}

spritesheet::spritesheet(SDL_Renderer *renderer,
			 const char filename[]) : img(renderer, filename) {}

int spritesheet::loaddatafile(const char filename[]) {
  std::ifstream F = std::ifstream(filename, std::ios::in | std::ios::binary);
  char buffer[2];
  struct anirow *newdata = NULL;
  char cells = 0;
  unsigned int length = 0;

  F.read(buffer, 1);
  cells = buffer[0];
  F.ignore(std::numeric_limits<std::streamsize>::max());
  length = F.gcount() - 1;
  F.clear();
  F.seekg(1, std::ios_base::beg);
  englogger->debug("Length: {}, cells: {}", length, cells);
  
  if (cells * 9 >= length) {
    newdata = new struct anirow[buffer[0]];
    for (int i = 0; i < cells; i++) {
      F.read(buffer, 2);
      newdata[i].x = *buffer;
      F.read(buffer, 2);
      newdata[i].y = *buffer;
      F.read(buffer, 2);
      newdata[i].width = *buffer;
      F.read(buffer, 2);
      newdata[i].height = *buffer;
      F.read(buffer, 1);
      newdata[i].frames = buffer[0];
    }
    F.close();
    setdata(newdata, cells);
    return F.gcount();
  } else {
    setdata(NULL, 0);
    F.close();
    return -1;
  }
}

// animationsheet::animationsheet(SDL_Renderer *renderer,
// 	      const std::string filename,
// 			       std::vector<struct anirow *> *data) :
//   spritesheet(renderer, filename, data) {}

// bool animationsheet::animated() {
//   return playing;
// }

// void animationsheet::setanimated(bool swtch) {
//   playing = swtch;
// }

struct anirow *spritesheet::data() {
  return framedata;
}

void spritesheet::setdata(struct anirow *newdata, uint8_t datalength) {
  framedata = newdata;
  length = datalength;
  //setframe(0);
}

// int animationsheet::getspeed() {
//   return speed;
// }

// void animationsheet::setspeed(int newspeed) {
//   speed = newspeed;
// }

void spritesheet::rendersprite(uint8_t row, uint8_t col, SDL_Rect *dest, double angle, SDL_Point *center,
			      SDL_RendererFlip flip) {
  SDL_Rect sourcequad = { data()[row].x + data()[row].width * col, data()[row].y,
			  data()[row].width, data()[row].height };
  render(dest, &sourcequad, angle, center, flip);
}

void spritesheet::rendersprite(uint8_t row, uint8_t col, int x, int y, double angle, SDL_Point *center,
			      SDL_RendererFlip flip) {
  SDL_Rect destquad = { x, y,
			data()[row].width, data()[row].height };
  rendersprite(row, col, &destquad, angle, center, flip);
}

spritesheet::~spritesheet() {
  delete framedata;
}

// class renwindow {
// public:
//   renwindow(const std::string name, int width,
// 	     int height, Uint8 red = 0xFF,
// 	     Uint8 green = 0xFF, Uint8 blue = 0xFF,
// 	     Uint8 alpha = 0xFF);
//   ~renwindow();
//   SDL_Window *getwindow();
//   SDL_Renderer *getren();
//   int getwidth();
//   int getheight();
//   void renclear();
//   void renupdate();
// private:
//   SDL_Window *window;
//   SDL_Renderer *ren;
//   int w;
//   int h;
// };

/* ====== Font ====== */

//This is specifically for the font since it specificies its own data internally

// spritesheet::spritesheet(SDL_Renderer *renderer,
// 			 const std::string filename): img(renderer, filename) {}

font::font(SDL_Renderer *renderer,
	   const char filename[], struct anirow *data,
	   uint8_t size) : spritesheet(renderer, filename, data, size) {
  // std::vector<struct anirow *> *newdata = new std::vector<struct anirow *>;
  // newdata->push_back(new anirow(9, 16, 96));
  // setdata(newdata);
  defaultcolor = { 255, 255, 255, 255 };
}

void font::renderchar(SDL_Rect *dest, char cchar, double angle,
		      SDL_Point *center, SDL_RendererFlip flip) {
  //                                                       PLACEHOLDER Y
  SDL_Rect sourcequad = { framedata->width * (cchar - 32), 0,
			  framedata->width, framedata->height };
  render(dest, &sourcequad);
}

void font::renderchar(int x, int y, char cchar, double angle,
		      SDL_Point *center, SDL_RendererFlip flip) {
  SDL_Rect sourcequad = { framedata->width * (cchar - 32), 0,
			  framedata->width, framedata->height };
  SDL_Rect destquad = { x, y,
			framedata->width, framedata->height };
  render(&destquad, &sourcequad);
}

int font::rendertext(int x, int y, const char *str) {
  if (!str)
    return 0;
  char c;
  int i = 0;
  //  int offset = 0;
  int rendered = 0;
  while ((c = str[i]) > 0) {
    //SDL_Rect dest = { };
    i++;
    if (c >= 32 && c <= 127) {
      renderchar(x + (rendered * framedata->width), y, c);
      rendered++;
    }
  }
  return rendered;
}

int font::rendertext(int x, int y, const char *str, int r, int g, int b) {
  setcolor(r, g, b);
  int tmp = rendertext(x, y, str);
  setcolor(255, 255, 255);
  return tmp;
}

font::~font() {}

/* ====== renwindow ====== */

renwindow::renwindow(const std::string name, int width,
		     int height, Uint8 red,
		     Uint8 green, Uint8 blue,
		     Uint8 alpha) {
  window = newwin(name, width, height);
  rencolor.r = red;
  rencolor.g = green;
  rencolor.b = blue;
  rencolor.a = alpha;
  ren = newren(window, red, green, blue, alpha);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
  w = width;
  h = height;
}

renwindow::renwindow(const std::string name, int width,
	  int height, SDL_Color color) {
  window = newwin(name, width, height);
  rencolor = color;
  ren = newren(window, color.r, color.g, color.b, color.a);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
  w = width;
  h = height;
}

renwindow::~renwindow() {
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(window);
}

SDL_Window *renwindow::getwindow() {
  return window;
}

SDL_Renderer *renwindow::getren() {
  return ren;
}

int renwindow::getwidth() {
  return w;
}

int renwindow::getheight() {
  return h;
}

void renwindow::renclear() {
  SDL_RenderClear(ren);
}

void renwindow::renupdate() {
  SDL_RenderPresent(ren);
}

int renwindow::setviewport(const SDL_Rect *port) {
  return SDL_RenderSetViewport(ren, port);
}

void renwindow::setcolor(SDL_Color newcolor) {
  SDL_SetRenderDrawColor(getren(), newcolor.r, newcolor.g,
			 newcolor.b, newcolor.a);
}

void renwindow::setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  SDL_SetRenderDrawColor(getren(), r, g, b, a);
}

void renwindow::resetcolor() {
  SDL_SetRenderDrawColor(getren(), rencolor.r, rencolor.g, rencolor.b, rencolor.a);
}

/* ----- timer ----- */

timer::timer() {
  startticks = 0;
  pauseticks = 0;
  pse = false;
  strt = false;
}

void timer::start() {
  strt = true;
  pse = false;
  startticks = SDL_GetTicks();
  pauseticks = 0;
}

void timer::stop() {
  strt = false;
  pse = false;
  startticks = 0;
  pauseticks = 0;
}

void timer::pause() {
  if (strt && !pse) {
    pse = true;

    pauseticks = SDL_GetTicks() - startticks;
    startticks = 0;
  }
}

void timer::unpause() {
  if (strt && pse) {
    pse = false;
    startticks = SDL_GetTicks() - pauseticks;
    pauseticks = 0;
  }
}

Uint32 timer::getticks() {
  Uint32 time = 0;
  if (strt) {
    if (pse)
      time = pauseticks;
    else
      time = SDL_GetTicks() - startticks;
  }
  return time;
}

bool timer::started() {
  return strt;
}

bool timer::paused() {
  return pse && strt;
}

/* ----- loopingbg ----- */

loopingbg::loopingbg(SDL_Renderer *renderer, const char *filename, float hspd, float vspd) :
                    img(renderer, filename) {
  sethspeed(hspd);
  setvspeed(vspd);
  hoff = 0;
  voff = 0;
}

int loopingbg::gethspeed() {
  return horispd;
}

int loopingbg::getvspeed() {
  return vertspd;
}

void loopingbg::tickbg(float ticks) {
  voff += ticks * vertspd;
  hoff += ticks * horispd;

  if (abs(voff) > getheight())
    voff -= getheight() * (voff/abs(voff));
  if (abs(hoff) > getwidth())
    hoff -= getwidth() * (hoff/abs(hoff));
}

void loopingbg::renderbg() {
  render(hoff, voff);
  if (voff) 
    render(hoff, voff - getheight() * (voff/abs(voff)));
  if (hoff)
    render(hoff - getwidth() * (hoff/abs(hoff)), voff);
  if (hoff && voff)
    render(hoff - getwidth() * (hoff/abs(hoff)), voff - getheight() * (voff/abs(voff)));
}

void loopingbg::sethspeed(float newspeed) {
  horispd = newspeed;
}

void loopingbg::setvspeed(float newspeed) {
  vertspd = newspeed;
}

// struct menuopt {
//   bool selected = 0;
//   bool chosen = 0;
//   Texture *texture = NULL;
// };

// struct menuopts {
//   std::vector<struct menuopt *> *options = NULL;
//   int selected = -1;
//   int chosen = -1;
// };

int initsdl() {
  englogger->set_level(spdlog::level::debug);
  englogger->flush_on(spdlog::level::debug);
  englogger->info("Program starting...");
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO |
	       SDL_INIT_GAMECONTROLLER) < 0) {
    englogger->critical("SDL failed! {}\n", SDL_GetError());
    return 1;
  }
  int imgflags = IMG_INIT_PNG;
  if (!(IMG_Init(imgflags) & imgflags)) {
    englogger->critical("SDL_image failed! {}", IMG_GetError());
    return 2;
  }
  if(TTF_Init() == -1) {
    englogger->critical("SDL_ttf failed! {}", TTF_GetError());
    return 3;
  }
  return 0;
}

void closesdl() {
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

std::vector<SDL_GameController *> *initpads() {
  // This one is basically stolen from
  // https://sausage-factory.games/dev-blog/easier-game-controller-input-in-sdl/
  int sticks = SDL_NumJoysticks();
  unsigned int padc = 0;
  std::vector<SDL_GameController *> *pads = NULL;

  for (unsigned int i=0;i<sticks;i++)
    if (SDL_IsGameController(i))
      padc++;

  if (padc) {
    pads = new std::vector<SDL_GameController *>;
    // Reallocing for dead controller may save RAM, but I decided the CPU cycles
    // Were worth more.
    for (unsigned int i=0;i<padc;i++) {
      SDL_GameController *pad = SDL_GameControllerOpen(i);
      if (SDL_GameControllerGetAttached(pad) == 1) {
	pads->push_back(pad);
      } else {
	englogger->warn("Dead controller: {}", SDL_GetError());
      }
    }
  }
  return pads;
}
