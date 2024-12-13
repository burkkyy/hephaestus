CXX := g++
CXXFLAGS := -O2 -Wall -Wextra -ggdb
CXXINCLUDES := -I../
LDLIBS := -lhephaestus
LDFLAGS := -L../build/lib/

TARGET := app.bin

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: run
run: $(TARGET)
	@./$(TARGET)

.PHONY: clean
clean:
	-rm *.bin

