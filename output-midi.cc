
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "libautomusic.h"
#include "output-midi.h"

namespace autocomp
{

#define lowerbyte(x)    ((unsigned char)(x & 0xff))
#define upperbyte(x)    ((unsigned char)((x & 0xff00)>>8))

uint32_t OutputMIDI::mf_sec2ticks(float secs, uint32_t division, uint32_t tempo)
{
  return ((secs * 1000.0) / 4.0 * division) / tempo;
}

float OutputMIDI::mf_ticks2sec(uint32_t ticks, uint32_t division, uint32_t tempo)
{
  float smpte_format, smpte_resolution;

  if( division > 0 )
    {
      return ((float)ticks * (float)tempo) / ((float)division * 1000000.0);
    }
  else
    {
      smpte_format = upperbyte(division);
      smpte_resolution = lowerbyte(division);
      return (float)ticks / (smpte_format * smpte_resolution * 1000000.0);
    }
}

#define MThd    0x4d546864L
#define MTrk    0x4d54726bL
#define MTHD    256
#define MTRK    257
#define TRKEND  258

#define MIDI_META_EVENT         0xFF

#define MIDI_NOTE_OFF           0x80
#define MIDI_NOTE_ON            0x90
#define MIDI_CONTROL_CHANGE     0xb0
#define MIDI_PROGRAM_CHANGE     0xc0

uint8_t OutputMIDI::mf_put8(std::ofstream &stream, uint8_t ch)
{
  stream.put(ch); m_written++;
  return ch;
}

void OutputMIDI::mf_write32(std::ofstream &stream, uint32_t data)
{
  mf_put8(stream, (uint8_t)((data >> 24) & 0xff));
  mf_put8(stream, (uint8_t)((data >> 16) & 0xff));
  mf_put8(stream, (uint8_t)((data >> 8 ) & 0xff));
  mf_put8(stream, (uint8_t)(data & 0xff));
}

void OutputMIDI::mf_write16(std::ofstream &stream, uint16_t data)
{
  mf_put8(stream, (uint8_t)((data & 0xff00) >> 8));
  mf_put8(stream, (uint8_t)(data & 0xff));
}

void OutputMIDI::mf_put_variable_len(std::ofstream &stream, uint32_t value)
{
  uint32_t buffer;

  buffer = value & 0x7f;
  while((value >>= 7) > 0)
    {
      buffer <<= 8;
      buffer |= 0x80;
      buffer += (value & 0x7f);
    }
  for(;;)
    {
      mf_put8(stream, (uint8_t)(buffer & 0xff));
      if (buffer & 0x80)
        buffer >>= 8;
      else
        return;
    }
}

int OutputMIDI::mf_w_header_chunk(std::ofstream &stream, int format, int ntracks, int division)
{
  uint32_t ident, length;

  ident = MThd;
  length = 6;
  mf_write32(stream, ident);
  mf_write32(stream, length);
  mf_write16(stream, format);
  mf_write16(stream, ntracks);
  mf_write16(stream, division);
  return 0;
}

int OutputMIDI::mf_w_track_chunk(std::ofstream &stream, int channel, MIDITrack *track )
{
  uint32_t trackHeader, trackLength;
  size_t offset, place_marker;
  int rc;

  trackHeader = MTrk;
  trackLength = 0;

  offset = stream.tellp();
  mf_write32( stream, trackHeader );
  mf_write32( stream, trackLength );

  m_written = 0;
  m_laststate = 0;
  if( (rc = track->serialize(stream, channel, this)) )
    return rc;

  /*
   * Append the MTrkEnd if it has been ignored, ensuring that the track is closed properly.
   */
  if (m_laststate != MIDI_META_EVENT || m_lastmeta != (uint8_t)MIDIMetaEvent::MIDI_META_END_OF_TRACK)
    {
      mf_put8(stream, 0);
      mf_put8(stream, MIDI_META_EVENT);
      mf_put8(stream, (uint8_t)MIDIMetaEvent::MIDI_META_END_OF_TRACK);
      mf_put8(stream, 0);
    }

  m_laststate = 0;

  /*
   * Replace the header of track with its real length
   */
  trackLength = m_written;
  place_marker = stream.tellp();
  stream.seekp(offset, std::ios::beg);
  mf_write32( stream, trackHeader );
  mf_write32( stream, trackLength );
  stream.seekp(place_marker, std::ios::beg);
  return 0;
}

int OutputMIDI::mf_w_midi_event(std::ofstream &stream, uint32_t abs_time, uint16_t type, uint16_t channel, uint8_t *data, uint32_t size)
{
  unsigned char c = type | channel;
  uint32_t delta_time = getDeltaTime(abs_time);
  if( delta_time < 0 )
    return -RC_FAILED;

  mf_put_variable_len( stream, delta_time );

  if( channel > 15 )
    {
      std::cerr << "error: MIDI channel greater than 16\n" << std::endl;
      return -RC_FAILED;
    }
  if( m_laststate != c )
    mf_put8(stream, c);
  m_laststate = c;
  for(uint32_t i = 0; i < size; i++)
    mf_put8(stream, data[i]);

  return 0;
}

int OutputMIDI::mf_w_meta_event(std::ofstream &stream, uint32_t abs_time, uint16_t type, const uint8_t *data, uint32_t size)
{
  uint32_t delta_time = getDeltaTime(abs_time);
  if( delta_time < 0 )
    return -RC_FAILED;
  mf_put_variable_len( stream, delta_time );
  mf_put8( stream, MIDI_META_EVENT );
  m_laststate = MIDI_META_EVENT;
  mf_put8( stream, type );
  m_lastmeta = type;
  mf_put_variable_len(stream, size);
  for(uint32_t i = 0; i < size; i++)
    {
      if( mf_put8(stream, data[i]) != data[i] ) return -RC_FAILED;
    }
  return 0;
}

int OutputMIDI::mf_w_sysex_event(std::ofstream &stream, uint32_t abs_time, const uint8_t *data, uint32_t size)
{
  uint32_t delta_time = getDeltaTime(abs_time);
  if( delta_time < 0 )
    return -RC_FAILED;

  mf_put_variable_len( stream, delta_time );
  mf_put8( stream, *data );
  m_laststate = 0;
  mf_put_variable_len( stream, size-1 );

  for(uint32_t i = 1; i < size; i++)
    {
      if( mf_put8(stream, data[i]) != data[i] ) return -RC_FAILED;
    }
  return 0;
}

uint32_t OutputMIDI::getDeltaTime(uint32_t absTime)
{
  uint32_t deltaTime = absTime - m_currentTime;
  if (deltaTime < 0)
    {
      std::cerr << "Illegal time value" << std::endl;
      return -1;
    }
  m_currentTime = absTime;
  return deltaTime;
}

int MIDINoteEvent::serialize(std::ofstream &stream, int channel, class OutputMIDI *output)
{
  int rc;
  uint8_t data[2] = {m_note.pitch, m_note.velocity};

  if( (rc = output->mf_w_midi_event(stream, m_note.tick, m_note.off ? MIDI_NOTE_OFF : MIDI_NOTE_ON, channel, data, 2L)) )
    return rc;
  return 0;
}

int MIDIProgramChangeEvent::serialize(std::ofstream &stream, int channel, class OutputMIDI *output)
{
  uint8_t data[1] = {m_program};
  return output->mf_w_midi_event(stream, m_delta_time, MIDI_PROGRAM_CHANGE, channel, data, 1L);
}

int MIDITempoEvent::serialize(std::ofstream &stream, int channel, class OutputMIDI *output)
{
  output->mf_put_variable_len(stream, m_delta_time);
  output->mf_put8(stream, MIDI_META_EVENT);
  output->m_laststate = MIDI_META_EVENT;
  output->mf_put8(stream, (uint8_t)(MIDIMetaEvent::MIDI_META_SET_TEMPO));
  output->mf_put8(stream, 3);
  output->mf_put8(stream, (uint8_t)(0xff & (m_tempo >> 16)));
  output->mf_put8(stream, (uint8_t)(0xff & (m_tempo >> 8)));
  output->mf_put8(stream, (uint8_t)(0xff & m_tempo));
  return 0;
}

int MIDIMetaEvent::serialize(std::ofstream &stream, int channel, class OutputMIDI *output)
{
  return output->mf_w_meta_event(stream, m_abs_time, (uint16_t)m_meta, (uint8_t*)m_data.c_str(), m_data.size());
}

int MIDITrack::serialize(std::ofstream &stream, int channel, class OutputMIDI *output)
{
  int rc;
  for(std::vector<MIDIEvent *>::iterator iter = m_events.begin(); iter != m_events.end(); iter++)
    {
      MIDIEvent *event = *iter;
      if( (rc = event->serialize(stream, channel, output)) )
        return rc;
    }
  return 0;
}

OutputMIDI::OutputMIDI()
    :   m_written(0),
        m_laststate(0),
        m_lastmeta(0),
        m_currentTime(0)
{
}

OutputMIDI::~OutputMIDI()
{
  removeTracks();
}

int OutputMIDI::writeMIDI(std::ofstream &stream, int format, int division)
{
  int rc, channel = 0;
  mf_w_header_chunk(stream, format, m_tracks.size(), division);

  for(std::vector<MIDITrack *>::iterator iter = m_tracks.begin(); iter != m_tracks.end(); iter++)
    {
      m_currentTime = 0;
      if( (rc = mf_w_track_chunk(stream, channel++, (*iter))) )
        return rc;
    }
  return 0;
}

#define MICROSECONDS_PER_MINUTE 60000000

int OutputMIDI::outputTracks(std::ofstream &stream, const std::vector<OutputBase::Track> &sequence, float tempo)
{
  for(std::size_t trackNum=0; trackNum < sequence.size(); trackNum++)
    {
      const OutputBase::Track &track = sequence[trackNum];

      MIDITrack *midiTrack = appendTrack(new MIDITrack);

      int mf_pnq = MICROSECONDS_PER_MINUTE / tempo;

      midiTrack->appendEvent(new MIDIMetaEvent(MIDIMetaEvent::MIDI_META_SEQNAME))->setData("Track");
      midiTrack->appendEvent(new MIDIProgramChangeEvent)->setProg(track.gm_timbre);
      midiTrack->appendEvent(new MIDITempoEvent)->setTempo(mf_pnq);

      for(std::size_t i=0; i < track.events.size(); i++)
        {
          midiTrack->appendEvent(new MIDINoteEvent(track.events[i]));
        }
    }

  return 0;
}

int OutputMIDI::outputFinal(std::ofstream &stream)
{
  int tick_per_quarternote = int(tick_64p() * 2 * 2 * 2 * 2);
  return writeMIDI(stream, 1, tick_per_quarternote);
}

}
