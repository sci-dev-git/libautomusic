/*
 *  libautomusic (Library for Image-based Algorithmic Musical Composition)
 *  Copyright (C) 2018, automusic.
 *
 *  THIS PROJECT IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR
 *  MODIFY IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE(GPL)
 *  AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; EITHER VERSION 2.1
 *  OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 *
 *  THIS PROJECT IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 *  BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 *  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE GNU
 *  LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
#include <fstream>
#include <climits>
#include <cassert>

#include "libautomusic.h"
#include "theory-harmonics.h"
#include "theory-orchestration.h"
#include "output-music-xml.h"

namespace autocomp
{

static const int node_ticks[] = {64, 128, 256, 512, 1024, 2048, 4096};
static const char *node_types[] = {"64th", "32th", "16th", "eighth", "quarter", "half", "whole"};

#define NOTE_WHOLE_TICKS (4096) /* ticks of a whole note */

OutputMusicXML::OutputMusicXML()
    :   m_beat_time(0),
        m_beats(0),
        m_tempo(0)
{}

OutputMusicXML::~OutputMusicXML()
{}

int OutputMusicXML::getNoteIndex(int duration)
{
  int min = INT_MAX, type;
  for(int deg=6; deg <=0 ; deg--)
    {
      int delta = ABS(duration - node_ticks[deg]);
      if (min > delta)
        { min = delta; type = delta; }
    }
  return type;
}

int OutputMusicXML::outputPrepare(std::ofstream &stream, int beat_time, int beats, float tempo)
{
  /*
   * Put the header and score root node of MusicXML.
   */
  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
  stream << "<!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\">\n";
  stream << "<score-partwise version=\"3.0\">\n";

  m_beat_time = beat_time;
  m_beats = beats;
  m_tempo = tempo;
  return 0;
}

int OutputMusicXML::outputTracks(std::ofstream &stream, const std::vector<OutputBase::Track> &sequence)
{
  /*
   * Write score parts (each part is to a instrument).
   */
  for(std::size_t trackNum=0; trackNum < sequence.size(); trackNum++)
    {
      stream << "<part-list>\n"
                "  <score-part id=\"P" << trackNum << "\">\n"
                "    <part-name>" << trackNum << "</part-name>\n"
                "  </score-part>\n"
                "</part-list>\n";
    }

  /*
   * Write measures and notes belonged to each measure.
   */
  for(std::size_t trackNum=0; trackNum < sequence.size(); trackNum++)
    {
      const OutputBase::Track & track = sequence[trackNum];
      
      if (!track.events.size())
        return 0;
  
      switch(track.figureClass)
        {
        case theory::FIGURE_CLASS_SOLO:
          break;
        case theory::FIGURE_CLASS_CHORD:
          break;
        }

      int bound_end = track.events[track.events.size()-1].tick;
      int last_note_pos = 0;
      int current_measure = -1;
      int measure_start = -1, measure_end = -1;

      stream << "<part id=\"P" << (trackNum) << "\">\n";
      for (;;)
        {
          ++current_measure;
          measure_start = 4096 * current_measure;
          measure_end = measure_start + 4096;

          if (measure_start >= bound_end)
            break;

          stream << "<measure number=\"" << (current_measure + 1) << "\">\n";

          for(std::size_t i=0 ; i < track.events.size(); i++)
            {
              if (track.events[i].off) /* only does it consider note-on */
                continue;
              if( measure_start <= track.events[i].tick && track.events[i].tick + track.events[i].duration < measure_end )
                {
                  /*
                   * Insert Rest notes to pad the silence before the voicing notes.
                   */
                  int rest_duartion = track.events[i].tick - last_note_pos;
                  while (rest_duartion >= 64)
                    {
                      int deg;
                      for(deg=6; deg >= 0; deg--)
                        {
                          if (rest_duartion >= node_ticks[deg])
                            break;
                        }
                      assert(deg);
                      rest_duartion -= node_ticks[deg];
                      printf("%d %d\n", deg, rest_duartion);
                      stream << "<note>\n"
                                "  <rest/>\n"
                                "  <duration>" << node_ticks[deg] << "</duration>\n"
                                "  <type>" << node_types[deg] << "</type>\n"
                                "</note>\n";
                    }
                  last_note_pos = track.events[i].tick;

                  /*
                   * Write this note.
                   */
                  int noteIndex = getNoteIndex(track.events[i].duration);
                  const char *noteType = node_types[noteIndex];
                  const char *step = theory::key_get_name(track.events[i].pitch % 12);
                  int octave = track.events[i].pitch / 12;

                  stream << "<note>\n"
                            "  <pitch>\n"
                            "    <step>" << step << "</step>\n"
                            "    <octave>" << octave << "</octave>\n"
                            "  </pitch>\n"
                            "  <duration>" << track.events[i].duration << "</duration>\n"
                            "  <type>" << noteType << "</type>\n"
                            "</note>\n";
                }
            }

          stream << "</measure>\n";
        }
      stream << "</part>\n";
      break;
    }
  return 0;
}

int OutputMusicXML::outputFinal(std::ofstream &stream)
{
  stream << "</score-partwise>\n";
  return 0;
}

}
