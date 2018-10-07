#ifndef ISonglet_hpp
#define ISonglet_hpp

#include "Wave.hpp"
#include "IOffsetBox.hpp"
#include "IDeletable.hpp"
#include "ITextable.hpp"
#include "IDrawable.hpp"
#include "Config.hpp"

class ISinger; // forward
//class AudProject;// forward
class MetricsPacket;// forward
class MonkeyBox;// forward

class ISonglet: public IDrawable, IDeletable, public ITextable {// Cancionita
protected:
  IOffsetBox *MyOffsetBox = null;
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
  ISinger *ParentSinger;

  /* ********************************************************************************* */
  virtual IOffsetBox* Spawn_OffsetBox() = 0;// for compose time
  /* ********************************************************************************* */
  virtual ISinger* Spawn_Singer() = 0;// for render time

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
  virtual int Ref_Songlet() = 0;// Reference Counting: increment ref counter and return new value just for kicks
  virtual int UnRef_Songlet() = 0;// Reference Counting: decrement ref counter and return new value just for kicks
  virtual int GetRefCount() = 0;// Reference Counting: get number of references for serialization
};

  /* ********************************************************************************* */
  class MetricsPacket {
  public:
    double MaxDuration = 0.0;
    Config* MyProject = nullptr;
    int FreshnessTimeStamp = 1;
  };

/* ********************************************************************************* */
class ISinger{//: public IDeletable {// Cantante
public:// Cantante
  Config *MyProject;
  double Inherited_OctaveRate = 0.0;// bend context, change dynamically while rendering. not used yet.
  MonkeyBox InheritedMap;// InheritedMap is transformation to and from samples. Replacement for Inherited_*

  boolean IsFinished = false;
  ISinger* ParentSinger;
  IOffsetBox* MyOffsetBox = nullptr;
  //MonkeyBox* MyOffsetBox = nullptr;
  /* ********************************************************************************* */
  virtual void Start() = 0;
  /* ********************************************************************************* */
  virtual void Skip_To(double EndTime) = 0;
  /* ********************************************************************************* */
  virtual void Render_To(double EndTime, Wave& wave) = 0;
  /* ********************************************************************************* */
  virtual void Inherit(ISinger& parent) = 0;
  /* ********************************************************************************* */
  virtual void Compound() = 0;
  /* ********************************************************************************* */
  virtual void Compound(MonkeyBox& donor) = 0;
  /* ********************************************************************************* */
  virtual  IOffsetBox* Get_OffsetBox() = 0;
  /* ********************************************************************************* */
  virtual bool Create_Me() = 0;
  virtual void Delete_Me() = 0;
};

#endif // ISonglet_hpp

