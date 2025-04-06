CXX = g++
CXXFLAGS = -O3 -march=native -Wall -Wextra

all: calculator_opt calculator_unopt

calculator_opt: high_util.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

calculator_unopt: low_util.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f calculator_unopt calculator_unopt

.PHONY: all clean