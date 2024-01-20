include config.mk
include version.mk

SOURCE_DIRS := vulkan/ core/
SOURCE_FILES := $(foreach _dir,$(SOURCE_DIRS),$(wildcard $(_dir)/*.cpp))
OBJECT_FILES := $(patsubst %.cpp,build/obj/%.o,$(notdir $(SOURCE_FILES)))

HEPHAESTUS_LIBRARY = lib$(HEPHAESTUS_NAME).so
HEPHAESTUS_REAL = $(HEPHAESTUS_LIBRARY).$(HEPHAESTUS_VERSION)
HEPHAESTUS_SONAME = $(HEPHAESTUS_LIBRARY).$(HEPHAESTUS_MAJOR)

default: release

hephaestus: $(SOURCE_DIRS)
	@mkdir -p $(dir build/lib/)
	@for dir in $(SOURCE_DIRS); do $(MAKE) --no-print-directory -C $$dir; done
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) -shared $(OBJECT_FILES) -Wl,-soname,$(HEPHAESTUS_SONAME) -o build/lib/$(HEPHAESTUS_REAL) $(LDLIBS)
	@# Symbolic link to soname for dynamic linker to use and other for compiling
	ln -sf $(shell pwd)/build/lib/$(HEPHAESTUS_REAL) $(shell pwd)/build/lib/$(HEPHAESTUS_SONAME)
	ln -sf $(shell pwd)/build/lib/$(HEPHAESTUS_SONAME) $(shell pwd)/build/lib/$(HEPHAESTUS_LIBRARY)

.PHONY: release dev test clean
release: CXXFLAGS += -DNDEBUG
release: hephaestus

dev: CXXFLAGS += -ggdb -DDEBUG
dev: hephaestus

test: hephaestus
	$(MAKE) --no-print-directory -C test/
	./test/build/test.bin

clean:
	-rm -r build/

