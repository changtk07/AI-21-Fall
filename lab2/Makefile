SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,src/%.o,$(SOURCES))
DEPENDS := $(patsubst src/%.cpp,src/%.d,$(SOURCES))

WARNING := -Wall -Wextra
STD := -std=gnu++11

.PHONY: all clean

all: lab2

clean:
	$(RM) $(OBJECTS) $(DEPENDS) lab2

lab2: $(OBJECTS)
	$(CXX) $(WARNING) $(CXXFLAGS) $^ -o $@

-include $(DEPENDS)

%.o: %.cpp Makefile
	$(CXX) $(WARNING) $(STD) $(CXXFLAGS) -MMD -MP -c $< -o $@
