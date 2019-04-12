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
#include <cstdio>
#include <cstring>

#ifndef DLL_EXPORT
# define DLL_EXPORT /* normally this will be defined by libtool automatically */
#endif
#include "config.h"
#include "libautomusic.h"

#include "libautomusic.h"
#include "knowledge-model.h"
#include "parameter-generator.h"
#include "theory-harmonics.h"
#include "output-base.h"
#include "util-randomize.h"
#include "composition-toplevel.h"

#define CURRENT_VERSION_MAJOR 1
#define CURRENT_VERSION_MINOR 0
#define CURRENT_VERSION_REVISON 0

#define BEAT_TYPE 4

struct am_context_s
{
  autocomp::CompositionToplevel *composition;
  autocomp::Output *output;
};

int
LIBAM_EXPORT(libam_require_version)(int major, int minor, int revsion)
{
  return (major <= CURRENT_VERSION_MAJOR && minor <= CURRENT_VERSION_MINOR && revsion <= CURRENT_VERSION_REVISON) ? 0 : -RC_FAILED;
}

am_context_t *
LIBAM_EXPORT(libam_create_context)(const char *modelPath)
{
  am_context_t *context = new am_context_t;
  std::memset(context, 0, sizeof(*context));
  
  context->composition = new autocomp::CompositionToplevel;
  context->output = new autocomp::Output;
  
  /*
   * Load all the models in database to memory.
   */
  if( context->composition->knowledgeModel()->loadModels(modelPath) )
    {
      libam_free_context(context);
      return 0l;
    }
  return context;
}

int
LIBAM_EXPORT(libam_composite_by_image)(am_context_t *context, int form_template_index, int beats, const char *filename)
{
  if( int err = context->composition->generator()->gen(filename, form_template_index, beats) )
    return err;
  if( int err = context->composition->startup() )
    return err;
  
  return 0;
}

int
LIBAM_EXPORT(libam_output_file)(am_context_t *context, int filetype, const char *filename)
{
  return context->output->outputCompositionChain(filename, filetype,
      context->composition->chains(),
      context->composition->generator()->timbreBanks(),
      context->composition->generator()->figureBanks(), context->composition->generator()->figureClasses(),
      BEAT_TYPE,
      context->composition->generator()->beats(), context->composition->tempo());
}

void
LIBAM_EXPORT(libam_free_context)(am_context_t *context)
{
  delete context->output;
  delete context->composition;
  delete context;
}
