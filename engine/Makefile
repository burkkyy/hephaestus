include config.mk
include version.mk

SOURCE_DIRS := vulkan_plugin/ core/
SOURCE_FILES := $(foreach _dir,$(SOURCE_DIRS),$(wildcard $(_dir)/*.cpp))
OBJECT_FILES := $(patsubst %.cpp,build/%.o,$(notdir $(SOURCE_FILES)))

HEPHAESTUS_LIBRARY = lib$(HEPHAESTUS_NAME).so
HEPHAESTUS_REAL = $(HEPHAESTUS_LIBRARY).$(HEPHAESTUS_VERSION)
HEPHAESTUS_SONAME = $(HEPHAESTUS_LIBRARY).$(HEPHAESTUS_MAJOR)

ENTRY = build/hephaestus.o
OBJECT_FILES += $(ENTRY)

default: release

$(ENTRY): hephaestus.cpp hephaestus.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: release
release: clean $(ENTRY)
	@mkdir -p $(dir build/lib/)
	@for dir in $(SOURCE_DIRS); do $(MAKE) --no-print-directory -C $$dir release; done
	
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) -shared $(OBJECT_FILES) -Wl,-soname,$(HEPHAESTUS_SONAME) -o build/lib/$(HEPHAESTUS_REAL) $(LDLIBS)
	@# Symbolic link to soname for dynamic linker to use and other for compiling
	@ln -sf $(shell pwd)/build/lib/$(HEPHAESTUS_REAL) $(shell pwd)/build/lib/$(HEPHAESTUS_SONAME)
	@ln -sf $(shell pwd)/build/lib/$(HEPHAESTUS_SONAME) $(shell pwd)/build/lib/$(HEPHAESTUS_LIBRARY)

.PHONY: dev
dev: CXXFLAGS += -ggdb
dev: clean $(ENTRY)
	@mkdir -p $(dir build/lib/)
	@for dir in $(SOURCE_DIRS); do $(MAKE) --no-print-directory -C $$dir dev; done
	
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) -shared $(OBJECT_FILES) -Wl,-soname,$(HEPHAESTUS_SONAME) -o build/lib/$(HEPHAESTUS_REAL) $(LDLIBS)
	# Symbolic link to soname for dynamic linker to use and other for compiling
	ln -sf $(shell pwd)/build/lib/$(HEPHAESTUS_REAL) $(shell pwd)/build/lib/$(HEPHAESTUS_SONAME)
	ln -sf $(shell pwd)/build/lib/$(HEPHAESTUS_SONAME) $(shell pwd)/build/lib/$(HEPHAESTUS_LIBRARY)

.PHONY: debug
debug: dev
	$(MAKE) --no-print-directory -C debug/ run

.PHONY: test
test: release
	@$(MAKE) --no-print-directory -C test/
	@./test/build/test.bin

.PHONY: clean
clean:
	-rm -r build/

