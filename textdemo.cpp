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

#define SCRWIDTH 640
#define SCRHEIGH 480

auto logger = spdlog::basic_logger_mt("main_logger", "textdemolog.txt");

void initlogger() {
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  logger->info("Program starting...");
}

int main(int argc, char *argv[]) {
  initlogger();

  int quit = 0;
  SDL_Event e;
  // TTF_Font *font = NULL;
  SDL_Color white = { 255, 255, 255 };
  renwindow *win = NULL;
  font *VGA = NULL;
  // ttftext *statustext = NULL;
  std::vector<struct anirow *> *fdat = new std::vector<struct anirow *>;
  fdat->push_back(new struct anirow(9, 16, 96));

  if (!initsdl()) {
    win = new renwindow("Text Demo", SCRWIDTH, SCRHEIGH, 0, 0, 0);
    // font = TTF_OpenFont("VGA9.ttf", 16);
    // if (font == NULL)
    //   logger->warn("Couldn't load font: {}", SDL_GetError());
    // statustext = new ttftext(win->getren(), font, &white);
    VGA = new font(win->getren(), "font.gif", fdat);
    double f = 0;
    Uint64 minimumfps = 1000/6;
    Uint64 lastfticks = SDL_GetTicks();
    SDL_Color newcolor = { 255, 255, 255 };

    while (quit != 1) {
      while (SDL_PollEvent(&e) != 0) {
	if (e.type == SDL_QUIT)
	  quit = 1;
      }

      Uint64 now = SDL_GetTicks();
      if (lastfticks < now) {
	Uint64 tdelta = now - lastfticks;
	if (tdelta > minimumfps)
	  tdelta = minimumfps;
	
	SDL_Point *lines = new SDL_Point[30];
	win->renclear();
	for (int i=0;i<=29;i++) {
	  lines[i].x = (sin(f+i*(M_PI/30)) + 1) * (double)315.5;
	  lines[i].y = i * ((double)480/(double)29);
	  if (lines[i].x - 315.5 > 0) {
	    VGA->renderchar(lines[i].x, lines[i].y, '!'+i, 255, 255 * (lines[i].x/(double)631), 0);
	    VGA->renderchar(-lines[i].x + 631, lines[i].y, '!'+i, 0,
			    255 * (lines[i].x/(double)631), 255);
	  } else {
	    VGA->renderchar(-lines[i].x + 631, lines[i].y, '!'+i, 0,
			    255 * (lines[i].x/(double)631), 255);
	    VGA->renderchar(lines[i].x, lines[i].y, '!'+i, 255, 255 * (lines[i].x/(double)631), 0);
	  }
	}
	// SDL_RenderDrawLines(win->getren(), lines, 30);
	win->resetcolor();
	lastfticks = now;
	win->renupdate();
	delete[] lines;
	f += (double)tdelta/(double)1000;
      } else
	SDL_Delay(1);
    }
  }
  delete win;
  delete VGA;
  return 0;
}
