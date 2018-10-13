BIN?=bin

EXT?=exe

all: $(BIN)/rel/abrasive.$(EXT)

SRCS:=\
	src/main.cpp\
	src/audio_sdl.cpp\
	src/display_ogl.cpp\

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

