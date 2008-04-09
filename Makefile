
CC=gcc

all: togedasym

togedasym : togedasym.c PfWSchLibParser/libPfWSchLibParser.a
	$(CC) togedasym.c -o togedasym \
	-IPfWSchLibParser PfWSchLibParser/libPfWSchLibParser.a  -lm 
	chmod 755 togedasym

clean:
	rm -f togedasym

PfWSchLibParser/libPfWSchLibParser.a : lib

lib: 
	(cd ./PfWSchLibParser && make static )

cleanlib:
	( cd PfWSchLibParser && make clean )

