SOURCES := src/main.cpp
OBJECTS := build/*.o
DEPENDS := build/*.d

WARNING := -Wall -Wextra
STD := -std=c++11

.PHONY: all clean

all: build/lab3

clean:
	$(RM) $(OBJECTS) $(DEPENDS) build/lab3

build/lab3: $(OBJECTS)
	$(CXX) $(WARNING) $(CXXFLAGS) $^ -o $@

-include $(DEPENDS)

build/%.o: src/%.cpp Makefile
	$(CXX) $(WARNING) $(STD) $(CXXFLAGS) -MMD -MP -c $< -o $@
