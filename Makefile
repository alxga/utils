LIB =
CFLAGS = -O2 -fpic -std=gnu++0x -I..

ifdef HAVE_MPI
  CC = mpicxx
  CFLAGS += -DHAVE_MPI
else
  CC = g++
endif

ifdef BOOST_INC
  CFLAGS += -DHAVE_BOOST -I$(BOOST_INC)
endif

HEADERS = app.h distr.h dpoint.h except.h fsutils.h libutils.h math.h \
          matrix.h mpimsg.h mseccnt.h rand01.h rselect.h stdafx.h strutils.h \
          utils.h

SRC = app.cpp distr.cpp dpoint.cpp except.cpp fsutils.cpp math.cpp mseccnt.cpp \
      rand01.cpp strutils.cpp utils.cpp

OBJ = app.o distr.o dpoint.o except.o fsutils.o math.o mseccnt.o \
      rand01.o strutils.o utils.o

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libutils.so : $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB)
	rm $(OBJ)
	cp $@ ../so

clean:
	rm -Rf $(OBJ) libutils.so *~

