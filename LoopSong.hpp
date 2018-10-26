#ifndef LoopSong_hpp
#define LoopSong_hpp

#include "GroupSong.hpp"

/**
 *
 * @author MultiTool

   WORK IN PROGRESS - NOT FUNCTIONAL YET

*/

/* ********************************************************************************* */
class Dummy_OffsetBox_Base: public OffsetBoxBase {// used like a forward
public:
  int MyIndex;
};

/* ********************************************************************************* */
class LoopSong_Base: public GroupSong {// used like a forward
public:
  virtual void Update_Rhythm(Dummy_OffsetBox_Base& mbox){}
};

class DummySong: public ISonglet {
public:
  /* ********************************************************************************* */
  class Dummy_OffsetBox: public Dummy_OffsetBox_Base {
  public:
    LoopSong_Base *ParentLoop;
    DummySong *ContentSonglet;
    void MoveTo(double XLoc, double YLoc) override {
      if (XLoc >= 0) { this->TimeX = XLoc; }// don't go backward in time
      this->OctaveY = YLoc;// should just make this 0.
      //this->ParentLoop->Refresh_Me_From_Beneath(*this);
      this->ParentLoop->Update_Rhythm(*this);
    }
    /* ********************************************************************************* */
    SingerBase* Spawn_Singer() override {// always always always override this
      SingerBase *Singer = this->ContentSonglet->Spawn_Singer();// for render time
      Singer->MyOffsetBox = this;// Transfer all of this box's offsets to singer.
      return Singer;
    }
  };
  /* ********************************************************************************* */
  LoopSong_Base *GrandParentLoop;
  OffsetBoxBase *Child = nullptr;// any child obox->songlet
  ISonglet *ChildSong;
  /* ********************************************************************************* */
  Dummy_OffsetBox* Spawn_OffsetBox() override {// for compose time
    Dummy_OffsetBox *child = new Dummy_OffsetBox();
    child->MySonglet = child->ContentSonglet = this;
    child->ParentLoop = this->GrandParentLoop;
    return child;
  }
  /* ********************************************************************************* */
  SingerBase* Spawn_Singer() override {// for render time
    SingerBase* singer = ChildSong->Spawn_Singer();
    return singer;
  }
  /* ********************************************************************************* */
  double Get_Duration() override { return ChildSong->Get_Duration(); }
  /* ********************************************************************************* */
  double Get_Max_Amplitude() override { return ChildSong->Get_Max_Amplitude(); }
  /* ********************************************************************************* */
  void Update_Guts(MetricsPacket& metrics) override {
    ChildSong->Update_Guts(metrics);
  }
  /* ********************************************************************************* */
  void Refresh_Me_From_Beneath(IMoveable& mbox) override { ChildSong->Refresh_Me_From_Beneath(mbox); }// should be just for IContainer types, but aren't all songs containers?
};// end of DummySong

/* ********************************************************************************* */
/* ********************************************************************************* */

class LoopSong: public LoopSong_Base {
public:
/*
general plan:
create a groupsong where all children are the same songlet, spaced evenly in time.
any update to the location of one child triggers a callback to keep them all evenly spaced/rhythmic.
DummySong is the most rigorous approach but inherits too much interface junk to fill in.  Is it needed, can we strip it out?

*/
  /* ********************************************************************************* */
  class Loop_OffsetBox: public Group_OffsetBox {
  public:
  };
  /* ********************************************************************************* */
  DummySong SingleSong;// the one child song that gets repeated, which in turn owns the custom song we are given.
  double Interval = 0.25;// time spacing
  /* ********************************************************************************* */
  LoopSong(): LoopSong_Base() {
    this->SingleSong.GrandParentLoop = this;
    this->Create_Me();
  }
  ~LoopSong(){
    this->Delete_Me();
  }
  /* ********************************************************************************* */
  void Set_Interval(double Interval0){
    this->Interval = Interval0;
    OffsetBoxBase *obox;
    int NumKids = this->SubSongs.size();
    for (int cnt=0;cnt<NumKids;cnt++){
      obox = this->SubSongs.at(cnt);
      obox->TimeX = this->Interval * (double)cnt;
    }
  }
  /* ********************************************************************************* */
  void Set_Beats(int NumBeats){
    DummySong::Dummy_OffsetBox *dsobox;
    OffsetBoxBase *RepeatHandle;
    int PrevSize = this->SubSongs.size();
    if (NumBeats>PrevSize){// add beats
      this->SubSongs.resize(NumBeats);
      for (int cnt=PrevSize;cnt<NumBeats;cnt++){
        dsobox = this->SingleSong.Spawn_OffsetBox();
        dsobox->MyIndex = cnt;
        dsobox->TimeX = Interval * (double)cnt;// to do: Assign dsobox OctaveY and TimeX here
        dsobox->OctaveY = 0.0;// snox is this good enough?
        //this->SubSongs.at(cnt) = dsobox;
        this->SubSongs[cnt] = dsobox;
      }
    } else if (NumBeats<PrevSize){// remove beats
      for (int cnt=NumBeats;cnt<PrevSize;cnt++){
        RepeatHandle = this->SubSongs.at(cnt);
        delete RepeatHandle;
      }
      this->SubSongs.resize(NumBeats);
    }
    Refresh_Splines();
  }
  int Add_SubSong(OffsetBoxBase *obox) override {
    int dex = 0;// When someone gives me an OffsetBox, I own it.  I must delete it.
    delete this->SingleSong.Child;// It is my responsibility to delete the handle I was given.
    this->SingleSong.Child = obox;// Now assign the new handle.
    this->SingleSong.ChildSong = obox->GetContent();
    return dex;
  }
  /* ********************************************************************************* */
  void Update_Rhythm(Dummy_OffsetBox_Base& mbox) override {
    int Index = mbox.MyIndex;// like Refresh_Me_From_Beneath but specific for Dummy_OffsetBox.
    this->Interval = mbox.TimeX / (double)Index;
    OffsetBoxBase *obox;// = this->SubSongs.at(Index);
    for (int cnt=0;cnt<Index;cnt++){
      obox = this->SubSongs.at(cnt);
      obox->OctaveY = mbox.OctaveY;
      obox->TimeX = this->Interval * (double)cnt;
    }
    Index++;
    for (int cnt=Index;cnt<this->SubSongs.size();cnt++){
      obox = this->SubSongs.at(cnt);
      obox->OctaveY = mbox.OctaveY;
      obox->TimeX = this->Interval * (double)cnt;
    }
  }
  /* ********************************************************************************* */
  void Refresh_Me_From_Beneath(IMoveable& mbox) override {
    double XLoc = mbox.GetX();
    double YLoc = mbox.GetY();
    int NumSubSongs = this->SubSongs.size();
    for (int cnt = 0; cnt < NumSubSongs; cnt++) {
      OffsetBoxBase *obx = this->SubSongs.at(cnt);
      ISonglet *songlet = obx->GetContent();
      //obx->TimeX =
      obx->OctaveY = YLoc;
    }
  }
  /* ********************************************************************************* */
  Loop_OffsetBox* Spawn_OffsetBox() override {
    Loop_OffsetBox *lbox = new Loop_OffsetBox();// Spawn an OffsetBox specific to this type of songlet.
    lbox->Attach_Songlet(this);
    return lbox;
  }
};

#endif// LoopSong_hpp
