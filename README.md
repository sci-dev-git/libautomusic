# libautomusic

------
![Build status](https://img.shields.io/badge/libautomusic-blue.svg) ![Build status](https://img.shields.io/badge/build-passing-orange.svg) ![Item status](https://img.shields.io/badge/status-unstable-lightgreen.svg)

This is a C++ library for Algorithmic Musical Composition and automation, which is based on musical theory model and knowledge library system, helping us create multi-track MIDI projects. 

`libautomusic` allows you to create a unique music based on a given `picture` (all styles are acceptable), without any manual configurations or interventions.

# Just few steps to begin with

In Windows you should prepare a MinGW or CygWin environment at first. In unix-like system these tings are all natrual.

libautomusic requires the following libraries to be installed:

* yaml-cpp (at least 0.6.2)

* opencv (at least 3.0)

Then start the terminal and type:

    mkdir build && cd build
    ../configure
    make all
    make install
    
You will get an executable program named 'libautomusic' in the prefix directory you specificed. The following command is to start our first composition by giving a picture.

    ./libautomusic your_image_file_here.jpg
    
And you will get a MIDI file in the same path of the executable, which stores all the notes and voices of your new music.

You can import this file into other MIDI editors for the futher polishing.

# Maintain
We're in maintenance of this project though currently the item is still in a very alpha stage. A bootstrap script is provided to construct building system.
Maintenance-Mode is disabled by default, however, you can enable it if it really needs.
