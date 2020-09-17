# makefile

CXX:=g++
SRCDIR:=./src
OBJDIR:=./obj

CXXFLAGS=--output $@ -std=c++98 -Wall -g

# For building a dependency of the main executable
BUILDDEP=$(CXX) $(CXXFLAGS) -c $<

# Only needs to be run once
# Not currently needed (no dependencies to incrementally build)
# init:
# 	mkdir -p $(OBJDIR)

run: build
	./main

runfresh: clean build
	./main

build: main

buildfresh: clean build

main: $(SRCDIR)/main.cpp
	$(CXX) $(CXXFLAGS) -lfltk $^

clean:
	rm -f ./main
	# The ./ at the beginning is redundant but imagine if it was not there
	#  and $(OBJDIR) resolved to empty string
	# rm -f ./$(OBJDIR)/*

# Example for incrementally building a dependency of the main executable
# $(OBJDIR)/StackPanel.o: $(SRCDIR)/StackPanel.cpp $(SRCDIR)/StackPanel.hpp
# 	$(BUILDDEP)
