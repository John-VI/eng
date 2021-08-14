#ifndef MESSAGING_H
#define MESSAGING_H

#include <SDL2/SDL.h>
#include "eng.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

enum MSGLEVEL { DDEBUG, DINFO, DWARN, DERR, DCRITICAL, DFATAL, MESSAGE, WARN, DANGER, CRITICAL,
		FATAL, LEVELS };

struct message {
  const char *msgtext;
  MSGLEVEL level;
  COLOR altcolor = DEFAULT;
  // ~message();
};

template <class T>
void pushstack(T *stack[], T *newentry, int size);

template <class T>
void invert(T *sequence, unsigned int size);

class messager {
public:
  messager(SDL_Renderer *renderer, std::shared_ptr<spdlog::logger> log, unsigned int size, 
	   font *fontface, SDL_Color *colorpalette, MSGLEVEL mesglevel = MESSAGE);
  void send(const char *message, MSGLEVEL level = MESSAGE, COLOR altcolor = DEFAULT);
  void send(struct message *message);
  void rendercappedmessages(unsigned int from, SDL_Rect *box);
  void setlevelcolor(MSGLEVEL level, COLOR color);
  uint16_t flags = 0;
  MSGLEVEL level;
  ~messager();
protected:
  std::shared_ptr<spdlog::logger> logger;
  struct message **queue;
  SDL_Renderer *ren;
  unsigned int queuesize;
  font *face;
  SDL_Color *palette;
  unsigned int index[LEVELS];
};

#endif
