# NixCompress #

## Compiling ##

The project is shipped along with a CMake file. You can easily compile from source by building a Makefile for your OS.


You will need at least version 2.8. Any c compiler will do the job.

When using the visualization of the huffmann-tree you need to get the SimpleDraw executable from the [official site](http://www.iem.thm.de/iem/graefe.html). Pleace the executable within the same or in the parent path relative to the nc executable.

### Linux ###

    git clone https://github.com/YoloSoftDE/NixCompress.git
    cd NixCompress
    cmake .
    make

### Windows ###

Go get cmake and run the CMakeLists.txt file against it.

## License ##

This project has been licensed under the GNU GPLv3. A copy of the license-text should have been shipped along with the code. If not you can find an always up-to-date version at gnu.org - https://www.gnu.org/licenses/gpl-3.0.html.

SimpleDraw is a project by Prof. Dr.-Ing. Martin Gräfe. All files belonging to this project (*/simple_draw.c|.h) are copyrighted by him and excluded from the project license (GPLv3).
