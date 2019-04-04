
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>

#define TSF_IMPLEMENTATION
#include "synth/tsf.hpp"

#include "libautomusic.h"
#include "output-pcm-audio.h"

namespace autocomp
{

static const char *sample_file = "synth-samples.sf2"; /* Audio samples for synth instruments */
static int render_sample_rate = 44100; /* PCM audio sample rate */

OutputPcmAudio::OutputPcmAudio()
    :   m_tsf(0l),
        m_time_64p(1)
{}

OutputPcmAudio::~OutputPcmAudio()
{
  if (m_tsf) tsf_close(m_tsf);
}

int OutputPcmAudio::outputPrepare(std::ofstream &stream, float tempo)
{
  m_time_64p = tempo * 1000 / 60/ 32; /* duration of each 1/64 note in ms */

  if (!m_tsf) {
    /*
     * Lazy init.
     * Loading the audio samples once for the later processes of instrument synthesis.
     */
    m_tsf = tsf_load_filename(sample_file);
  }
  return m_tsf ? 0 : -RC_LOADING_SAMPLES;
}

#define MAX_BUFFER_SIZE 4096

class ChannelEvent : public OutputBase::Event {
public:
  ChannelEvent() : channel(-1)
  {}
  ChannelEvent(int channel, const OutputBase::Event &event) : OutputBase::Event(event), channel(channel)
  {}
  int channel;
};

static bool events_cmp(const ChannelEvent &a, const ChannelEvent &b)
{
  return a.tick < b.tick;
}

int OutputPcmAudio::outputTracks(std::ofstream &stream, const std::vector<OutputBase::Track> &sequence, float tempo)
{
  std::vector<ChannelEvent> events;
  for(std::size_t trackNum=0; trackNum < sequence.size(); trackNum++)
    {
      for(std::size_t i=0; i < sequence[trackNum].events.size(); i++)
        {
          ChannelEvent channel (trackNum, sequence[trackNum].events[i]);
          events.push_back(channel);
        }
    }
  std::sort(events.begin(), events.end(), events_cmp);

  /* Set the synthesizer rendering output mode */
  tsf_set_output(m_tsf, TSF_STEREO_INTERLEAVED, render_sample_rate, 0.0f);

  tsf_channel_set_volume(m_tsf, 0, 1.2); /* Adjust the volume gain of melody track */

  /*
   * Set timbre bank for each channels
   */
  std::size_t drums_channel = sequence.size();
  for(std::size_t trackNum=0; trackNum < sequence.size(); trackNum++)
    {
      if (sequence[trackNum].gm_timbre == 128)
        {
          drums_channel = trackNum;
          tsf_channel_set_volume(m_tsf, trackNum, 0.1); /* Adjust the volume gain of drum track */
        }
      tsf_channel_set_presetnumber(m_tsf, trackNum, sequence[trackNum].gm_timbre, trackNum == drums_channel);
    }
  /*
   * Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
   */
  tsf_channel_set_bank_preset(m_tsf, (int)drums_channel, 128, 0);

  double msec;
  std::size_t seq = 0;
  char buff[MAX_BUFFER_SIZE * 2 * sizeof(short)];
  for(;;)
    {
      char *chunk = buff;
      /* Number of samples to process */
      int sampleBlock, sampleCount = (MAX_BUFFER_SIZE / (2 * sizeof(short))); /* 2 output channels */

      for (sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; sampleCount > 0; sampleCount -= sampleBlock, chunk += (sampleBlock * (2 * sizeof(short))))
        {
          if (sampleBlock > sampleCount) sampleBlock = sampleCount;

          /*
           * Loop through all MIDI messages which need to be rendered until the current timestamp
           */
          for (msec += sampleBlock * (1000.0 / 44100.0); seq < events.size() && msec >= events[seq].tick; seq++)
            {
              if (events[seq].off)
                tsf_channel_note_off(m_tsf, events[seq].channel, events[seq].pitch);
              else
                tsf_channel_note_on(m_tsf, events[seq].channel, events[seq].pitch, events[seq].velocity / 127.0f);
            }

          /*
           * Render the block of audio samples in S16 format
           */
          tsf_render_short(m_tsf, (short *)chunk, sampleBlock, 0);
          stream.write(chunk, sampleBlock * 2 * sizeof(short));
        }
      if (seq == events.size()) break;
    }

  return 0;
}

int OutputPcmAudio::outputFinal(std::ofstream &stream)
{
  return 0;
}

}
