GLM_INCLUDE = lib/glm

GLFW_SRC = lib/glfw
GLFW_INCLUDE = lib/glfw/include

.PHONY: GLFW
GLFW: lib/glfw/src/libglfw3.a

lib/glfw/src/libglfw3.a:
	cd $(GLFW_SRC) && \
	cmake -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF .
	cd $(GLFW_SRC) && make
