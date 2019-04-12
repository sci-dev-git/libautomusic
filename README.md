# libautomusic (Algorithmic Musical Composition Library)

------
![Build status](https://img.shields.io/badge/libautomusic-blue.svg) ![Build status](https://img.shields.io/badge/build-passing-orange.svg) ![Item status](https://img.shields.io/badge/status-unstable-lightgreen.svg)

`libautomusic` allows us to create a unique music based on the given `picture` (all styles are acceptable), without any manual configurations or interventions.

The result of composition is not obscure or machine-like but just humanized to a certain extent, which thanks to the algorithmic based on music-theory model and knowledge-library-system, enabling us to create complicated and multi-track MIDI projects.

The paper describing the algorithm in details is available on https://www.cnblogs.com/sci-dev/p/10261380.html.

# Guide to quick start

Start a terminal:

    mkdir build && cd build

Then configure the source tree, indicating install prefix and enabling options or features we wanted. For details of all the available options and features, try configure --help.

    ../configure

Go ahead with compilation.

    make all

Finally install the library and executables.

    make install

In Windows you should prepare a MinGW or CygWin environment at first. In unix-like system these facilities are all natrual for you.

If things go right you will get a program named 'libautomusic' in your prefix directory. The following command is to start our first composition process by merely giving a picture.

    ./libautomusic your_image_file_here.jpg

And you will get a MIDI file in the same path of the executable, which stores all the notes and voices of your new music.

For the futher polishing, you may want to import this file to other MIDI edition softwares.

# Various output formats

libautomusic supports various formats for storage of the music output. Besides traditional MIDI files, we have implemented other formats such as MusicXML, musical score in SVG and audio files(raw PCM or compressed MP3).

Among these formats, audio output is implemented by a virtual instrument synthesizer that generates the sound of all the real instruments by software. Currently the synthesizer is based on soundfont, an ancient standard that is firstly applied in GM soundcards.

# Dependencies

libautomusic requires the following libraries to be installed (if all the corresponding features are enabled):

* yaml-cpp (at least 0.6.2)

* opencv (at least 3.0) (optional)

* libmp3lame (at least 3.1) (optional)


# Maintain
We're in maintenance of this project though currently the item is still in a very alpha stage. A bootstrap script is provided to construct building system.
Maintenance-Mode is disabled by default, however, you can enable it if it really needs.
