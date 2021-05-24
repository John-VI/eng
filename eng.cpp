#include "eng.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>

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
	 const std::string filename) : texture(renderer) {
  setrenderer(renderer);
  loadfile(filename);
  resetwh();
}

bool img::loadfile(const std::string filename) {
  if (sdltexture)
    SDL_DestroyTexture(sdltexture);
  sdltexture = loadtex(ren, filename.c_str());
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

anirow::anirow(int w, int h, int f) {
  width = w;
  height = h;
  frames = f;
}

spritesheet::spritesheet(SDL_Renderer *renderer,
			      const std::string filename,
			      std::vector<struct anirow *> *data) : img(renderer, filename) {
  setdata(data);
  // setwidth(animationdata->at(frame)->width);
  // setheight((animationdata->at(frame)->height));
}

int spritesheet::cframe() {
  return frame;
}

void spritesheet::setframe(int newframe) {
  frame = newframe;
}

int spritesheet::advanceframe() {
  if (cframe() == currentdata()->frames - 1)
    setframe(0);
  else
    setframe(cframe() + 1);
  return cframe();
}

int spritesheet::cset() {
  return set;
}

void spritesheet::setset(int newset) {
  set = newset;
  setstruct = framedata->at(set);
  //setwh(animationdata->at(animationset())->width, animationdata->at(animationset())->height);
  setframe(0);
}

animationsheet::animationsheet(SDL_Renderer *renderer,
	      const std::string filename,
			       std::vector<struct anirow *> *data) :
  spritesheet(renderer, filename, data) {}

bool animationsheet::animated() {
  return playing;
}

void animationsheet::setanimated(bool swtch) {
  playing = swtch;
}

std::vector<struct anirow *> *spritesheet::data() {
  return framedata;
}

void spritesheet::setdata(std::vector<struct anirow *> *newdata) {
  framedata = newdata;
  setset(0);
  //setframe(0);
}

anirow *spritesheet::currentdata() {
  return setstruct;
}

int animationsheet::getspeed() {
  return speed;
}

void animationsheet::setspeed(int newspeed) {
  speed = newspeed;
}

void animationsheet::renderframe(int x, int y,
				 double angle, SDL_Point *center, SDL_RendererFlip flip) {
    int i = 0;
    int offset = 0;
    if (cset())
      for (i = 0; i < cset(); i++)
	offset += data()->at(i)->height;
    SDL_Rect sourcequad = { data()->at(cset())->width * frame,
			    offset,
			    data()->at(cset())->height,
			    data()->at(cset())->width };
    SDL_Rect destquad = { x, y,
			  data()->at(cset())->height,
			  data()->at(cset())->width };
    render(&destquad, &sourcequad);
    if (iter >= speed) {
      if (cframe() < data()->at(cset())->frames - 1)
	setframe(frame + 1);
      else
	setframe(0);
      iter = 0;
    }
    else
      iter += 1;
    
    //englogger->warn("{}, {}, {}, {}", frame, animation, speed, iter);
}

void spritesheet::renderframe(int x, int y, double angle,
			      SDL_Point *center, SDL_RendererFlip flip) {
  int i = 0;
  int offset = 0;
  for (i = 0; i < set; i++)
    offset += framedata->at(i)->height;
  SDL_Rect sourcequad = { setstruct->width * frame, offset,
			  setstruct->width, setstruct->height };
  SDL_Rect destquad = { x, y,
			setstruct->width, setstruct->height };
  render(&destquad, &sourcequad);
}

void spritesheet::renderframe(SDL_Rect *dest, double angle,
			      SDL_Point *center, SDL_RendererFlip flip) {
  int i = 0;
  int offset = 0;
  for (i = 0; i < set; i++)
    offset += framedata->at(i)->height;
  SDL_Rect sourcequad = { setstruct->width * frame, offset,
			  setstruct->width, setstruct->height };
  render(dest, &sourcequad);
}

void spritesheet::renderframe(int row, int col, SDL_Rect *dest, double angle, SDL_Point *center,
			      SDL_RendererFlip flip) {
  int i = 0;
  int offset = 0;
  for (i = 0; i < row; i++)
    offset += framedata->at(i)->height;
  SDL_Rect sourcequad = { setstruct->width * col, offset,
			  setstruct->width, setstruct->height };
  render(dest, &sourcequad);
}

void spritesheet::renderframe(int row, int col, int x, int y, double angle, SDL_Point *center,
			      SDL_RendererFlip flip) {
  SDL_Rect destquad = { x, y,
			  setstruct->width, setstruct->height };
  renderframe(row, col, &destquad, angle, center, flip);
}

void animationsheet::renderframe(SDL_Rect *dest,
				 double angle, SDL_Point *center, SDL_RendererFlip flip) {
    int i = 0;
    int offset = 0;
    if (cset())
      for (i = 0; i < cset(); i++)
	offset += data()->at(i)->height;
    SDL_Rect sourcequad = { data()->at(cset())->width * frame,
			    offset,
			    data()->at(cset())->height,
			    data()->at(cset())->width };
    render(dest, &sourcequad);
    if (iter >= speed) {
      if (cframe() < data()->at(cset())->frames - 1)
	setframe(frame + 1);
      else
	setframe(0);
      iter = 0;
    }
    else
      iter += 1;
    
    //englogger->warn("{}, {}, {}, {}", frame, animation, speed, iter);
}

spritesheet::~spritesheet() {
  for (int i = 0; i < framedata->size(); i++)
    delete framedata->at(i);
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
	   const std::string filename,
	   std::vector<struct anirow *> *data) : spritesheet(renderer, filename, data) {
  // std::vector<struct anirow *> *newdata = new std::vector<struct anirow *>;
  // newdata->push_back(new anirow(9, 16, 96));
  // setdata(newdata);
  defaultcolor = { 255, 255, 255, 255 };
}

void font::renderchar(SDL_Rect *dest, char cchar, double angle,
		      SDL_Point *center, SDL_RendererFlip flip) {
  //                                                       PLACEHOLDER Y
  SDL_Rect sourcequad = { setstruct->width * (cchar - 32), 0,
			  setstruct->width, setstruct->height };
  render(dest, &sourcequad);
}

void font::renderchar(int x, int y, char cchar, double angle,
		      SDL_Point *center, SDL_RendererFlip flip) {
  SDL_Rect sourcequad = { setstruct->width * (cchar - 32), 0,
			  setstruct->width, setstruct->height };
  SDL_Rect destquad = { x, y,
			setstruct->width, setstruct->height };
  render(&destquad, &sourcequad);
}

int font::rendertext(int x, int y, const char *str) {
  char c;
  int i = 0;
  //  int offset = 0;
  int rendered = 0;
  while ((c = str[i]) > 0) {
    //SDL_Rect dest = { };
    i++;
    if (c >= 32 && c <= 127) {
      renderchar(x + (rendered * currentdata()->width), y, c);
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
