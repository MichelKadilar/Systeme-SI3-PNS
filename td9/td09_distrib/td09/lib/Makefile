#---------------------------------------------------------------------------------
# Variables
#---------------------------------------------------------------------------------

JAVA_H=/usr/lib/jvm/java-17-openjdk-amd64

# Compilateur et options
# ----------------------

CC=g++
CXXFLAGS=-fpic -Wall -I$(JAVA_H)/include/ -I$(JAVA_H)/include/linux

TARGET=libHelloWorld.so libGetPid.so
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
H_FILE=$(SRC:.cpp=.h)

#---------------------------------------------------------------------------------
# Cibles
#---------------------------------------------------------------------------------
all: $(TARGET)

lib%.so: %.o
	$(CC) -Wl,-soname,$@ -shared -o $@ $<

%.o: %.cpp %.h
	$(CC) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(TARGET) $(OBJ) \#* *~
