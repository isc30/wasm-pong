PROJECT_NAME = webgl2-sdl2-pong

SRC_DIR = ./src
BUILD_DIR = ./build

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SOURCES := $(call rwildcard,$(SRC_DIR),*.cpp)
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))

all:
	$(error Available targets: clang, wasm)

set-g++:
	$(eval LDFLAGS := -lstdc++)
	$(eval TARGETFLAGS := )
	$(eval CXX := g++ -x c++)
	$(eval CXXFLAGS := -std=c++11 $(LDFLAGS) $(TARGETFLAGS))
	$(eval TARGET := g++)
	$(eval OUTFILE := $(PROJECT_NAME).exe)
	
set-wasm:
	$(eval LIBRARIES := -I ./externals/glm)
	$(eval LDFLAGS := -O3 -s USE_WEBGL2=1 -s USE_SDL=2)
	$(eval WARNINGS := -Wall -Wextra -Wwrite-strings -Werror -Wno-unused-parameter -Wno-unused-variable)
	$(eval TARGETFLAGS := -s DISABLE_EXCEPTION_CATCHING=0 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s SAFE_HEAP=1)
	$(eval CXX := em++)
	$(eval CXXFLAGS := -std=c++14 $(WARNINGS) $(LDFLAGS) $(TARGETFLAGS) $(LIBRARIES) -I $(SRC_DIR))
	$(eval TARGET := wasm)
	$(eval OUTFILE := index.js)
	
clean:
	rm -rf $(OBJECTS)
	
compile: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/$(TARGET)/$(OUTFILE)
	rm -rf $^
	
g++: clean set-g++ compile
wasm: clean set-wasm compile

show-vars:
	echo $(CODEFILES)
	echo $(SOURCES)
	echo $(OBJECTS)