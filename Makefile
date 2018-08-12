PROJECT_NAME = webgl2-sdl2-mixed-rendering

SRC_DIR = ./src
BUILD_DIR = ./build

SOURCES := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/sdl/*.cpp)
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
	$(eval LDFLAGS := -O3)
	$(eval TARGETFLAGS := -s DISABLE_EXCEPTION_CATCHING=0 -s USE_WEBGL2=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s SAFE_HEAP=1 -s USE_SDL=2)
	$(eval CXX := em++)
	$(eval CXXFLAGS := -std=c++14 $(LDFLAGS) $(TARGETFLAGS))
	$(eval TARGET := wasm)
	$(eval OUTFILE := index.html)
	
clean:
	rm -rf $(OBJECTS)
	
compile: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(BUILD_DIR)/$(TARGET)/$(OUTFILE)
	rm -rf $^
	
g++: clean set-g++ compile
wasm: clean set-wasm compile

show-vars:
	echo $(SOURCES)
	echo $(OBJECTS)