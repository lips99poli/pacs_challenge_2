CXX      ?= g++
CXXFLAGS ?= -std=c++20
CPPFLAGS ?= -O3 -Wall -I"../include"
LINK.o := $(LINK.cc) 

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
HEADERS = $(wildcard *.hpp)

exe_sources = $(filter main.cpp, $(SRCS))
EXEC = $(exe_sources:.cpp=)

.PHONY: all clean distclean

all: $(EXEC)

$(EXEC): $(OBJS)

$(OBJS): $(SRCS) $(HEADERS)

clean:
	-\rm -f $(EXEC) $(OBJS)

distclean: clean
	$(RM) -f ./doc $(DEPEND)
	$(RM) *.out *.bak *~