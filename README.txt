Pro*el schematic ascii library to gEDA importer
2008.04.09 19:47

{{{ INTRODUCTION
    ------------
  This program is designed to import ascii exported pro*el schematic library
  to gEDA schematic format. Documentation is short, but precise and can be used
  to write converters for other formats. 
  It contains:
    - simple code "parsertest" in PfWSchLibParser directory to test parser
    - "togedasym" to use to convert ascii exported pro*el schematic 
      library to gEDA schematic symbol.

  What togedasym can and cannot do:
    - doesn't create single component from multipart component,
      parts of multipart component are written to separate files
      and have to be combine to single part (or stay as they are)
    - doesn't write power nets,
      power pins appear in symbols and after conversion they should be manually
      removed and new power net has to be created,
    - doesn't draw negation lines above pins' names (they should be drawn manually)
    - dash and dot attributes are ignored, as they don't look good 
      in pro*el final prints; primitives with these attributes
      are drawn as continued lines,
    - color attributes are ignored,
      primitives are in standard gschem colors

  Refer to togedasym options in order to select possible modifications
  and final appearance of the components or to modify the source.
}}}

{{{ QUICK START
    -----------
    To convert your pro*el schematic ascii library to gEDA symbols
    copy "togedasym" executable from tarball to any place and run:

	$ togedasym <PfwSchLibAsciiExportedFile>

    or
 	$ togedasym --help

	to read about possible options

    Before using symbols in design, open them in gschem and translate to correct
    position (menu Edit/Symbol_Translate with 0 offset or simple et0<Enter>)
    or use any script for global translation. Pieces of symbols with negative
    coordinates will be placed outside of the workspace, but translation will
    correct this.
}}}

{{{ COMPILING
    ---------
  This program was compiled with:
    - bison-2.3-4
    - flex-2.5.33-14 (for lower version small changes should be do)
    - gcc-4.1.2-33
    - libgcc-4.1.2-33
    (all programs are available as sources or packages
     ready to install in most distributions)

  To compile togedasym:
  run Linux, unpack tarbool and in togedasym directory run:

    $ make clean
	for remove binary
    $ make
	for make togedasym program

  To compile parser only:
  run Linux, unpack tarbool and in PfwSchLibParser directory run:

    $ make

  It contain example "parsertest" code in PfWSchLibParser directory,
  to test pro*el schematic ascii library parser. It list library.

  Intention of this code is to help users design on free platform.
  That is why this software hasn't been compiled on other systems than Linux.
}}}

