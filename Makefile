BIN?=bin

EXT?=exe

all: $(BIN)/rel/abrasive.$(EXT)

CXXFLAGS+=-Wall -Wextra -Werror

SRCS:=\
	src/main.cpp\
	src/mesh.cpp\
	src/file.cpp\
	src/scene.cpp\
	src/audio_sdl.cpp\
	src/display_ogl.cpp\

# Scenes
SRCS+=src/scene_init.cpp
SRCS+=src/scene_bass.cpp

CXXFLAGS+=$(shell pkg-config gl sdl2 --cflags)
LDFLAGS+=$(shell pkg-config gl sdl2 --libs)

CXXFLAGS+=-g3
LDFLAGS+=-g

$(BIN)/rel/abrasive.$(EXT): $(SRCS:%=$(BIN)/%.o)

#------------------------------------------------------------------------------

clean:
	rm -rf $(BIN)

$(BIN)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -MM -MP -MT "$@" -o "$(BIN)/$*.dep"
	$(CXX) $(CXXFLAGS) -c -o "$@" $<

$(BIN)/%.exe:
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) -o "$@" $^

include $(shell test -d $(BIN) && find $(BIN) -name '*.dep')

