# libautomusic

------
![Build status](https://img.shields.io/badge/libautomusic-blue.svg) ![Build status](https://img.shields.io/badge/build-passing-orange.svg) ![Item status](https://img.shields.io/badge/status-unstable-lightgreen.svg)

This is a C++ library for Algorithmic Musical Composition and automation, which is based on musical theory model and knowledge library system, helping us create multi-track MIDI projects. 

`libautomusic` allows you to create a unique music based on a given `picture` (all styles are acceptable), without any manual configurations or interventions.

# Quick start

In Windows you should prepare a MinGW or CygWin environment at first. In unix-like system these facilities are all natrual for you.

Start a terminal:

    mkdir build && cd build
    ../configure
    make all
    make install

If things go right you will get a program named 'libautomusic' in your prefix directory. The following command is to start our first composition process by merely giving a picture.

    ./libautomusic your_image_file_here.jpg

And you will get a MIDI file in the same path of the executable, which stores all the notes and voices of your new music.

For the futher polishing, you may want to import this file to other MIDI edition softwares.

# Third-party libraries

libautomusic requires the following libraries to be installed (if all the corresponding fetures are enabled):

* yaml-cpp (at least 0.6.2)

* opencv (at least 3.0) (optional)

* libmp3lame (at least 3.1) (optional)


# Maintain
We're in maintenance of this project though currently the item is still in a very alpha stage. A bootstrap script is provided to construct building system.
Maintenance-Mode is disabled by default, however, you can enable it if it really needs.
