CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra

TARGET = app.bin

default: $(TARGET)

$(TARGET): app.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: run
run: $(TARGET)
	@./$(TARGET)

.PHONY: clean
clean:
	@-rm -r *.bin *.o build/
