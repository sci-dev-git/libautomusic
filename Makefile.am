
INCLUDES = -I$(top_builddir)/include      \
           -I$(top_srcdir)/include

lib_LTLIBRARIES = libautomusic.la

libautomusic_la_SOURCES = \
  util-randomize.cc \
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
  libautomusic.cc

libautomusic_la_LDFLAGS = -no-undefined

bin_PROGRAMS = libautomusic

libautomusic_LDADD = libautomusic.la
libautomusic_SOURCES = \
  main.cc
