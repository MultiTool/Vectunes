#ifndef LoopBox_hpp
#define LoopBox_hpp

#include "GroupBox.hpp"

/**
 *
 * @author MultiTool
*/

class LoopBox: public GroupBox {
public:
/*
general plan:
create a groupbox where all children are the same songlet, spaced evenly in time.
any update to the location of one child triggers a callback to keep them all evenly spaced/rhythmic.
*/
  class Dummy_Songlet: public ISonglet {
  public:
    /* ********************************************************************************* */
    class Dummy_Songlet_OffsetBox: public OffsetBoxBase {
    public:
      LoopBox *lbox;
      Dummy_Songlet *ContentSonglet;
    };
    LoopBox *Parent;
    OffsetBoxBase *Child;// any child obox->songlet
    /* ********************************************************************************* */
    Dummy_Songlet_OffsetBox* Spawn_OffsetBox() override {
      Dummy_Songlet_OffsetBox *child = new Dummy_Songlet_OffsetBox();
      child->ContentSonglet = this;
      child->lbox = this->Parent;
      return child;
    }
  };
  // Dummy_Songlet SingleSong; to do: make Dummy_Songlet not abstract
  /* ********************************************************************************* */
  class LoopBox_OffsetBox: public Group_OffsetBox {
  public:
  };
  void Add_SubSong(OffsetBoxBase* obox) override {
    /*
    if Dummy_Songlet/SingleSong already has a child, unref it.
    add obox as the child to single Dummy_Songlet
    spawn a Dummy_Songlet_OffsetBox from Dummy_Songlet
    attach Dummy_Songlet_OffsetBox to me (LoopBox)

    */
    Refresh_Splines();// maybe sort_me and refresh_splines should be in update_guts instead?
  }
  /* ********************************************************************************* */
  LoopBox_OffsetBox* Spawn_OffsetBox() override {
    LoopBox_OffsetBox *lbox = new LoopBox_OffsetBox();// Spawn an OffsetBox specific to this type of songlet.
    lbox->Attach_Songlet(this);
    return lbox;
  }
};

#endif// LoopBox_hpp
