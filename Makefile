CXX := g++
CXXFLAGS :=-std=c++17 -O2 -Wall -Wextra
CXX_INCLUDES := -I./external/glfw/include -I./external/glm/glm
LDLIBS :=-lvulkan -lglfw3
LDFLAGS :=-L./external/glfw/src

app.bin: app.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: clean
clean:
	-rm app.bin

