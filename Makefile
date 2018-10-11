BIN?=bin

all: $(BIN)/abrasive.exe

SRCS:=\
	src/main.cpp

CXXFLAGS+=-g3
LDFLAGS+=-g

$(BIN)/abrasive.exe: $(SRCS:%=$(BIN)/%.o)

#------------------------------------------------------------------------------

clean:
	rm -rf $(BIN)

$(BIN)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o "$@" $^

$(BIN)/%.exe:
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) -o "$@" $^

