# Compiler
CXX = mpicxx

# Compiler flags
CXXFLAGS = -Wall -Wextra -O2

# Source file
SRC = Generate.cpp Mat_Mul.cpp Verify.cpp

# Output file
OUT = Generate Mat_Mul Verify

all: $(OUT)

Generate: Generate.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

MatMul: Mat_Mul.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

Verify: Verify.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OUT)
