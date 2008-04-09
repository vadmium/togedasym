
#_DEBUG=1
#_PARSERDEBUG=1

ifdef _DEBUG
  ifdef _PARSERDEBUG
    FLEXDBG=-d -b
    BIZONDBG=--debug -v
  endif
  DEBUGCFLAGS=-g -D_DEBUG
endif

CC=gcc
BIZON=bison
FLEX=flex

InputName=PfWSchLib

ifdef _DEBUG
  DEBUGLIBS=-ldmalloc
endif

all : static

clean:
	rm -f *.o
	rm -f *.so*
	rm -f *.a
	rm -f *.y.c
	rm -f *.y.h
	rm -f *.l.c
	rm -f *~
	rm -f parsertest
	rm -f sparsertest

static : libPfWSchLibParser.a

shared: libPfWSchLibParser.so.1 testprogram

libPfWSchLibParser.a :  PfWSchLibParser.o PfWSchLib.y.o PfWSchLib.l.o 
		ar rcs  libPfWSchLibParser.a PfWSchLibParser.o PfWSchLib.l.o PfWSchLib.y.o

#Tu jeszcze mo¿na dodaæ visibility
libPfWSchLibParser.so.1 :  PfWSchLibParser.o PfWSchLib.y.o PfWSchLib.l.o 
		$(CC) -shared -Wl,-soname,libPfWSchLibParser.so.1 -o libPfWSchLibParser.so.1 PfWSchLibParser.o PfWSchLib.l.o PfWSchLib.y.o
		ln -f -s  libPfWSchLibParser.so.1  libPfWSchLibParser.so
		ln -f -s  libPfWSchLibParser.so.1  libPfWSchLibParser.so.1.0

$(InputName).y.c $(InputName).y.h : $(InputName).y PfWSchLibStruct.h
		$(BIZON) $(BIZONDBG) -p$(InputName) -o$(InputName).y.c --defines=$(InputName).y.h $(InputName).y

$(InputName).l.c : $(InputName).l PfWSchLibStruct.h
		$(FLEX) $(FLEXDBG) -P$(InputName) -t $(InputName).l > $(InputName).l.c

$(InputName).l.o : $(InputName).l.c 
		$(CC) -O2 $(DEBUGCFLAGS) -fPIC -c $(InputName).l.c -o $(InputName).l.o

$(InputName).y.o : $(InputName).y.c
		$(CC) -O2  $(DEBUGCFLAGS) -fPIC -c $(InputName).y.c -o $(InputName).y.o

PfWSchLibParser.o : PfWSchLibParser.c PfWSchLib.y.h PfWSchLibStruct.h
		$(CC) -O2 $(DEBUGCFLAGS) -fPIC -c PfWSchLibParser.c -o PfWSchLibParser.o

testprogram : parsertest.c PfWSchLibStruct.h
	$(CC)   -O2 $(DEBUGCFLAGS)  parsertest.c -o parsertest -Wl,-rpath,. -L. -lPfSchWLibParser $(DEBUGLIBS)
	chmod 755 parsertest

testprogramstatic : parsertest.c PfWSchLibStruct.h libPfWSchLibParser.a
	$(CC)  -O2 $(DEBUGCFLAGS) parsertest.c -o sparsertest libPfWSchLibParser.a $(DEBUGLIBS)
	chmod 755 sparsertest
