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
#ifndef THEORY_INSTRUMENT_H
#define THEORY_INSTRUMENT_H

#include <vector>

#include "typedefs.h"

namespace autocomp
{
class FigureListEntry;
class KnowledgeEntry;

namespace theory
{

enum FigureClass
{
  FIGURE_CLASS_CHORD = 0,
  FIGURE_CLASS_SOLO,
  FIGURE_CLASS_DEC
};

enum FigureBank
{
  FIGURE_BANK_PIANO = 0,
  FIGURE_BANK_MELODY,
  FIGURE_BANK_GUITAR,
  FIGURE_BANK_BASS,
  FIGURE_BANK_ORGAN,
  FIGURE_BANK_DRUMS,
  FIGURE_BANK_STRINGS,
  FIGURE_BANK_WIND,
  FIGURE_BANK_EFFECT,
  FIGURE_BANK_NATIONAL,
  FIGURE_BANK_UNSORTED
};

int layout_timbres(std::vector<int> &dst_timbre_banks,
                   std::vector<int> &dst_figure_banks,
                   std::vector<int> &dst_figure_classes,
                   const std::vector<int> &src_timbre_banks,
                   const std::vector<int> &src_figure_banks,
                   const std::vector<int> &src_figure_classes,
                   bool has_melody);
int get_timbres(const KnowledgeEntry *knowledgeEntry,
                   std::vector<int> &timbre_banks,
                   std::vector<int> &figure_banks,
                   std::vector<int> &figure_classes);

int get_timbre_figures(const KnowledgeEntry ** ppKnowledgeEntry,
                       int *pTrack,
                       const std::vector<const KnowledgeEntry *> &knowledge_entries, int figure_bank, int figure_class);

bool is_timbre_bank_related(int figure_bank, int dst_figure_bank);

int processVelocity(std::vector<std::vector<CompositionChainNode *>> &compositionChainTrack, const std::vector<int> &figureBanks, const std::vector<int> &figureClasses, float velocityFactor = 1.0, float soloProportion = 1.0);

}
}

#endif
