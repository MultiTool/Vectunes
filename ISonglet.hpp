#ifndef ISonglet_hpp
#define ISonglet_hpp

#include "Wave.hpp"
#include "OffsetBoxBase.hpp"
#include "IDeletable.hpp"
#include "ITextable.hpp"
#include "IDrawable.hpp"
#include "Config.hpp"

class SingerBase; // forward
//class AudProject;// forward
class MetricsPacket;// forward
class MonkeyBox;// forward

class ISonglet: public IDrawable, IDeletable, public ITextable {// Cancionita
protected:
  OffsetBoxBase *MyOffsetBox = null;
public:
  Config *MyProject;
  double Inherited_OctaveRate = 0.0;// bend context, change dynamically while rendering. not used yet.
  MonkeyBox *InheritedMap = nullptr;// InheritedMap is transformation to and from samples. Replacement for Inherited_*
  /*
  InheritedMap breakdown:
  Inherited_Time = 0.0, Inherited_Octave = 0.0, Inherited_Loudness = 1.0;// time, octave, and loudness context
  Inherited_ScaleX = 1.0;// tempo rescale context
  Inherited_ScaleY = 1.0;// 'temper' context, which we will NEVER use unless we want to make ugly anharmonic noise.
  */
  boolean IsFinished = false;
  SingerBase *ParentSinger;
  int RefCount = 0;

  /* ********************************************************************************* */
  virtual OffsetBoxBase* Spawn_OffsetBox() = 0;// for compose time
  /* ********************************************************************************* */
  virtual SingerBase* Spawn_Singer() = 0;// for render time

  /* ********************************************************************************* */
  virtual int Get_Sample_Count(int SampleRate) = 0;
  /* ********************************************************************************* */
  virtual double Get_Duration() = 0;
  /* ********************************************************************************* */
  virtual double Get_Max_Amplitude() = 0;
  /* ********************************************************************************* */
  virtual double Update_Durations() = 0;
  /* ********************************************************************************* */
  virtual void Update_Guts(MetricsPacket& metrics) = 0;
  /* ********************************************************************************* */
  virtual void Refresh_From_Beneath(IMoveable& mbox) = 0;
  /* ********************************************************************************* */
  virtual void Sort_Me() = 0;
  /* ********************************************************************************* */
  virtual Config* Get_Project() = 0;
  /* ********************************************************************************* */
  virtual void Set_Project(Config* project) = 0;
  /* ********************************************************************************* */
  //@Override ISonglet Deep_Clone_Me(ITextable.CollisionLibrary HitTable) = 0;

  /* ********************************************************************************* */
  virtual int Ref_Songlet() = 0;// Reference Counting: increment ref counter and return neuvo value just for kicks
  virtual int UnRef_Songlet() = 0;// Reference Counting: decrement ref counter and return neuvo value just for kicks
  virtual int GetRefCount() = 0;// Reference Counting: get number of references for serialization
  static int Unref(ISonglet** SongletPointerPointer){// Failed(?) experiment. Should probably delete this.
    ISonglet *songlet = *SongletPointerPointer;
    if (songlet==nullptr){ return -1; }
    int NumLeft = songlet->UnRef_Songlet();
    if (NumLeft<=0){
       delete songlet;
       *SongletPointerPointer = nullptr;
    }
    return NumLeft;
  }
};

  /* ********************************************************************************* */
  class MetricsPacket {
  public:
    double MaxDuration = 0.0;
    Config* MyProject = nullptr;
    int FreshnessTimeStamp = 1;
  };

/* ********************************************************************************* */
class SingerBase: public IDeletable {// Cantante
public:// Cantante
  Config *MyProject;
  double Inherited_OctaveRate = 0.0;// bend context, change dynamically while rendering. not used yet.
  MonkeyBox InheritedMap;// InheritedMap is transformation to and from samples. Replacement for Inherited_*

  boolean IsFinished = false;
  SingerBase* ParentSinger;
  OffsetBoxBase* MyOffsetBox = nullptr;
  //MonkeyBox* MyOffsetBox = nullptr;
  /* ********************************************************************************* */
  SingerBase(){ this->Create_Me(); }
  /* ********************************************************************************* */
  ~SingerBase(){ this->Delete_Me(); }
  /* ********************************************************************************* */
  virtual void Start() {};
  /* ********************************************************************************* */
  virtual void Skip_To(double EndTime) {};
  /* ********************************************************************************* */
  virtual void Render_To(double EndTime, Wave& wave) {};
  /* ********************************************************************************* */
  virtual void Inherit(SingerBase& parent) {// accumulate transformations
    this->ParentSinger = &parent;
    this->InheritedMap.Copy_From(parent.InheritedMap);
    this->Compound();
  };
  /* ********************************************************************************* */
  virtual void Compound() {// accumulate my own transformation
    this->Compound(*(this->Get_OffsetBox()));
  };
  /* ********************************************************************************* */
  virtual void Compound(MonkeyBox& donor) {// accumulate my own transformation
    this->InheritedMap.Compound(donor);// to do: combine matrices here.
  };
  /* ********************************************************************************* */
  virtual OffsetBoxBase* Get_OffsetBox() {
    return this->MyOffsetBox;
  };
  /* ********************************************************************************* */
  bool Create_Me() override {// IDeletable
    return true;
  };
  void Delete_Me() override {// IDeletable
    this->MyProject = null;// wreck everything
    this->Inherited_OctaveRate = Double_NEGATIVE_INFINITY;
    this->InheritedMap.Delete_Me();// should be redundant
    this->IsFinished = true;
    this->ParentSinger = null;
    this->MyOffsetBox = null;
  };
};

#endif // ISonglet_hpp

