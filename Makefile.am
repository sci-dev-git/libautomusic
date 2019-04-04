
INCLUDES = -I$(top_builddir)/include      \
           -I$(top_srcdir)/include

bin_PROGRAMS = libautomusic

libautomusic_SOURCES = \
  knowledge-model.cc \
  theory-harmonics.cc \
  theory-structure.cc \
  theory-orchestration.cc \
  parameter-generator.cc \
  model-base.cc \
  model-chord.cc \
  model-solo-instrumental.cc \
  model-solo-melody.cc \
  model-percussion.cc \
  output-base.cc \
  output-midi.cc \
  output-pcm-audio.cc \
  composition-toplevel.cc \
  main.cc

libautomusic_CFLAGS =
libautomusic_CPPFLAGS = $(yaml_cpp_CFLAGS) $(opencv_CFLAGS) -DENABLE_IMAGE_COMPOSITION
libautomusic_LDFLAGS = $(yaml_cpp_LIBS) $(opencv_LIBS)
