CXX := g++
CXXFLAGS := -fpic -std=c++17 -O2 -Wall -Wextra -I../
CXXINCLUDES := -I../lib/glfw/include -I../lib/glm/
LDLIBS := -lvulkan -lglfw3
LDFLAGS := -L../lib/glfw/src

