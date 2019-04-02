
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>

#include "libautomusic.h"
#include "knowledge-model.h"
#include "parameter-generator.h"
#include "theory-harmonics.h"
#include "output-base.h"
#include "util-randomize.h"
#include "composition-toplevel.h"

#define MODEL_PATH "../models/"
#define MODEL_COUNT 111

static autocomp::KnowledgeModel *g_model = 0l;
static autocomp::CompositionToplevel *g_composition = 0l;

void printChordVector(const std::vector<autocomp::ChordPair> &chords)
{ using namespace autocomp;
  for(std::size_t j=0; j < chords.size(); j++)
    {
      std::cout << theory::chord_get_name(chords[j]) << " ";
    }
  std::cout << std::endl;
}

static int tst_parameter_generator()
{
  using namespace autocomp;
  ParameterGenerator *generator = new ParameterGenerator(g_model);

  if( int err = generator->gen(0, 0, 0, 4) )
    return err;

  const std::vector<FormChainNode *> &chain = generator->chains();

  for(std::size_t i=0; i < chain.size(); i++)
    {
      std::cout << "form: " << i << " = ";
      printChordVector(chain[i]->chords);
      std::cout << "size: " << chain[i]->chords.size() << " bars: " << chain[i]->form.bars() << " begin=" << chain[i]->form.begin() << " end=" << chain[i]->form.end() << std::endl;
    }
  std::cout << "count: " << generator->timbreBanks().size() << std::endl;
  return 0;
}

static int tst_composition_toplevel()
{
  using namespace autocomp;
  Output *output = new Output;
  int beats = 3;

  if( int err = g_composition->generator()->gen(0, 0, 0, beats))
    return err;
  if( int err = g_composition->startup() )
    return err;
  if(int err = output->outputCompositionChain("./comp.mid", g_composition->chains(), g_composition->generator()->timbreBanks(), beats, g_composition->tempo()))
    return err;

  return 0;
}

static int tst_image_composition(const char *filename)
{
  using namespace autocomp;
  Output *output = new Output;
  int beats = 3;

  if( int err = g_composition->generator()->gen(filename, 0, beats) )
    return err;
  if( int err = g_composition->startup() )
    return err;
  if(int err = output->outputCompositionChain("./comp.mid", g_composition->chains(), g_composition->generator()->timbreBanks(), beats, g_composition->tempo()))
    return err;

  return 0;
}

int main(int argc, char *argv[])
{
  using namespace autocomp;
  using namespace std;
  int rc;
//std::srand(2000);
  std::srand(60);
  g_model = new KnowledgeModel;
  g_composition = new CompositionToplevel;

  std::cout << "Reading models..." << std::endl;

  for(unsigned int i=0; i<MODEL_COUNT; i++)
    {
      char model_bank_filename[1024];
      std::snprintf(model_bank_filename, sizeof model_bank_filename, MODEL_PATH "/knowledge.bank.%d.mdel", i);

      if( /*(rc = g_model->loadModel(model_bank_filename)) ||*/ (rc = g_composition->loadModel(model_bank_filename)) )
        {
          std::cout << "loadModel() = " << rc << std::endl;
          return 1;
        }
    }

  std::cout << "loadModel() succeeded with " << g_model->models().size() << " entries." << std::endl;

#if 0
  for(std::size_t k=0; k < g_model->models().size(); k++)
    {
      KnowledgeEntry * entry= g_model->models()[k];
      for(std::size_t i=0; i < entry->m_knowledgeArrayEntries.size(); i++)
        {
          KnowledgeArrayEntry *arrayEntry = entry->m_knowledgeArrayEntries[i];
          for(std::size_t j=0; j < arrayEntry->figure_list.size(); j++)
            {
              if( arrayEntry->figure_list[j]->pitchs.size() > 0 )
                printf("-=====%d %d %d\n", k, i, j);
            }
        }
    }
#endif

  //std::cout << "tst_parameter_generator() " << tst_parameter_generator() << std::endl;
  //std::cout << "tst_composition_toplevel() " << tst_composition_toplevel() << std::endl;
  std::cout << "tst_image_composition() " << tst_image_composition(argv[1]) << std::endl;
  return 0;
}
