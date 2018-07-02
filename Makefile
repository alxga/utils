LIB =
CFLAGS = -O2 -fpic -std=gnu++0x -I.. -I/home/aag2k/usr/include

CC = mpicxx

HEADERS = app.h dpoint.h except.h fsutils.h libutils.h mpimsg.h rand01.h \
          rselect.h stdafx.h strutils.h utils.h

SRC = app.cpp dpoint.cpp except.cpp fsutils.cpp rand01.cpp strutils.cpp \
      utils.cpp

OBJ = app.o dpoint.o except.o fsutils.o rand01.o strutils.o utils.o

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libutils.so : $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB)
	rm $(OBJ)
	cp $@ ../so

clean:
	rm -Rf $(OBJ) libutils.so *~

