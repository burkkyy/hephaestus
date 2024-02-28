CXX := g++
CXXFLAGS := -O2 -Wall -Wextra -DNDEBUG
CXXINCLUDES := -I../
LDLIBS := -lhephaestus
LDFLAGS := -L../build/lib/

TARGET := build/test.bin

$(TARGET): test.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: run clean
run: $(TARGET)
	@./$(TARGET)

clean:
	-rm -r *.bin *.o build/

