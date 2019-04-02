# libautomusic

------
![passing](https://camo.githubusercontent.com/948ddd4d1b81323800104755c8ed392d5410f5e2/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f6275696c642d70617373696e672d677265656e2e737667) ![unstable](https://camo.githubusercontent.com/31a260091d356cea706b3792d242699cfa2c1f04/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f7374617475732d756e737461626c652d6c69676874677265656e2e737667)

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
