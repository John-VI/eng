#include <stdio.h>
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

#define SCRWIDTH 1280
#define SCRHEIGH 720

// #define STGWIDTH 960
// #define STGHEIGH 720

// #define MSGWIDTH 320
// #define MSGHEIGH 720

auto logger = spdlog::basic_logger_mt("main_logger", "roguelog.txt");

void initlogger() {
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  logger->info("Program starting..");
}

int main(int argc, char *argv[]) {
  initlogger();

  int quit = 0;
  SDL_Event e;
  renwindow *win = NULL;
  font *vga = NULL;
  spritesheet *worldtiles = NULL;
  std::vector<struct anirow *> *fdat = NULL;
  loopingbg *background = NULL;

  timer *steptimer = NULL;

  SDL_Rect *fieldport = NULL;
  SDL_Rect *messgport = NULL;

  if (!initsdl()) {
    win = new renwindow("Parabolus", SCRWIDTH, SCRHEIGH, 0, 0, 0);
    fdat = new std::vector<struct anirow *>;
    fdat->push_back(new struct anirow(9, 16, 96));
    vga = new font(win->getren(), "font.gif", fdat);
    fdat = new std::vector<struct anirow *>;
    fdat->push_back(new struct anirow(16, 16, 10));
    fdat->push_back(new struct anirow(16, 16, 10));
    worldtiles = new spritesheet(win->getren(), "SpritesheetV1.png", fdat);

    fieldport = new SDL_Rect;
    fieldport->x = 0;
    fieldport->y = 0;
    fieldport->w = SCRHEIGH/3 * 4;
    fieldport->h = SCRHEIGH;

    messgport = new SDL_Rect;
    messgport->x = fieldport->w;
    messgport->y = 0;
    messgport->w = SCRWIDTH - fieldport->w;
    messgport->h = SCRHEIGH;

    background = new loopingbg(win->getren(), "Background.png", 0, 0);

    steptimer = new timer();

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
      steptimer->start();

      win->renclear();
      win->setviewport(fieldport);
      background->renderbg();
      worldtiles->renderframe(0, 0, 0, 16);
      worldtiles->renderframe(0, 0, 0, 32);
      worldtiles->renderframe(0, 0, 0, 48);
      worldtiles->renderframe(0, 1, 16, 0);
      worldtiles->renderframe(0, 1, 32, 0);
      worldtiles->renderframe(0, 1, 48, 0);
      worldtiles->renderframe(0, 2, 64, 16);
      worldtiles->renderframe(0, 2, 64, 32);
      worldtiles->renderframe(0, 2, 64, 48);
      worldtiles->renderframe(0, 3, 16, 64);
      worldtiles->renderframe(0, 3, 32, 64);
      worldtiles->renderframe(0, 3, 48, 64);

      win->setviewport(messgport);
      vga->rendertext(0, 0, "TEST PROGRAM");
      vga->rendertext(0, 16, "STR", 255, 0, 0);
      win->renupdate();
    }
  }
  delete steptimer;
  delete background;
  delete fieldport;
  delete messgport;
  delete worldtiles;
  delete win;
  delete vga;
  return 0;
}
