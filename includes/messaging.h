#ifndef MESSAGING_H
#define MESSAGING_H

#include <SDL2/SDL.h>
#include "eng.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

enum MSGLEVEL { DEBUG, INFO, MESSAGE, WARN, ERR, CRITICAL, FATAL, LEVELS };

struct message {
  std::string msgtext;
  MSGLEVEL level;
  SDL_Color *altcolor = NULL;
};
  
template <class T>
void pushstack(T *stack[], T *newentry, int size);

template <class T>
void invert(T *sequence, unsigned int size);

class messager {
public:
  messager(std::shared_ptr<spdlog::logger> log, unsigned int size, font *fontface, );
  void send(std::string message, MSGLEVEL level);
  void send(struct message message);
  void rendermessages(unsigned int from, int x, int y, unsigned int width, unsigned int height);
  uint16_t flags = 0;
  MSGLEVEL level = DEBUG;
protected:
  std::shared_ptr<spdlog::logger> logger;
  std::string *queue;
  unsigned int queuesize;
  font *face;
  SDL_Color *palette;
  unsigned int index[LEVELS];
};

#endif
