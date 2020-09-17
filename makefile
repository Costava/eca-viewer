# makefile

CXX:=g++
SRCDIR:=./src
OBJDIR:=./obj

CXXFLAGS=--output $@ -std=c++98 -Wall -g

# For building a dependency of the main executable
BUILDDEP=$(CXX) $(CXXFLAGS) -c $<

# Only needs to be run once after cloning repo
init:
	mkdir -p $(OBJDIR)

run: build
	./main

build: main

main: $(SRCDIR)/main.cpp \
	$(OBJDIR)/EcaWindow.o \
	$(OBJDIR)/IntHorValueSlider.o
	$(CXX) $(CXXFLAGS) -lfltk $^

# The ./ at the beginning of the second line is redundant
#  but imagine if it was not there and $(OBJDIR) resolved to empty string
clean:
	rm -f ./main
	rm -f ./$(OBJDIR)/*

################################################################################

$(OBJDIR)/EcaWindow.o: $(SRCDIR)/EcaWindow.cpp $(SRCDIR)/EcaWindow.hpp
	$(BUILDDEP)

$(OBJDIR)/IntHorValueSlider.o: $(SRCDIR)/IntHorValueSlider.cpp $(SRCDIR)/IntHorValueSlider.hpp
	$(BUILDDEP)
