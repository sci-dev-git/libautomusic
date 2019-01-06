#ifndef THEORY_STRUCTURE_H
#define THEORY_STRUCTURE_H

#include <vector>
#include "typedefs.h"

namespace autocomp
{
class FigureListEntry;

namespace theory
{

int stretch_figure_sequence(std::vector<PitchNote> &dst_figure_list,
                         const std::vector<PitchNote> &src_figure_list,
                         int src_barlen, int dst_barlen, int beats = 4);

int stretch_chord_sequence(std::vector<ChordPair> &dst,
                        const std::vector<ChordPair> &src_chord_list,
                        int src_barlen, int dst_barlen,
                        int beats = 4);


int get_form_template(std::vector<StructureForm> &dst, unsigned int id);

const FigureListEntry *pick_form(StructureForm::FormType form, const std::vector<const FigureListEntry *> &forms_vector);
int transform_figure_4_3(std::vector<PitchNote> &dst, const std::vector<PitchNote> &figures, int bars);

extern const StructureForm::FormType form_replacement_rules[][6];

}
}

#endif
