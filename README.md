# NixCompress #

## Compiling ##

The project is shipped along with a CMake file. You can easily compile from source by building a Makefile for your OS.

You will need cmake in at least version 2.8, but c compiler will do the 
job.

When using the visualization of the huffmann-tree you need to get the 
SimpleDraw executable from the [official 
site](http://www.iem.thm.de/iem/graefe.html). Pleace the executable 
within the same or in the parent path relative to the lz1337 executable.

Visualizations are disabled by default. Open CMakeLists.txt and uncomment (remove the # at the beginning) the cmake command listed below (it sould be at line 7):

    add_definitions(-DVISUALIZATION)

Compile again!

### Linux ###

    git clone https://github.com/YoloSoftDE/NixCompress.git
    cd NixCompress
    cmake .
    make

### Windows ###

Go get cmake and run the CMakeLists.txt file against it.

## Usage ##

You'll get help, when you request for it (lz1337 -h).

    nix-compress (c) 2012-2013 by Florian Zorbach and Sebastian Büttner.
    
    nix-compress -c [-a] archive.nix file1 [file2 [...]]
    nix-compress -x [-a] archive.nix [targetpath]
    
      -a, --archive-file    archive file
      -c, --compress        pack archive
      -l, --list            only list archive contents
      -x, --extract         unpack archive
      -v, --verbose         verbose output
      -h, --help            shows this help message

## Screenshots ##

Normal usage, creation of an archive and listing of its contents.
![Normal usage, creation of an archive and listing of its contents](http://i.imgur.com/RllH7Ra.png "Normal usage, creation of an archive and listing of its contents")

Example of an generation huffman tree (you'll get this when visualizations are enabled at compile time).
![Example of an generation huffman tree](http://i.imgur.com/xra5URK.png "Example of an generation huffman tree")

## License ##

This project has been licensed under the GNU GPLv3. A copy of the license-text should have been shipped along with the code. If not you can find an always up-to-date version at gnu.org - https://www.gnu.org/licenses/gpl-3.0.html.

SimpleDraw is a project by Prof. Dr.-Ing. Martin Gräfe. All files belonging to this project (*/simple_draw.c|.h) are copyrighted by him and excluded from the project license (GPLv3).
