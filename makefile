# makefile

CXX:=g++
SRCDIR:=./src
OBJDIR:=./obj

CXXFLAGS=--output $@ -std=c++98 -Wall -g

# For building a dependency of the main executable
BUILDDEP=$(CXX) $(CXXFLAGS) -c $<

# Only needs to be run once after cloning repo
# Not currently needed because nothing is being built to obj folder
# init:
# 	mkdir -p $(OBJDIR)

run: build
	./main

build: main

main: $(SRCDIR)/main.cpp
	$(CXX) $(CXXFLAGS) -lfltk $^

# The ./ at the beginning of the second line is redundant
#  but imagine if it was not there and $(OBJDIR) resolved to empty string
clean:
	rm -f ./main
	rm -f ./$(OBJDIR)/*

################################################################################

# Example to incrementally build a dependency
# $(OBJDIR)/Button.o: $(SRCDIR)/Button.cpp $(SRCDIR)/Button.hpp
# 	$(BUILDDEP)
