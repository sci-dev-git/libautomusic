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
#ifndef THEORY_CHORD_H
#define THEORY_CHORD_H

#include <string>
#include <vector>

#include "typedefs.h"

namespace autocomp
{ namespace theory
  {

std::string chord_get_name(const ChordPair &chord);
const char *key_get_name(int key);
const char *scale_get_name(int scale);

ChordPair chord_shift(const ChordPair &chord, int root_offset);
int     chord_get_tone(int key, int chord_root, int scale = 0 );
bool    chord_is_in_key(const ChordPair &chord, int key, int scale);

bool    pitch_is_in_chord(int pitch, const ChordPair &chord);
int     pitch_get_in_scale(int pitch, int diff_tone, int key, int scale);
int     pitch_get_in_chord(int pitch, const ChordPair &chord, int dst_in_chord_level = -1, int avg_pitch_no = 128);
int     pitch_get_in_octave_1(int pitch, int dst_pitch);

int transform_figure_chord(std::vector<PitchNote> &dst,
                           int src_key,
                           const std::vector<ChordPair> &src_chord_list,
                           const std::vector<PitchNote> &src_figure_list,
                           int num_bar,
                           int dst_key,
                           const std::vector<ChordPair> &dst_chord_list,
                           int dst_scale = 0,
                           int dst_offset = 0,
                           int dst_beats = 4,
                           int src_beats = 4);
  }
}

#endif
