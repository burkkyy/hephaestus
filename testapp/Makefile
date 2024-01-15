CXX := g++
CXXFLAGS := -O2 -Wall -Wextra
CXXINCLUDES := -I../engine/ -I../engine/include/
LDLIBS := -lhephaestus
LDFLAGS := -L../engine/build/lib/

TARGET := build/app.bin

$(TARGET): app.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: run clean
run: $(TARGET)
	@./$(TARGET)

clean:
	-rm -r *.bin *.o build/

