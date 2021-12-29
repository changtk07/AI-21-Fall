WARNING := -Wall -Wextra
STD := -std=c++11

.PHONY: all clean

all: build/knn build/kmeans

clean:
	$(RM) build/*.o build/*.d build/knn build/kmeans

build/knn: build/knn.o
	$(CXX) $(WARNING) $(CXXFLAGS) $^ -o $@

build/kmeans: build/kmeans.o
	$(CXX) $(WARNING) $(CXXFLAGS) $^ -o $@

-include build/knn.d build/kmeans.d

build/%.o: src/%.cpp Makefile
	$(CXX) $(WARNING) $(STD) $(CXXFLAGS) -MMD -MP -c $< -o $@
