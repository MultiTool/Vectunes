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
public:
  Config *MyProject;
  int FreshnessTimeStamp;
  int RefCount = 0;

  /* ********************************************************************************* */
  virtual OffsetBoxBase* Spawn_OffsetBox() = 0;// for compose time
  /* ********************************************************************************* */
  virtual SingerBase* Spawn_Singer() = 0;// for render time

  /* ********************************************************************************* */
  virtual SoundFloat Get_Duration() = 0;
  /* ********************************************************************************* */
  virtual SoundFloat Get_Max_Amplitude() = 0;
  /* ********************************************************************************* */
  virtual void Update_Guts(MetricsPacket& metrics) = 0;
  /* ********************************************************************************* */
  virtual void Refresh_Me_From_Beneath(IMoveable& mbox) = 0;// should be just for IContainer types, but aren't all songs containers?
  /* ********************************************************************************* */
  virtual void Set_Project(Config* project) { this->MyProject = project; }
  /* ********************************************************************************* */
  //@Override ISonglet Deep_Clone_Me(ITextable.CollisionLibrary HitTable) = 0;
  /* ********************************************************************************* */
  virtual int Ref_Songlet() {// ISonglet Reference Counting: increment ref counter and return neuvo value just for kicks
    return ++this->RefCount;
  }
  virtual int UnRef_Songlet() {// ISonglet Reference Counting: decrement ref counter and return neuvo value just for kicks
    if (this->RefCount < 0) {
      throw std::runtime_error("Voice: Negative RefCount:" + this->RefCount);
    }
    return --this->RefCount;
  }
  virtual int GetRefCount() {// ISonglet Reference Counting: get number of references for serialization
    return this->RefCount;
  }
  /* ********************************************************************************* */
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

class IContainer: public ISonglet {
public:
  /* ********************************************************************************* */
  //virtual void Refresh_Me_From_Beneath(IMoveable& mbox) = 0;// should be just for IContainer types, but aren't all songs containers?
  void Refresh_Me_From_Beneath(IMoveable& mbox) override {};// should be just for IContainer types, but aren't all songs containers?
};

/* ********************************************************************************* */
class MetricsPacket {
public:
  SoundFloat MaxDuration = 0.0;
  Config* MyProject = nullptr;
  int FreshnessTimeStamp = 1;
  void Reset(){
    MaxDuration = 0.0;
    FreshnessTimeStamp = 1;
  }
};

/* ********************************************************************************* */
class SingerBase: public IDeletable {// Cantante
public:// Cantante
  Config *MyProject;
  SoundFloat Inherited_OctaveRate = 0.0;// bend context, change dynamically while rendering. not used yet.
  /*
  InheritedMap breakdown:
  Inherited_Time = 0.0, Inherited_Octave = 0.0, Inherited_Loudness = 1.0;// time, octave, and loudness context
  Inherited_ScaleX = 1.0;// tempo rescale context
  Inherited_ScaleY = 1.0;// 'temper' context, which we will NEVER use unless we want to make ugly anharmonic noise.
  */
  MonkeyBox InheritedMap;// InheritedMap is transformation to and from samples. Replacement for Inherited_*
  int SampleRate;

  boolean IsFinished = false;
  SingerBase* ParentSinger = nullptr;
  OffsetBoxBase* MyOffsetBox = nullptr;
  /* ********************************************************************************* */
  SingerBase(){ this->Create_Me(); }
  /* ********************************************************************************* */
  ~SingerBase(){ this->Delete_Me(); }
  /* ********************************************************************************* */
  virtual void Start() = 0;
  /* ********************************************************************************* */
  virtual void Skip_To(SoundFloat EndTime) = 0;
  /* ********************************************************************************* */
  virtual void Render_To(SoundFloat EndTime, Wave& wave) = 0;
  /* ********************************************************************************* */
  virtual void Skip_To_Sample(int EndSample) = 0;
  /* ********************************************************************************* */
  virtual void Render_To_Sample(int EndSample, Wave& wave) = 0;
  /* ********************************************************************************* */
  virtual void Inherit(SingerBase& parent) {// accumulate transformations
    this->ParentSinger = &parent;
    this->InheritedMap.Copy_From(parent.InheritedMap);
    this->Compound();
  };
  /* ********************************************************************************* */
  virtual void Set_Project(Config *config){
    this->MyProject = config;
    this->SampleRate = config->SampleRate;
  }
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

