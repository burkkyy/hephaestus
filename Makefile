CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra
LDFLAGS := -Llib/glfw/src -lglfw3 -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

.PHONY: TARGET 
default: TARGET

OBJECT_FILES := build/window.o build/device.o build/swapchain.o build/pipeline.o build/renderer.o build/model.o build/render_system.o build/engine.o

build/engine.o: src/engine.cpp src/engine.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/render_system.o: src/render_system.cpp src/render_system.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/model.o: src/model.cpp src/model.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/renderer.o: src/renderer.cpp src/renderer.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/pipeline.o: src/pipeline.cpp src/pipeline.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/swapchain.o: src/swapchain.cpp src/swapchain.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/device.o: src/device.cpp src/device.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

build/window.o: src/window.cpp src/window.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) -c $< -o $@

vertexShaderSources = $(shell find ./shaders -type f -name "*.vert")
vertexShaderObjectFiles = $(patsubst %.vert, %.vert.spv, $(vertexShaderSources))
fragmentShaderSources = $(shell find ./shaders -type f -name "*.frag")
fragmentShaderObjectFiles = $(patsubst %.frag, %.frag.spv, $(fragmentShaderSources))

%.spv: %
	glslc $< -o $@

include libraries.mk

TARGET: GLFW app.bin $(vertexShaderObjectFiles) $(fragmentShaderObjectFiles)

app.bin: app.cpp $(OBJECT_FILES)
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) app.cpp $(OBJECT_FILES) -o $@ $(LDFLAGS)

.PHONY: test
test: TARGET
	@./app.bin

.PHONY: clean
clean:
	@-rm -r app.bin 2>/dev/null 
	@-rm -r build/ 2>/dev/null 
	@-rm shaders/*.spv 2>/dev/null 
