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
#include "model-chord.h"
#include "model-percussion.h"
#include "model-solo-instrumental.h"
#include "model-solo-melody.h"
#include "theory-orchestration.h"
#include "model-base.h"

namespace autocomp
{

ModelLibrary::ModelLibrary() :
      m_percussionModel(new ModelPercussion),
      m_soloMelody(new  ModelSoloMelody),
      m_soloInstrumental(new ModelSoloInstrumental),
      m_chord(new ModelChord)
{
}

/**
 * @brief Invoke a model to generate figures based on bank and class.
 */
ModelBase *ModelLibrary::invokeModel(int figure_bank, int figure_class)
{
  switch(figure_bank)
  {
    case theory::FIGURE_BANK_MELODY:
      return m_soloMelody;
    case theory::FIGURE_BANK_DRUMS:
      return m_percussionModel;
    default:
        if(figure_class == theory::FIGURE_CLASS_CHORD)
          return m_chord;
        else
          return m_soloInstrumental;
  }
}

}
