#ifndef OUTPUT_MIDI_H
#define OUTPUT_MIDI_H

#include <fstream>
#include <vector>

#include "output-base.h"

namespace autocomp
{
  
enum MIDIEventType
{
  MIDI_NOTE_EVENT = 0,
  MIDI_PROGRAM_CHANGE_EVENT,
  MIDI_TEMPO_EVENT,
  MIDI_META_EVENT,
  MIDI_SYSEX_EVENT
};

class MIDIEvent
{
public:
  explicit MIDIEvent(MIDIEventType type) : type(type) {}
  virtual ~MIDIEvent() {}
  
  virtual int serialize(std::ofstream &stream, int channel, class OutputMIDI *output)=0;
public:
  MIDIEventType type;
};

class MIDITrack
{
public:
  MIDITrack()
  {}
  ~MIDITrack()
  {
    removeEvents();
  }

  template<typename T>
    T *appendEvent(T *event)
    {
      m_events.push_back(event);
      return event;
    }
  void removeEvents()
  {
    for(std::vector<MIDIEvent *>::iterator iter = m_events.begin(); iter != m_events.end(); iter++)
      delete *iter;
    m_events.clear();
  }

  int serialize(std::ofstream &stream, int channel, class OutputMIDI *output);
public:
  std::vector<MIDIEvent *> m_events;
};

class MIDINoteEvent : public MIDIEvent
{
public:
  MIDINoteEvent() : MIDIEvent(MIDI_NOTE_EVENT)
  {}
  MIDINoteEvent(const OutputBase::Event &note) : MIDIEvent(MIDI_NOTE_EVENT), m_note(note)
  {}
  MIDINoteEvent(const MIDINoteEvent &event) : MIDIEvent(MIDI_NOTE_EVENT), m_note(event.m_note)
  {}

  virtual int serialize(std::ofstream &stream, int channel, class OutputMIDI *output);

  inline const OutputBase::Event &note() const
    {
      return m_note;
    }
  inline void setNote(const OutputBase::Event &note)
    {
      m_note = note;
    }

public:
  OutputBase::Event m_note;
};


class MIDIProgramChangeEvent : public MIDIEvent
{
public:
  MIDIProgramChangeEvent() : MIDIEvent(MIDI_PROGRAM_CHANGE_EVENT),
        m_delta_time(0),
        m_program(0)
  {}
  MIDIProgramChangeEvent(uint32_t deltaTime, uint8_t program) : MIDIEvent(MIDI_PROGRAM_CHANGE_EVENT),
      m_delta_time(deltaTime),
      m_program(program)
  {}

  virtual int serialize(std::ofstream &stream, int channel, class OutputMIDI *output);

  inline uint32_t deltaTime() const
    {
      return m_delta_time;
    }
  inline void setDeltaTime(uint32_t delta_time)
    {
      m_delta_time = delta_time;
    }
  inline uint8_t prog() const
    {
      return m_program;
    }
  inline void setProg(uint8_t prog)
    {
      m_program = prog;
    }

public:
  uint32_t m_delta_time;
  uint8_t m_program;
};

class MIDIMetaEvent : public MIDIEvent
{
public:
  enum Meta
  {
    MIDI_META_SEQUENCE_NUMBER = 0x0,
    MIDI_META_TEXT = 0x01,
    MIDI_META_COPYRIGHT = 0x2,
    MIDI_META_SEQNAME = 0x03,
    MIDI_META_INSTRNAME = 0x04,
    MIDI_META_LYRIC = 0x05,
    MIDI_META_MARKER = 0x06,
    MIDI_META_CUE = 0x07,
    MIDI_META_CHANNEL_PREFIX = 0x20,
    MIDI_META_END_OF_TRACK = 0x2f,
    MIDI_META_SET_TEMPO = 0x51,
    MIDI_META_SMPTE_OFFSET = 0x54,
    MIDI_META_TIME_SIGNATURE = 0x58,
    MIDI_META_KEY_SIGNATURE = 0x59,
    MIDI_META_SEQUENCER_SPECIFIC = 0x7f
  };

  explicit MIDIMetaEvent(Meta meta) : MIDIEvent(MIDI_META_EVENT),
      m_meta(meta),
      m_abs_time(0)
  {
  }

  inline uint32_t time() const
  {
    return m_abs_time;
  }
  inline void setAbsTime(uint32_t time)
  {
    m_abs_time = time;
  }
  inline const std::string &data() const
  {
    return m_data;
  }
  inline void setData(const std::string &data)
  {
    m_data = data;
  }

  int serialize(std::ofstream &stream, int channel, class OutputMIDI *output);

private:
  Meta m_meta;
  uint32_t m_abs_time;
  std::string m_data;
};

class MIDITempoEvent : public MIDIEvent
{
public:
  MIDITempoEvent() : MIDIEvent(MIDI_TEMPO_EVENT),
      m_delta_time(0),
      m_tempo(0)
  {}

  int serialize(std::ofstream &stream, int channel, class OutputMIDI *output);

public:
  inline uint32_t deltaTime() const
    {
      return m_delta_time;
    }
  inline void setDeltaTime(uint32_t delta_time)
    {
      m_delta_time = delta_time;
    }
  inline uint32_t tempo() const
    {
      return m_delta_time;
    }
  inline void setTempo(uint32_t tempo)
    {
      m_tempo = tempo;
    }

private:
  uint32_t m_delta_time;
  uint32_t m_tempo;
};

class OutputMIDI : public OutputBase
{
public:
  OutputMIDI();
  ~OutputMIDI();
  
public:
  virtual float tick_64p() const { return 7.5; }
  virtual int outputPrepare(std::ofstream &stream, float) { return 0; }
  virtual int outputTracks(std::ofstream &stream, const std::vector<OutputBase::Track> &sequence, float tempo);
  virtual int outputFinal(std::ofstream &stream);

public:
  MIDITrack *appendTrack(MIDITrack *track)
  {
    m_tracks.push_back(track);
    return track;
  }
  void removeTracks()
  {
    for(std::vector<MIDITrack *>::iterator iter = m_tracks.begin(); iter != m_tracks.end(); iter++)
      delete *iter;
    m_tracks.clear();
  }

  int writeMIDI(std::ofstream &stream, int format, int division);

  uint32_t mf_sec2ticks(float secs, uint32_t division, uint32_t tempo);
  float mf_ticks2sec(uint32_t ticks, uint32_t division, uint32_t tempo);
private:
  unsigned char mf_put8(std::ofstream &stream, unsigned char ch);
  void mf_write32(std::ofstream &stream, uint32_t data);
  void mf_write16(std::ofstream &stream, uint16_t data);
  void mf_put_variable_len(std::ofstream &stream, uint32_t value);
private:
  int mf_w_header_chunk(std::ofstream &stream, int format, int ntracks, int division);
  int mf_w_track_chunk(std::ofstream &stream, int channel,  MIDITrack *track);

  int mf_w_midi_event(std::ofstream &stream, uint32_t abs_time, uint16_t type, uint16_t chan, uint8_t *data, uint32_t size);
  int mf_w_meta_event(std::ofstream &stream, uint32_t abs_time, uint16_t type, const uint8_t *data, uint32_t size);
  int mf_w_sysex_event(std::ofstream &stream, uint32_t abs_time, const uint8_t *data, uint32_t size);

protected:
  uint32_t getDeltaTime(uint32_t absTime);

protected:
  long m_written;
  int m_laststate;
  int m_lastmeta;
  uint32_t m_currentTime;

  std::vector<MIDITrack *> m_tracks;

  friend class MIDITrack;
  friend class MIDINoteEvent;
  friend class MIDIProgramChangeEvent;
  friend class MIDITempoEvent;
  friend class MIDIMetaEvent;
};

}

#endif
