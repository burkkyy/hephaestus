include config.mk
include version.mk

CXX_INCLUDES := -I../lib/glfw/include -I../lib/glm/glm
LDLIBS := -lvulkan -lglfw3
LDFLAGS := -L../lib/glfw/src

SOURCE_DIRS := core/ vulkan/
SOURCE_FILES := $(foreach _dir,$(SOURCE_DIRS),$(wildcard $(_dir)/*.cpp))
OBJECT_FILES := $(patsubst %.cpp,build/obj/%.o,$(notdir $(SOURCE_FILES)))

HEPHAESTUS_LIBRARY = lib$(HEPHAESTUS_NAME).so
HEPHAESTUS_REAL = $(HEPHAESTUS_LIBRARY).$(HEPHAESTUS_VERSION)
HEPHAESTUS_SONAME = $(HEPHAESTUS_LIBRARY).$(HEPHAESTUS_MAJOR)

default: release

hephaestus: $(SOURCE_DIRS)
	@mkdir -p $(dir build/lib/)
	@for dir in $(SOURCE_DIRS); do $(MAKE) --no-print-directory -C $$dir; done
	$(CXX) $(CXXFLAGS) -shared $(OBJECT_FILES) -Wl,-soname,$(HEPHAESTUS_SONAME) -o build/lib/$(HEPHAESTUS_REAL)
	@# Symbolic link to soname for dynamic linker to use and other for compiling
	ln -sf $(PWD)/build/lib/$(HEPHAESTUS_REAL) $(PWD)/build/lib/$(HEPHAESTUS_SONAME)
	ln -sf $(PWD)/build/lib/$(HEPHAESTUS_SONAME) $(PWD)/build/lib/$(HEPHAESTUS_LIBRARY)

.PHONY: release dev clean
release: CXXFLAGS += -DNDEBUG
release: hephaestus

dev: CXXFLAGS += -ggdb -DDEBUG
dev: hephaestus

clean:
	-rm -r build/

