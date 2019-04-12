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
#include <iostream>
#include "util-math.h"
#include "libautomusic.h"
#include "theory-harmonics.h"

namespace autocomp
{ namespace theory
  {

static const char *key_string[] =
  {
    "C", "#C",
    "D", "#D",
    "E",
    "F", "#F",
    "G", "#G",
    "A", "#A",
    "B"
  };

#define CHORD_SIGN_NUM 20
static const char *chord_sign_string[CHORD_SIGN_NUM] =
  {"", "m", "m7", "7", "M7", "aug", "dim", "dim7", "sus2", "sus4", "7sus4", "6sus4", "6", "m6", "-5", "+5", "M7+5", "m-5", "7-5", "7+5"};

#define CHORD_COMPONENT_PITCH_NUM 4
static const int chord_component_pitch[CHORD_SIGN_NUM][CHORD_COMPONENT_PITCH_NUM] =
  {
    {0, 4, 7, 12},      /**/
    {0, 3, 7, 12},      /* m */
    {0, 3, 7, 10},      /* m7 */
    {0, 4, 7, 10},      /* 7 */
    {0, 4, 7, 11},      /* M7 */
    {0, 4, 8, 12},      /* aug */
    {0, 3, 6, 12},      /* dim */
    {0, 3, 6, 9},       /* dim7 */
    {0, 2, 7, 12},      /* sus2 */
    {0, 5, 7, 12},      /* sus4 */
    {0, 5, 7, 10},      /* 7sus4 */
    {0, 5, 7, 9},       /* 6sus4 */
    {0, 4, 7, 9},       /* 6 */
    {0, 3, 7, 9},       /* m6 */
    {0, 4, 6, 12},      /* -5 */
    {0, 4, 8, 12},      /* +5 */
    {0, 4, 8, 11},      /* M7+5 */
    {0, 3, 6, 12},      /* m-5 */
    {0, 4, 6, 10},      /* 7-5 */
    {0, 4, 8, 10}       /* 7+5 */
  };

static const char *scale_string[] = {"Major", "Minor"};

#define SCALE_COMPONENT_PITCH_NUM 14
static const int scale_component_pitch[][SCALE_COMPONENT_PITCH_NUM] =
  {
    {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23},
    {0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22}
  };

#define NATURAL_CHORD_NUM 10
static const int natural_chord[][NATURAL_CHORD_NUM][2] =
  {
    {{0, 0}, {2, 1}, {2, 2}, {4, 1}, {4, 2}, {5, 0}, {7, 0}, {9, 1}, {9, 2}, {7, 3}},
    {{0, 1}, {0, 2}, {10, 3}, {3, 0}, {5, 1}, {5, 2}, {7, 1}, {7, 2}, {8, 0}, {10, 0}}
  };

/**
 * @brief Get the standard name of a chord
 */
std::string chord_get_name(const ChordPair &chord)
{
  std::string name("null");
  if( chord.root >= 0 && chord.root < 12 && chord.sign >= 0 && chord.sign < CHORD_SIGN_NUM )
    {
      name.assign(key_string[chord.root]);
      name.append(chord_sign_string[chord.sign]);
    }
  else
    std::cerr << "eee" << chord.root << "," << chord.sign << std::endl;
  return name;
}

/**
 * @brief Get the standard name of a scale
 */
const char *key_get_name(int key)
{
  if( key >=0 && key < 12 )
    return key_string[key];
  else
    return "null";
}

/**
 * @brief Get the standard name of a scale
 */
const char *scale_get_name(int scale)
{
  if( scale >= 0 && scale < 2 )
    return scale_string[scale];
  else
    return "null";
}

/**
 * @brief Shift a chord by root with offset.
 */
ChordPair chord_shift(const ChordPair &chord, int root_offset)
{
  return ChordPair(util::floor_mod(chord.root + root_offset, 12), chord.sign);
}

/**
 * @brief Get the tone of a chord based on key and scale
 * @return pitch [0~6]
 */
int chord_get_tone(int key, int chord_root, int scale /* = 0 */)
{
  int tone_index;
  int pitch_offset = util::floor_mod(chord_root - key, 12);

  for(tone_index=0; tone_index < SCALE_COMPONENT_PITCH_NUM; tone_index++)
    {
      if( pitch_offset == scale_component_pitch[scale][tone_index] )
        return tone_index;
    }

  for(tone_index=0; tone_index < SCALE_COMPONENT_PITCH_NUM-1; tone_index++) /* utilize the nearest tone */
    {
      if( !(pitch_offset > scale_component_pitch[scale][tone_index] && scale_component_pitch[scale][tone_index] < 12 ) )
        break;
    }
  return tone_index;
}

/**
 * @brief Check whether the specified chord is in-key.
 */
bool chord_is_in_key(const ChordPair &chord, int key, int scale)
{
  if( scale < 2 )
    {
      int picth = util::floor_mod(chord.root - key, 12);

      for(unsigned int i=0; i < NATURAL_CHORD_NUM; i++)
        {
          if( natural_chord[scale][i][0] == picth && natural_chord[scale][i][1] == chord.sign )
            return true;
        }
    }
  return false;
}

/**
 * @brief Check whether the specified pitch is in the component pitch of a chord
 */
bool pitch_is_in_chord(int pitch, const ChordPair &chord)
{
  int pitch_offset = util::floor_mod(pitch - chord.root, 12);
  for(unsigned int i=0; i < CHORD_COMPONENT_PITCH_NUM; i++)
    {
      if( chord_component_pitch[chord.sign][i] == pitch_offset )
        return true;
    }
  return false;
}

/**
 * @brief Get the most possible pitch that is in-scale.
 * Shifting the pitch until it becomes in-scale, then shifting the result with an offset.
 */
int pitch_get_in_scale(int pitch, int diff_tone, int key, int scale)
{
  const int *in_scale_list = scale_component_pitch[scale];
  int in_scale_count = SCALE_COMPONENT_PITCH_NUM;

  int pitch_offset = util::floor_mod(pitch - key, 12);
  int index;
  for(index=0; index < in_scale_count; index++)
    {
      if( pitch_offset == in_scale_list[index] )
        break;
    }
  if( index == in_scale_count ) /* pitch in 12-tone is not in-scale, then utilize the nearest scale */
    {
      index--;
      if( diff_tone > 0 )
        {
          for(int i=0; i < in_scale_count; i++)
            if( pitch_offset < in_scale_list[i] )
              {
                index = util::floor_mod(i - 1, in_scale_count);
                break;
              }
        }
      else
        {
          for(int i=0; i < in_scale_count; i++)
            if( pitch_offset < in_scale_list[i] )
              {
                index = i;
                break;
              }
        }
    }

  int shifted_index = index + diff_tone;

  while( shifted_index > in_scale_count ) /* shift down if index is higher than current 2 oct */
    {
        index -= in_scale_count / 2;
        shifted_index -= in_scale_count / 2;
        pitch_offset -= 12;
    }
  while( shifted_index < 0 ) /* shift up when index is lower than current 2 oct */
    {
       index += in_scale_count / 2;
       shifted_index += in_scale_count / 2;
       pitch_offset += 12;
    }

  return pitch + (in_scale_list[shifted_index] - pitch_offset);
}

/**
 * @brief Get the pitch that is harmonized with specified chord.
 * If the current pitch in higher than average pitch, selecting a lowest chord component pitch that is higher the current.
 * Otherwise, a highest chord component pitch lower than the current will be chosen.
 */
int pitch_get_in_chord(int pitch, const ChordPair &chord, int inchord_tone /*= -1*/, int avg_pitch /*= 128*/)
{
  const int *chord_sign_pitches = chord_component_pitch[chord.sign];
  int chord_time = (pitch - chord.root) / 12;
  int chord_tone = util::floor_mod(pitch - chord.root, 12);
  int dst_chord_time, dst_chord_tone;

  if( inchord_tone == -1 )
    {
      for(int i=0; i < CHORD_COMPONENT_PITCH_NUM-1 /* eliminate the last pitch */; i++)
        {
          if( chord_sign_pitches[i] == chord_tone ) /* firstly see if the pitch is already inchord */
            return pitch;
        }

      int index;
      if( pitch < avg_pitch )
        {
          for(index=0; index < CHORD_COMPONENT_PITCH_NUM-1; index++)
            {
              if( chord_sign_pitches[index] >= chord_tone )
                break;
            }
        }
      else
        {
          for(index=CHORD_COMPONENT_PITCH_NUM-2; index>=0; index--)
            {
              if( chord_sign_pitches[index] <= chord_tone )
                break;
            }
        }
      dst_chord_tone = chord_sign_pitches[index];
      dst_chord_time = chord_time;
    }
  else
    {
      dst_chord_tone = chord_sign_pitches[inchord_tone];
      dst_chord_time = chord_time;
      ++dst_chord_time;

      while( dst_chord_time * 12 + dst_chord_tone + chord.root > avg_pitch + 6 )
        --dst_chord_time;
    }
  return dst_chord_time * 12 + dst_chord_tone + chord.root;
}

/**
 * @brief Get the pitch whose difference from dst_pitch is 1 octave.
 * Shifting the pitch (1 octaves each time), until the difference from dst_pitch is 1 octave.
 */
int pitch_get_in_octave_1(int pitch, int dst_pitch)
{
  while( pitch - dst_pitch > 7 )
    pitch -= 12;

  while( pitch - dst_pitch < -7 )
    pitch += 12;

  return pitch;
}

/**
 * @brief Helper function of transform_figure_chord(). Process each 1/4 slice.
 * Push the result back to destination vector without clearing the content of vector(that's an exception).
 */
static int transform_figure_chord_helper(std::vector<PitchNote> &dst,
                                         const std::vector<PitchNote> &src_figure_list,
                                         int src_key, const ChordPair &src_chord,
                                         int dst_key, const ChordPair &dst_chord,
                                         int dst_scale)
{
  const int *src_chord_component = chord_component_pitch[src_chord.sign];
  const int *dst_chord_component = chord_component_pitch[dst_chord.sign];

  int dst_chord_tone = chord_get_tone(dst_key, dst_chord.root, dst_scale);
  if( src_chord.root == dst_chord.root && src_chord.sign == dst_chord.sign && src_key == dst_key )
    {
      dst = src_figure_list;
      return 0;
    }

  int offset = 0;
  if( dst_chord.root - src_chord.root > 5 )
    offset = -1;
  else if( dst_chord.root - src_chord.root < -5 )
    offset = 1;

  int note_chord;
  for(std::size_t i=0; i < src_figure_list.size(); i++)
    {
      const PitchNote &figure = src_figure_list[i];
      long src_note_time = (figure.pitch - src_chord.root) / 12 + offset;
      long src_note_index = util::floor_mod(figure.pitch - src_chord.root, 12);
      int dst_note_index;
      src_note_time = MAX(0, MIN(src_note_time, 10));

      unsigned int j;
      for(j=0; j < CHORD_COMPONENT_PITCH_NUM; j++)
        {
          if( src_chord_component[j] == src_note_index )
            {
              dst_note_index = dst_chord_component[j] + dst_chord.root;
              break;
            }
        }
      if( j == CHORD_COMPONENT_PITCH_NUM )
        {
          if( 0 && scale_component_pitch[dst_scale][0] == 14 ) /* fixme */
            {
              if( src_note_index < src_chord_component[1] )
                note_chord = scale_component_pitch[dst_scale][dst_chord_tone + 1] - scale_component_pitch[dst_scale][dst_chord_tone];
              else if( src_note_index < src_chord_component[2] )
                note_chord = scale_component_pitch[dst_scale][dst_chord_tone + 3] - scale_component_pitch[dst_scale][dst_chord_tone];
              else if( src_note_index < src_chord_component[3] )
                {
                  if( src_note_index == 10 || src_note_index == 11 )
                    note_chord = scale_component_pitch[dst_scale][dst_chord_tone + 6] - scale_component_pitch[dst_scale][dst_chord_tone];
                  else
                    note_chord = scale_component_pitch[dst_scale][dst_chord_tone + 5] - scale_component_pitch[dst_scale][dst_chord_tone];
                }
            }
          else
            {
              int min_delta_in_half = 100;
              int min_delta_in_index = 0;
              for(int j=dst_chord_tone + 1; j < SCALE_COMPONENT_PITCH_NUM; j++)
                {
                  int note24 = scale_component_pitch[dst_scale][j];
                  int cur_delta_in_half = ABS(note24 - scale_component_pitch[dst_scale][dst_chord_tone] - src_note_index);
                  if( ABS(ABS(note24 - scale_component_pitch[dst_scale][dst_chord_tone]) - src_note_index) < min_delta_in_half )
                    {
                      min_delta_in_half = cur_delta_in_half;
                      min_delta_in_index = j - (dst_chord_tone + 1);
                    }
                }
              note_chord = scale_component_pitch[dst_scale][min_delta_in_index] - scale_component_pitch[dst_scale][dst_chord_tone];
            }
          dst_note_index = note_chord + dst_chord.root;
        }
      dst.push_back(PitchNote(src_note_time * 12 + dst_note_index, figure.velocity, figure.start, figure.end));
    }

  return 0;
}

/**
 * @brief Transform a figure into new one, with new key, chord, and time beats.
 * If succeeded, clear destination vector at first, then push the result back it.
 */
int transform_figure_chord(std::vector<PitchNote> &dst,
                           int src_key,
                           const std::vector<ChordPair> &src_chord_list,
                           const std::vector<PitchNote> &src_figure_list,
                           int num_bar,
                           int dst_key,
                           const std::vector<ChordPair> &dst_chord_list,
                           int dst_scale /*= 0*/,
                           int dst_offset /*= 0*/,
                           int dst_beats /*= 4*/,
                           int src_beats /*= 4*/)
{
  int rc;
  std::vector<ChordPair> reg_old_chord_list(src_chord_list);
  std::vector<ChordPair> reg_dst_chord_list(dst_chord_list);

  if( src_chord_list.size() / src_beats != dst_chord_list.size() / dst_beats )
    {
      std::cerr << "Transformation is broken down as the number of beats between origin and new is not equal" << std::endl;
    }

  reg_old_chord_list.push_back(reg_old_chord_list.back());
  reg_dst_chord_list.push_back(reg_dst_chord_list.back());

  std::size_t src_num_beat = num_bar * src_beats;
  if( src_num_beat + 1 != reg_old_chord_list.size() )
    {
      std::cerr << "Less or more chords are needed, src_num_beat = " << src_num_beat << " chord_num = " << reg_old_chord_list.size()-1 << std::endl;
      return -RC_FAILED;
    }
  std::size_t dst_num_beat = num_bar * dst_beats;
  if( dst_num_beat + 1 != reg_dst_chord_list.size() )
    {
      std::cerr << "Less or more chords are needed, dst_num_beat = " << dst_num_beat << " chord_num = " << reg_dst_chord_list.size()-1 << std::endl;
      return -RC_FAILED;
    }
  dst.clear();

  int32_t from_beat_in64;
  int32_t to_beat_in64;
  int j = 0, k = 0;

  for(std::size_t i=0; i < src_num_beat; i++)
    {
      std::vector<PitchNote> slice_trans_result;

      if( src_beats == dst_beats )
        {
          /*
           * Slice the figure sequence into 1/4 fragments to be transformed
           */
          if( i == 0 )
            {
              from_beat_in64 = 0;
              to_beat_in64 = 16 * (i + dst_offset + 1);
            }
          else
            {
              from_beat_in64 = 16 * (i + dst_offset);
              to_beat_in64 = 16 * (i + dst_offset + 1);
            }
          std::vector<PitchNote> slice_figure_list;
          for(std::size_t m=0; m < src_figure_list.size(); m++)
            {
              if( from_beat_in64 <= src_figure_list[m].start && src_figure_list[m].start < to_beat_in64 )
                slice_figure_list.push_back(src_figure_list[m]);
            }

          if( (rc = transform_figure_chord_helper(dst, slice_figure_list,
                                                  src_key, reg_old_chord_list[i],
                                                  dst_key, reg_dst_chord_list[i], dst_scale)) )
            { return rc; }
        }
      else
        {
          /*
           * We are supposed to transform a figure into another that owns different beats.
           * Trying to adjust the duration of each note to align at first, then slice.
           */
          if( i % 4 == 2 )
            {
              k += 1;
              continue;
            }
          if( i == 0 )
            {
              from_beat_in64 = 0;
              to_beat_in64 = 16 * (i + dst_offset + 1);
            }
          else
            {
              from_beat_in64 = 16 * (i + dst_offset);
              to_beat_in64 = 16 * (i + dst_offset + 1);
            }

          std::vector<PitchNote> slice_figure_list;
          for(std::size_t m=0; m < src_figure_list.size(); m++)
            {
              const PitchNote &figure = src_figure_list[m];

              if( from_beat_in64 <= figure.start && figure.start < to_beat_in64 )
                {
                  slice_figure_list.push_back(PitchNote(figure.pitch, figure.velocity,
                                                        figure.start - 16 * k,
                                                        figure.end - 16 * k));
                }
            }

          if( (rc = transform_figure_chord_helper(dst, slice_figure_list,
                                                  src_key, reg_old_chord_list[i],
                                                  dst_key, reg_dst_chord_list[j], dst_scale)) )
            { return rc; }
          j += 1;
        }
    }
  return 0;
}

  }
}
