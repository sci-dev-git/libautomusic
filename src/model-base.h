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
#ifndef MODEL_BASE_H
#define MODEL_BASE_H

#include <vector>
#include "typedefs.h"

namespace autocomp
{

class CompositionToplevel;
class ModelBase
{
public:
  ModelBase()
  {}
  virtual ~ModelBase()
  {}

public:
  virtual const char *model_name() const=0;
  virtual int generate(std::vector<PitchNote> &dst,
                       CompositionToplevel *composition,
                       int src_figure_bank,
                       const std::vector<ChordPair> &src_chords,
                       const std::vector<PitchNote> &src_figure,
                       StructureForm::FormType dst_form_id,
                       const std::vector<ChordPair> &dst_chords,
                       int32_t dst_offset,
                       int32_t dst_barlen,
                       int src_track_key,
                       int key, int scale,
                       int beats)=0;
};

class ModelLibrary
{
public:
  ModelLibrary();

  ModelBase *invokeModel(int figure_bank, int figure_class);

private:
  ModelBase *m_percussionModel;
  ModelBase *m_soloMelody;
  ModelBase *m_soloInstrumental;
  ModelBase *m_chord;
};

}

#endif
