CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra
LDFLAGS := -Llib/glfw/src -lglfw3 -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

.PHONY: TARGET
default: TARGET

OBJECT_FILES := build/window.o

build/window.o: src/window.cpp src/window.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -c $< -o $@

include libraries.mk

TARGET: GLFW app.bin
app.bin: app.cpp $(OBJECT_FILES)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) $^ -o $@ $(LDFLAGS)

.PHONY: run
run: $(TARGET)
	@./$(TARGET)

.PHONY: clean
clean:
	@-rm -r *.bin *.o build/
