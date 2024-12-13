CXX := g++
CXXFLAGS := -O2 -Wall -Wextra
CXXINCLUDES := -I../engine/ -I../engine/include/
LDLIBS := -lhephaestus
LDFLAGS := -L../engine/build/lib/

TARGET := build/app.bin

all: $(TARGET)

.PHONY: $(TARGET)
$(TARGET): app.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: run
run: $(TARGET)
	@./$(TARGET)

.PHONY: clean
clean:
	-rm -r *.bin *.o build/

