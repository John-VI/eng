IDIR=includes
CXX=g++
CXXFLAGS=-I$(IDIR) -ggdb -Wall -pedantic -fdiagnostics-color=always -Dmain=SDL_main -std=gnu++20 
LIBS=-LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows -lpthread statlibs/libspdlog.a -static-libgcc -static-libstdc++
DEPS=eng.h
OBJ=eng.o

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) 

sticktest.exe: $(OBJ) sticktest.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

animationtest.exe: $(OBJ) animationtest.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

textdemo.exe: $(OBJ) textdemo.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

rogue.exe: $(OBJ) roguemain.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
