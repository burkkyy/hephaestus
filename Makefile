CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra
LDFLAGS := -Llib/glfw/src -lglfw3 -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

TARGET = app.bin
default: $(TARGET)

include dependencies.mk

$(TARGET): app.cpp GLFW
	$(CXX) $(CXXFLAGS) -I$(GLFW_INCLUDE) -I$(GLM_INCLUDE) $< -o $@ $(LDFLAGS)

.PHONY: run
run: $(TARGET)
	@./$(TARGET)

.PHONY: clean
clean:
	@-rm -r *.bin *.o build/
