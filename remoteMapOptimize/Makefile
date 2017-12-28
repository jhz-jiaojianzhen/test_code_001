
DIR_INC = -I./include -I/usr/include/python2.7
DIR_INC_PATH = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_LIB = ./lib

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

#CROSE_COMPILE := arm-arago-linux-gnueabi
CC          := gcc 
CXX         := g++ 
CFLAGS 		:= -fPIC -shared -g -Wall  ${DIR_INC}
STRIP       := strip
RM          := -rm -f
LIBS        := -lpython2.7 -lboost_python

TARGET = mapOptimize.so
 
BIN_TARGET = ${DIR_LIB}/${TARGET}
 
${BIN_TARGET}:${OBJ}
	$(CXX) $(CFLAGS) $(OBJ) $(LIBS) -o $@
	$(STRIP) ${DIR_LIB}/*
	   
${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm $(DIR_OBJ)/*.o -rf; rm $(DIR_LIB)/$(TARGET) -rf
