
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
  output-music-xml.cc \
  composition-toplevel.cc \
  libautomusic.cc

pkginclude_HEADERS = $(top_builddir)/include/libautomusic.h \
                    $(top_builddir)/include/exportdecl.h \
                    $(top_builddir)/include/typedefs.h

libautomusic_la_CPPFLAGS = $(LIBAM_CPPFLAGS)
libautomusic_la_LDFLAGS = $(LIBAM_LDFLAGS) -no-undefined
