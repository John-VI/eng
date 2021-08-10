#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <cmath>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_ttf.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "eng.h"

#define INTWIDTH 900
#define INTHEIGH 560

#define SCRWIDTH 1366
#define SCRHEIGH 768

// #define STGWIDTH 960
// #define STGHEIGH 720

// #define MSGWIDTH 320
// #define MSGHEIGH 720

auto logger = spdlog::basic_logger_mt("main_logger", "roguelog.txt");
auto messagelog = spdlog::basic_logger_mt("Messages", "messagelog.txt");

void initlogger() {
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  logger->info("Program starting..");
}

template <class T>
void pushstack(T *stack[], T *newentry, int size) {
  T *temp1;
  T *temp2 = newentry;
  for (int i=size-1;i>=0;i--) {
    temp1 = stack[i];
    // logger->debug("1");
    stack[i] = temp2;
    // logger->debug("2");
    temp2 = temp1;
    // logger->debug("3");
  }
  delete temp1;
}

template <class T>
void invert(T *sequence, unsigned int size) {
  T temp;
  unsigned int i = 0;
  size--;
  while (i < size) {
    // logger->debug("size: {}, i: {}", size, i);
    temp = sequence[i];
    sequence[i] = sequence[size];
    sequence[size] = temp;
    i++;
    size--;
  }
}

char *atoi(int size) {
  unsigned int place = 0;
  unsigned int strln = 0;
  char *str = NULL;
  if (size == 0) {
    str = new char[2];
    str[0] = '0';
    str[1] = 0;
  } else if (size <= -1) {
    strln = (unsigned int)floor(log10(-size)) + 3;
    str = new char[strln];
    for (int i = 0; i < strln; i++)
      str[i] = 0;
    place = strln - 2;
    while (size <= -1) {
      if (str[place] <= '0') {
	str[place] = '1';
	size++;
	place = strln - 2;
      } else if (str[place] >= '9') {
	str[place] = '0';
	place--;
      } else {
	str[place]++;
	size++;
	place = strln - 2;
      }
    }
    str[0] = '-';
    // str[strln - 1] = 0;
  } else {
    strln = (unsigned int)floor(log10(size)) + 2;
    str = new char[strln];
    for (int i = 0; i < strln; i++)
      str[i] = 0;
    place = strln - 2;
    while (size >= 1) {
      if (str[place] <= '0') {
	str[place] = '1';
	size--;
	place = strln - 2;
      } else if (str[place] >= '9') {
	str[place] = '0';
	place--;
      } else {
	str[place]++;
	size--;
	place = strln - 2;
      }
    }
    // str[strln - 1] = 0;
  }
  // invert<char>(str, usage);
  return str;
}

int main(int argc, char *argv[]) {
  initlogger();

  int quit = 0;
  SDL_Event e;
  renwindow *win = NULL;
  font *vga = NULL;
  spritesheet *worldtiles = NULL;
  struct anirow *fdat = NULL;
  loopingbg *background = NULL;

  timer *steptimer = NULL;

  SDL_Rect fieldport;
  SDL_Rect messgport;
  SDL_Rect mainport;

  std::string *messages[5] = { NULL };
  int strint = -10;
  float ftick = 0;
  char *newcstr = NULL;
  SDL_Color red = { 255, 0, 0, 0 };
  SDL_Rect wrapzone = { 0, 64, 100, 500 };

  if (!initsdl()) {
    win = new renwindow("Parabolus", SCRWIDTH, SCRHEIGH, INTWIDTH, INTHEIGH, 0, 0, 0);
    SDL_RenderSetLogicalSize(win->getren(), INTWIDTH, INTHEIGH);
    fdat = new struct anirow(0, 0, 9, 16, 96);
    vga = new font(win->getren(), "font.gif", fdat, (uint8_t)1);
    worldtiles = new spritesheet(win->getren(), "SpritesheetV1.png", "SpritesheetV1.fd");

    SDL_RenderGetViewport(win->getren(), &mainport);

    fieldport.x = mainport.x;
    fieldport.y = mainport.y;
    fieldport.w = mainport.w/3 * 2;
    fieldport.h = mainport.h;

    messgport.x = mainport.x + fieldport.w;
    messgport.y = mainport.y;
    messgport.w = mainport.w - fieldport.w;
    messgport.h = mainport.h;

    background = new loopingbg(win->getren(), "Background.png", 0, 0);

    steptimer = new timer();

    newcstr = new char[500];

    while (quit != 1) {
      while (SDL_PollEvent(&e) != 0) {
	if (e.type == SDL_QUIT)
	  quit = 1;
      }

      int x = 0;
      int y = 0;
      SDL_GetMouseState(&x, &y);
      background->setvspeed(y - 360);
      background->sethspeed(x - 640);
      float ticks = steptimer->getticks() / 1000.f;
      background->tickbg(ticks);
 
      if ((ftick += ticks) >= .2f) {
	ftick -= .2f;
	snprintf(newcstr, 500, "%d", strint);
	std::string *newstr = new std::string(newcstr);
	// logger->debug("newstr: {}", newstr);
	// for (int i = 0; i < 10; i++)
	//   logger->debug("Character {}: {}", i, newstr[i]);
	pushstack<std::string>(messages, newstr, 5);
	strint++;
      }
      
      steptimer->start();

      win->renclear();
      win->setviewport(&fieldport);
      background->renderbg();
      worldtiles->rendersprite(0, 0, 0, 16);
      worldtiles->rendersprite(0, 0, 0, 32);
      worldtiles->rendersprite(0, 0, 0, 48);
      worldtiles->rendersprite(0, 1, 16, 0);
      worldtiles->rendersprite(0, 1, 32, 0);
      worldtiles->rendersprite(0, 1, 48, 0);
      worldtiles->rendersprite(0, 2, 64, 16);
      worldtiles->rendersprite(0, 2, 64, 32);
      worldtiles->rendersprite(0, 2, 64, 48);
      worldtiles->rendersprite(0, 3, 16, 64);
      worldtiles->rendersprite(0, 3, 32, 64);
      worldtiles->rendersprite(0, 3, 48, 64);

      win->setviewport(&messgport);
      vga->rendertext(0, 0, "TEST PROGRAMMMMMMMMMMMMMMMMMMM");
      vga->rendertext(0, 16, "STR", &red);
      vga->renderwrappedtext(&wrapzone,
      			     "dog poop dog poop aaaaaaaaaaaaaaaaaaaaaaaaaaa EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
      for (int i = 4; i >= 0; i--)
	if (messages[i])
	  vga->rendertext(0, 50 + i * 16, messages[i]->c_str());
      
      win->renupdate();
    }
  }
  delete[] newcstr;
  delete steptimer;
  delete background;
  delete worldtiles;
  delete win;
  delete vga;
  return 0;
}
