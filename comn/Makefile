CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 $(INCLUDES)
AR = ar

SOURCE = $(wildcard src/*.cpp)
OBJS =	$(SOURCE:.cpp=.o)


INCLUDES = -I. -I./include -I./src
LIBS =

DIR_LIB = ./lib
DIR_DIST = ./dist

TARGET = $(DIR_LIB)/libcomn.a

.PHONY: all dist clean distclean test doc dir

	
all: dir $(TARGET) 
	

$(TARGET):	$(OBJS)
	#$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	$(AR) -rcs $(TARGET) $(OBJS)

	
clean:
	rm -f $(OBJS) $(TARGET)

dir:
	-mkdir $(DIR_LIB)

dist: all
	-mkdir -p $(DIR_DIST)/comn/include
	-mkdir -p $(DIR_DIST)/comn/lib
	-cp ./include/*.h $(DIR_DIST)/comn/include
	-cp $(TARGET) $(DIR_DIST)/comn/lib
	
distclean:
	rm -rf $(DIR_DIST)
