
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
