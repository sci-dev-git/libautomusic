#ifndef LIBAUTOMUSIC_TYPEDEFS_H
#define LIBAUTOMUSIC_TYPEDEFS_H

#include <stdint.h>

namespace autocomp
{

class ChordPair
{
public:
  ChordPair(unsigned int root, unsigned int sign)
    : root(root),
      sign(sign)
  {}

  bool operator==(const ChordPair &chord)
    {
      return root == chord.root && sign == chord.sign;
    }
  bool operator!=(const ChordPair &chord)
    {
      return root != chord.root || sign != chord.sign;
    }
public:
  int root;
  int sign;
};

class PitchNote
{
public:
  PitchNote(unsigned int pitch, unsigned int velocity, unsigned int start, unsigned int end)
    : pitch(pitch),
      velocity(velocity),
      start(start),
      end(end)
  {}
  PitchNote()
    : pitch(0),
      velocity(0),
      start(0),
      end(0)
  {}
public:
  uint8_t pitch;
  uint8_t velocity;
  int32_t start;
  int32_t end;
};

class StructureForm
{
public:
  enum FormType
  {
    FORM_BLANK = 0,
    FORM_PRELUDE = 1,
    FORM_VERSE11 = 2, FORM_VERSE12 = 3,
    FORM_BRIDGE1 = 4,
    FORM_CHORUS11 = 5, FORM_CHORUS12 = 6,
    FORM_INTERLUDE1 = 7,
    FORM_INTERLUDE2 = 8,
    FORM_VERSE21 = 9, FORM_VERSE22 = 10,
    FORM_CHORUS21 = 11, FORM_CHORUS22 = 12,
    FORM_ENDING = 13,
    FORM_VERSE13 = 14, FORM_VERSE23 = 15, FORM_VERSE31 = 16, FORM_VERSE32 = 17, FORM_VERSE33 = 18,
    FORM_CHORUS31 = 19, FORM_CHORUS32 = 20,
    FORM_BRIDGE2 = 21,
    FORM_TRANS1 = 22, FORM_TRANS2 = 23, FORM_TRANS3 = 24,
    FORM_BRIDGE3 = 25,
    FORM_INVALID = -1
  };
  explicit StructureForm(FormType type) : m_type(type), m_bars(0), m_begin(0), m_end(0)
  {}
  StructureForm(FormType type, int bars) : m_type(type), m_bars(bars), m_begin(0), m_end(0)
  {}
  StructureForm(FormType type, int bars, int begin, int end) : m_type(type), m_bars(bars), m_begin(begin), m_end(end)
  {}
  StructureForm() : m_type(FORM_BLANK), m_bars(0), m_begin(0), m_end(0)
  {}
  StructureForm(const StructureForm &form) : m_type(form.m_type), m_bars(form.m_bars), m_begin(form.m_begin), m_end(form.m_end)
  {}

  inline FormType type() const  { return m_type; }
  inline void setType(FormType type)    { m_type = type; }
  inline int bars() const       { return m_bars; }
  inline void setBars(int bars)         { m_bars = bars; }
  inline int begin() const      { return m_begin; }
  inline void setBegin(int begin)       { m_begin = begin; }
  inline int end() const        { return m_end; }
  inline void setEnd(int end)           { m_end = end; }

private:
  FormType m_type;
  int m_bars;
  int m_begin, m_end;
};

class FigureListEntry;
class FormChainNode
{
public:
  FormChainNode()
    : figure(0l), offset(0), key(0) {}
  FormChainNode(const FormChainNode &formChain)
    : form(formChain.form),
      chords(formChain.chords),
      figure(formChain.figure), offset(formChain.offset), key(formChain.key)
  {}
public:
  StructureForm form;
  std::vector<ChordPair> chords;
  const FigureListEntry *figure;
  int offset;
  int key;
};

class CompositionChainNode : public FormChainNode
{
public:
  CompositionChainNode(const FormChainNode &formChain) :
      FormChainNode(formChain)
  {}
public:
  std::vector<PitchNote> pitch;
};

}

/* look out the side effort of the following macros! */
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif
#ifndef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif

#endif
