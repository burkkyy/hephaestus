include ../config.mk

SOURCE_FILES := $(wildcard *.cpp)
HEADER_FILES := $(wildcard *.hpp)
OBJECT_FILES := $(patsubst %.cpp,../build/obj/%.o,$(notdir $(SOURCE_FILES)))

all: $(OBJECT_FILES)

$(OBJECT_FILES): $(SOURCE_FILES) $(HEADER_FILES)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $(patsubst %.o,%.cpp,$(notdir $@)) -o $@

