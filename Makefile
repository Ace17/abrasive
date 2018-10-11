BIN?=bin

EXT?=exe

all: $(BIN)/rel/abrasive.$(EXT)

SRCS:=\
	src/main.cpp

CXXFLAGS+=$(shell pkg-config sdl2 --cflags)
LDFLAGS+=$(shell pkg-config sdl2 --libs)

CXXFLAGS+=-g3
LDFLAGS+=-g

$(BIN)/rel/abrasive.$(EXT): $(SRCS:%=$(BIN)/%.o)

#------------------------------------------------------------------------------

clean:
	rm -rf $(BIN)

$(BIN)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o "$@" $^

$(BIN)/%.exe:
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) -o "$@" $^

