CC=gcc
CXX=g++
RM=rm -f
 
CPPFLAGS=-g -std=c++03 -O3 -I eigen3 $(shell pkg-config Magick++ --cflags)
LDFLAGS=-g $(shell pkg-config Magick++ --libs)
 
SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
 
.PHONY: all clean profile
 
all: as2
 
as2: $(OBJS)
	$(CXX) -o as2 $(OBJS) $(LDFLAGS)
 
clean:
	$(RM) as2 *.o *.d callgrind.out*
 
-include $(OBJS:.o=.d)
 
%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $*.cpp -o $*.o
	$(CXX) -MM $(CPPFLAGS) $*.cpp > $*.d
 
profile: as2
	valgrind --tool=callgrind ./as2



