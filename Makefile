LIB =
WARNS = -Wno-unused-result -Wno-format-overflow
CFLAGS = -fpic -std=gnu++0x -I.. $(WARNS)

ifneq ($(VSCFG), LinuxRelease)
  CFLAGS += -g
else
  CFLAGS += -O2
endif

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
          unionfnd.h utils.h

SRC = app.cpp distr.cpp dpoint.cpp except.cpp fsutils.cpp math.cpp mseccnt.cpp \
      rand01.cpp strutils.cpp unionfnd.cpp utils.cpp

OBJ = app.o distr.o dpoint.o except.o fsutils.o math.o mseccnt.o \
      rand01.o strutils.o unionfnd.o utils.o

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libutils.so : $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB)
#	rm $(OBJ)
	cp $@ ../so

clean:
	rm -Rf $(OBJ) libutils.so *~

