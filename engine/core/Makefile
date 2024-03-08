include ../config.mk

SOURCE_FILES := $(wildcard *.cpp)
HEADER_FILES := $(wildcard *.hpp)
OBJECT_FILES := $(patsubst %.cpp,../build/%.o,$(notdir $(SOURCE_FILES)))

default: release

$(OBJECT_FILES): $(SOURCE_FILES) $(HEADER_FILES)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $(patsubst %.o,%.cpp,$(notdir $@)) -o $@

.PHONY: all
all: $(OBJECT_FILES)

.PHONY: release
release: CXXFLAGS += -DNDEBUG
release: all

.PHONY: dev
dev: CXXFLAGS += -ggdb
dev: all

