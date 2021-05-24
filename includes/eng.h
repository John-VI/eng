#ifndef ENG_H
#define ENG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class texture {
public:
  texture(SDL_Renderer *renderer,
  	  SDL_Texture *Texture);
  texture(SDL_Renderer *renderer);
  ~texture();

  void free();
  void setblend(SDL_BlendMode blending);
  void render(int x, int y, SDL_Rect *clip = NULL,
	      double angle = 0.0,  SDL_Point *center = NULL,
	      SDL_RendererFlip flip = SDL_FLIP_NONE);
  void render(SDL_Rect *dest, SDL_Rect *clip, double angle = 0.0,
	      SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
  bool getrsize(int *w, int *h);
  int getwidth();
  int getheight();
  void setwidth(int width);
  void setheight(int height);
  void setwh(int width, int height);
  void resetwh();
  SDL_Renderer *getrenderer();
  void setrenderer(SDL_Renderer *rend);
  SDL_Texture *gettexture();
  //void settexture(SDL_Texture *tex);
  //std::string *getname();
  //void setname(std::string *str);
  SDL_Color *getcolor();
  void setcolor(Uint8 red, Uint8 green, Uint8 blue);
  void setcolor(SDL_Color *newcolor);
  void setalpha(Uint8 alpha);

  void destroytexture();
protected:
  // To be implemented at a later date
  //std::string *name;
  SDL_Renderer *ren;
  SDL_Texture *sdltexture = NULL;
  //SDL_Color *color;
  int width;
  int height;
};

class img: public texture {
public:
  img(SDL_Renderer *renderer,
      const std::string filename);
  ~img();
  bool loadfile(const std::string filename);
};

class ttftext: public texture {
public:
  ttftext(SDL_Renderer *renderer,
       TTF_Font *font,
       SDL_Color *colr,
       const std::string inittext);
  ttftext(SDL_Renderer *renderer,
       TTF_Font *font,
       SDL_Color *colr);
  ~ttftext();
  bool rentext(const std::string txt);
  TTF_Font *getfont();
  void setfont(TTF_Font *font);
protected:
  TTF_Font *font;
};

struct anirow {
public:
  anirow(int w, int h, int f);
  int width;
  int height;
  int frames;
};

class spritesheet: public img {
public:
  spritesheet(SDL_Renderer *renderer,
		 const std::string filename,
		 std::vector<struct anirow *> *data);
  int cframe();
  void setframe(int newframe);
  int advanceframe();
  int cset();
  void setset(int newset);
  // bool animated();
  // void setanimated(bool swtch);
  std::vector<struct anirow *> *data();
  void setdata(std::vector<struct anirow *> *newdata);
  anirow *currentdata();
  // int getspeed();
  // void setspeed(int newspeed);
  void renderframe(int x, int y, double angle = 0.0,  SDL_Point *center = NULL,
		   SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderframe(SDL_Rect *dest, double angle = 0.0,  SDL_Point *center = NULL,
		   SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderframe(int row, int col, SDL_Rect *dest, double angle = 0.0,  SDL_Point *center = NULL,
		   SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderframe(int row, int col, int x, int y, double angle = 0.0,  SDL_Point *center = NULL,
		   SDL_RendererFlip flip = SDL_FLIP_NONE);
  ~spritesheet();
protected:
  // spritesheet(SDL_Renderer *renderer,
  // 	      const std::string filename);
  std::vector<struct anirow *> *framedata;
  int frame = 0;
  int set = 0;
  struct anirow *setstruct = NULL;
  // bool playing = true;
  // int speed = 1;
  // int iter = 0;
};

class animationsheet: public spritesheet {
public:
  animationsheet(SDL_Renderer *renderer,
	      const std::string filename,
	      std::vector<struct anirow *> *data);
  bool animated();
  void setanimated(bool swtch);
  int getspeed();
  void setspeed(int newspeed);
  void renderframe(int x, int y, double angle = 0.0,  SDL_Point *center = NULL,
		   SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderframe(SDL_Rect *dest, double angle = 0.0,  SDL_Point *center = NULL,
		   SDL_RendererFlip flip = SDL_FLIP_NONE);
protected:
  bool playing = true;
  int speed = 1;
  int iter = 0;
};

class font: public spritesheet {
public:
  font(SDL_Renderer *renderer,
       const std::string filename,
       std::vector<struct anirow *> *data);
  void renderchar(SDL_Rect *dest, char cchar, double angle = 0.0, SDL_Point *center = NULL,
		  SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderchar(int x, int y, char cchar, double angle = 0.0, SDL_Point *center = NULL,
		  SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderchar(int x, int y, char cchar, int r, int g, int b, double angle = 0.0,
		  SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
  void renderchar(SDL_Rect *dest, char cchar, int r, int g, int b, double angle = 0.0,
		  SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
  // void renderchar(int x, int y, SDL_Color color, double angle = 0.0, SDL_Point *center = NULL,
  // 		  SDL_RendererFlip flip = SDL_FLIP_NONE);
  // void renderchar(SDL_Point *center, SDL_Color color, double angle = 0.0, SDL_Point *center = NULL,
  // 		  SDL_RendererFlip flip = SDL_FLIP_NONE);
  int rendertext(int x, int y, const char *str);
  int rendertext(int x, int y, const char *str, int r, int g, int b);
  int rendertext(int x, int y, const char *str, SDL_Color color);
  void setdefaultcolor(int r, int g, int b);
  void setdefaultcolor(SDL_Color newcolor);
  void resetcolor();
  // int rendertext(std::string *str);
  ~font();
private:
  SDL_Color defaultcolor;
};

class renwindow {
public:
  renwindow(const std::string name, int width,
	     int height, Uint8 red = 0xFF,
	     Uint8 green = 0xFF, Uint8 blue = 0xFF,
	     Uint8 alpha = 0xFF);
  renwindow(const std::string name, int width,
	    int height, SDL_Color color);
  ~renwindow();
  SDL_Window *getwindow();
  SDL_Renderer *getren();
  int getwidth();
  int getheight();
  void renclear();
  void renupdate();
  int setviewport(const SDL_Rect *port);
  void setcolor(SDL_Color newcolor);
  void setcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
  void resetcolor();
protected:
  SDL_Window *window;
  SDL_Renderer *ren;
  int w;
  int h;
  SDL_Color rencolor;
};

class timer {
public:
  timer();
  void start();
  void stop();
  void pause();
  void unpause();

  Uint32 getticks();

  bool started();
  bool paused();
protected:
  Uint32 startticks;
  Uint32 pauseticks;
  bool pse;
  bool strt;
};

class loopingbg : public img {
public:
  loopingbg(SDL_Renderer *renderer, const char *filename, float hspd, float vspd);
  void sethspeed(float newspeed);
  void setvspeed(float newspeed);
  int gethspeed();
  int getvspeed();

  void tickbg(float ticks);
  void renderbg();
protected:
  float vertspd;
  float horispd;
  float voff;
  float hoff;
};

struct menuopt {
  bool selected = 0;
  bool chosen = 0;
  texture *seltexture = NULL;
};

struct menuopts {
  std::vector<struct menuopt *> *options = NULL;
  int selected = -1;
  int chosen = -1;
};

struct pad {
  bool buttons[SDL_CONTROLLER_BUTTON_MAX];
  int axis[SDL_CONTROLLER_AXIS_MAX];
};

int initsdl();
void closesdl();
std::vector<SDL_GameController *> *initpads();

#endif