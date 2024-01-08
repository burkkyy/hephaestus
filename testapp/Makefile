CXX := g++
CXXFLAGS := -O2 -Wall -Wextra
CXXINCLUDES := -I ../engine/
LDLIBS := -lhephaestus
LDFLAGS := -L../engine/build/lib/

app.bin: app.cpp
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: clean
clean:
	-rm *.bin *.o build/

