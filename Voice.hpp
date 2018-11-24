#ifndef Voice_hpp
#define Voice_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include <algorithm>    // std::find
#include "Globals.hpp"
#include "Wave.hpp"
#include "CajaDelimitadora.hpp"
#include "ISonglet.hpp"
#include "IDrawable.hpp"
#include "VoicePoint.hpp"
#include "OffsetBoxBase.hpp"
#include "Config.hpp"
#include "IGrabber.hpp"

/**
 *
 * @author MultiTool
*/

bool Voice_Iterative = false;// temporary variable for benchmarking
//class VoicePoint;// forward
//class Voice_OffsetBox;// forward
//class Voice_Singer;// forward
typedef Voice* VoicePtr;
class Voice: public ISonglet{//, IDrawable {// collection of control points, each one having a pitch and a volume. rendering morphs from one cp to another.
public:
  //static bool Iterative;
  ArrayList<VoicePoint*> CPoints;
  String CPointsName = "ControlPoints";// for serialization
  SoundFloat MaxAmplitude;
  SoundFloat BaseFreq = Globals::BaseFreqC0;
  SoundFloat ReverbDelay = 0.125 / 4.0;// delay in seconds
  //int RefCount = 0;
  // graphics support
  CajaDelimitadora MyBounds;
  Color* FillColor;
  static constexpr SoundFloat Filler = 0.3;
  /* ********************************************************************************* */
  Voice() { FreshnessTimeStamp = 0; }
  ~Voice(){this->Delete_Me();}
  /* ********************************************************************************* */
  void Add_Note(VoicePoint* pnt) {
    pnt->RefParent(this);
    this->CPoints.push_back(pnt);
  }
  /* ********************************************************************************* */
  VoicePoint* Add_Note(SoundFloat RealTime, SoundFloat Octave, SoundFloat Loudness) {
    VoicePoint *pnt = new VoicePoint();// decide whether vp is created/deleted by the voice, or someone else.
    pnt->TimeX = RealTime;
    pnt->OctaveY = Octave;
    pnt->SubTime = 0.0;
    pnt->LoudnessFactor = Loudness;
    this->Add_Note(pnt);
    return pnt;
  }
  /* ********************************************************************************* */
  void Remove_Note(VoicePoint *pnt) {
    this->CPoints.remove(pnt);// decide whether vp is created/deleted by the voice, or someone else.
  }
  /* ************************************************************************************************************************ */
  int Tree_Search(SoundFloat Time, int minloc, int maxloc) {// finds place where time would be inserted or replaced
    int medloc;
    while (minloc < maxloc) {
      medloc = (minloc + maxloc) >> 1; // >>1 is same as div 2, only faster.
      if (Time <= this->CPoints.at(medloc)->TimeX) {
        maxloc = medloc;
      }/* has to go through here to be found. */ else {
        minloc = medloc + 1;
      }
    }
    return minloc;
  }
  /* ********************************************************************************* */
  SoundFloat Get_Duration() override {
    int len = this->CPoints.size();
    if (len <= 0) {
      return 0;
    }
    VoicePoint *Final_Point = this->CPoints.at(len - 1);
    return Final_Point->TimeX + this->ReverbDelay;
  }
  /* ********************************************************************************* */
//  SoundFloat Update_Durations() override {
//    return this->Get_Duration();// this is not a container, so just return what we already know
//  }
  /* ********************************************************************************* */
  SoundFloat Get_Max_Amplitude() override {
    return this->MaxAmplitude;
  }
  /* ********************************************************************************* */
  void Update_Max_Amplitude() {
    int len = this->CPoints.size();
    VoicePoint* pnt;
    SoundFloat MaxAmp = 0.0;
    for (int pcnt = 0; pcnt < len; pcnt++) {
      pnt = this->CPoints.at(pcnt);
      if (MaxAmp < pnt->LoudnessFactor) {
        MaxAmp = pnt->LoudnessFactor;
      }
    }
    this->MaxAmplitude = MaxAmp;
  }
  /* ********************************************************************************* */
  void Update_Guts(MetricsPacket& metrics) override {
    if (this->FreshnessTimeStamp != metrics.FreshnessTimeStamp) {// don't hit the same songlet twice on one update
      this->Set_Project(metrics.MyProject);
      this->Sort_Me();
      this->Recalc_Line_SubTime();
      this->Update_Max_Amplitude();
      this->FreshnessTimeStamp = metrics.FreshnessTimeStamp;
    }
    metrics.MaxDuration = this->Get_Duration();
  }
  /* ********************************************************************************* */
  void Refresh_Me_From_Beneath(IMoveable& mbox) override {}
  /* ********************************************************************************* */
  void Sort_Me() {//override {// sorting by TimeX
    std::sort(this->CPoints.begin(), this->CPoints.end(), ComparePoints);
  }
  /* ********************************************************************** */
  static bool ComparePoints(VoicePoint* vp0, VoicePoint* vp1) {
    return vp0->TimeX < vp1->TimeX;
  }
  /* ********************************************************************************* */
  //void Set_Project(Config* project) override { this->MyProject = project; }
  /* ********************************************************************************* */
  void Recalc_Line_SubTime() {
    SoundFloat SubTimeLocal;// run this function whenever this voice instance is modified, e.g. control points moved, added, or removed.
    int len = this->CPoints.size();
    if (len < 1) { return; }
    this->Sort_Me();
    VoicePoint Dummy_First, *Prev_Point, *Next_Point;
    Next_Point = this->CPoints.at(0);
    Dummy_First.CopyFrom(*Next_Point);
    Dummy_First.SubTime = Dummy_First.TimeX = 0.0;// Times must both start at 0, even though user may have put the first audible point at T greater than 0.
    Next_Point = &Dummy_First;
    for (int pcnt = 0; pcnt < len; pcnt++) {
      Prev_Point = Next_Point;
      Next_Point = this->CPoints.at(pcnt);
      SoundFloat FrequencyFactorStart = Prev_Point->GetFrequencyFactor();
      SoundFloat TimeRange = Next_Point->TimeX - Prev_Point->TimeX;
      SoundFloat OctaveRange = Next_Point->OctaveY - Prev_Point->OctaveY;
      if (TimeRange == 0.0) {
        TimeRange = Globals::Fudge;// Fudge to avoid div by 0
      }
      SoundFloat OctaveRate = OctaveRange / TimeRange;// octaves per second
      if (OctaveRate == 0.0){
        SubTimeLocal = TimeRange;// snox is using TimeRange right?
      }else{
        SubTimeLocal = Voice::Integral(OctaveRate, TimeRange);
      }
      Next_Point->SubTime = Prev_Point->SubTime + (FrequencyFactorStart * SubTimeLocal);
    }
  }
  /* ********************************************************************************* */
  static SoundFloat Integral(SoundFloat OctaveRate, SoundFloat TimeAlong) {// to do: optimize this!
    SoundFloat SubTimeCalc;// given realtime passed and rate of octave change, use integration to get the sum of all subjective time passed.
    //return TimeAlong;// snox for testing.  removing integral about doubles speed
    //if (OctaveRate == 0.0) { return TimeAlong; }// do we really have to check this for every sample? more efficient to do it once up front.
    // Yep calling log and pow functions for every sample generated is expensive. We will have to optimize later.
    SoundFloat Denom = (std::log(2.0) * OctaveRate);// returns the integral of (2 ^ (TimeAlong * OctaveRate))
    //SoundFloat Denom = 1.0;// snox for testing.  doing this might maybe be about 0.05(?) seconds faster for a ~1.4 second render
    //SubTimeCalc = (Math::pow(2.0, (TimeAlong * OctaveRate)) / Denom) - (1.0 / Denom);
    //SubTimeCalc = Denom;// snox for testing
    //SubTimeCalc = ((Math::pow(2.0, (TimeAlong * OctaveRate)) - 1.0) / Denom);
    SubTimeCalc = ((std::pow(2.0, (TimeAlong * OctaveRate)) - 1.0) / Denom);// This one line eats more than half our speed.
    return SubTimeCalc;
  }
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {}
  /* ********************************************************************************* */
  CajaDelimitadora* GetBoundingBox() override {// IDrawable
    return &(this->MyBounds);
  }
  /* ********************************************************************************* */
  void UpdateBoundingBox() override {// IDrawable
    VoicePoint *pnt;
    //this->MyBounds.Reset();
    int len = this->CPoints.size();
    for (int pcnt = 0; pcnt < len; pcnt++) {
      pnt = this->CPoints.at(pcnt);
      pnt->UpdateBoundingBox();
      //this->MyBounds.Include(pnt.MyBounds);// Don't have to UnMap in this case because my points are already in my internal coordinates.
    }
    this->UpdateBoundingBoxLocal();
  }
  void UpdateBoundingBoxLocal() override {// IDrawable
    VoicePoint *pnt;
    this->MyBounds.Reset();
    int len = this->CPoints.size();
    for (int pcnt = 0; pcnt < len; pcnt++) {
      pnt = this->CPoints.at(pcnt);
      this->MyBounds.Include(pnt->MyBounds);// Don't have to UnMap in this case because my points are already in my internal coordinates.
    }
  }
  /* ********************************************************************************* */
  void GoFishing(IGrabber& Scoop) override {// IDrawable
    //System.out.print(" Voice GoFishing: ");
    if (Scoop.Intersects(MyBounds)) {
      int len = this->CPoints.size();
      VoicePoint *pnt;
      for (int pcnt = 0; pcnt < len; pcnt++) {// search my children
        //System.out.print("" + pcnt + ", ");
        pnt = this->CPoints.at(pcnt);
        pnt->GoFishing(Scoop);
      }
    }
    //System.out.println();
  }
  /* ********************************************************************************* */
  Voice* Clone_Me() override {// ICloneable
    Voice *child = new Voice();// clones
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  Voice* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    Voice *child;
    /*
     the idea is for hit table to be keyed by my actual identity (me), while the value
     is my first and only clone.
     */
    CollisionItem *ci = HitTable.GetItem(this);
    if (ci == null) {// not seen before, create a neuvo instance
      child = new Voice();// clone
      ci = HitTable.InsertUniqueInstance(this);
      ci->Item = child;
      child->Copy_From(*this);
      child->Copy_Children(*this, HitTable);
    } else {// pre exists
      child = (VoicePtr) ci->Item;// another cast!
    }
    return child;
  }
  /* ********************************************************************************* */
  void Copy_Children(Voice& donor, CollisionLibrary& HitTable) {
    VoicePoint* vpnt;
    int len = donor.CPoints.size();
    for (int cnt = 0; cnt < len; cnt++) {
      vpnt = donor.CPoints.at(cnt);
      this->Add_Note(vpnt->Deep_Clone_Me(HitTable));
    }
  }
  /* ********************************************************************************* */
  void Copy_From(const Voice& donor) {
    this->BaseFreq = donor.BaseFreq;
    this->MyProject = donor.MyProject;
    this->MaxAmplitude = donor.MaxAmplitude;
    this->FreshnessTimeStamp = 0;
    this->MyBounds.Copy_From(donor.MyBounds);
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {// IDeletable
    return true;
  }
  void Delete_Me() override {// IDeletable
    this->BaseFreq = Double_NEGATIVE_INFINITY;
    this->MyProject = null;
    this->MaxAmplitude = Double_NEGATIVE_INFINITY;
    this->FreshnessTimeStamp = Integer_MIN_VALUE;
    {
      this->MyBounds.Delete_Me();
    }
    {
      this->Wipe_CPoints();
    }
    this->ReverbDelay = Double_NEGATIVE_INFINITY;
    this->RefCount = Integer_MIN_VALUE;
    this->FillColor = null;
  }
  void Wipe_CPoints() {
    int len = this->CPoints.size();
    for (int cnt = 0; cnt < len; cnt++) {
      // this->CPoints.at(cnt)->Delete_Me(); // put this in voicepoint destructor
      delete this->CPoints.at(cnt);
    }
    this->CPoints.clear();
  }
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) {
    return nullptr;
  }
  void ShallowLoad(JsonParse::Node& phrase) {}
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
  /* ********************************************************************************* */
  HashMap<String, JsonParse::Node>* SerializeMyContents(CollisionLibrary& HitTable) {
    return nullptr;
  }
  /* ********************************************************************************* */
  class Voice_Singer: public SingerBase {
  public:
    Voice* MyVoice;
    SoundFloat Phase, Cycles;// Cycles is the number of cycles we've rotated since the start of this voice. The fractional part is the phase information.
    SoundFloat SubTime;// Subjective time.
    SoundFloat Current_Octave, Current_Frequency;
    int Prev_Point_Dex, Next_Point_Dex;
    int Render_Sample_Count;// number of samples since start of render
    VoicePoint Cursor_Point;
    int Bone_Sample_Mark = 0;// number of samples since time 0 absolute, not local
    int Sample_Start;
    SoundFloat BaseFreq;
    /* ********************************************************************************* */
    Voice_Singer() { this->Current_Frequency = 440; }
    /* ********************************************************************************* */
    ~Voice_Singer() {
      Delete_Me();
    }
    /* ********************************************************************************* */
    void Start() override {
      this->SubTime = 0.0; this->Phase = 0.0; this->Cycles = 0.0;
      this->Prev_Point_Dex = 0;//this->Parent.CPoints.at(0);
      this->Next_Point_Dex = 1;
      this->Render_Sample_Count = 0;
      this->Bone_Sample_Mark = 0;
      this->Sample_Start = 0;
      if (this->MyVoice->CPoints.size() < 2 || this->InheritedMap.LoudnessFactor == 0.0) {
        this->IsFinished = true;// muted, so don't waste time rendering
      } else {
        this->IsFinished = false;
        VoicePoint* pnt = this->MyVoice->CPoints.at(0);
        this->Bone_Sample_Mark = (int) (this->InheritedMap.UnMapTime(pnt->TimeX) * this->SampleRate);
        VoicePoint *ppnt = this->MyVoice->CPoints.at(this->Prev_Point_Dex);
        this->Cursor_Point.CopyFrom(*ppnt);
        SoundFloat UnMapped_Prev_Time = this->InheritedMap.UnMapTime(this->Cursor_Point.TimeX);// get start time in global coordinates
        this->Sample_Start = (int)(UnMapped_Prev_Time * (double)this->SampleRate);
      }
    }
    /* ********************************************************************************* */
    void Skip_To(SoundFloat EndTime) override {// to do: rewrite this to match bug-fixed render_to
      if (this->IsFinished){ return; }
      VoicePoint *Prev_Point = null, *Next_Point = null;
      EndTime = this->MyOffsetBox->MapTime(EndTime);// EndTime is now time internal to voice's own coordinate system
      this->Render_Sample_Count = 0;
      EndTime = this->ClipTime(EndTime);
      this->Sample_Start = EndTime * (SoundFloat)this->SampleRate;
      Prev_Point = &(this->Cursor_Point);
      int pdex = this->Next_Point_Dex;
      Next_Point = this->MyVoice->CPoints.at(pdex);
      while (Next_Point->TimeX < EndTime) {
        pdex++;
        Prev_Point = Next_Point;
        Next_Point = this->MyVoice->CPoints.at(pdex);
      }
      this->Next_Point_Dex = pdex;
      this->Prev_Point_Dex = this->Next_Point_Dex - 1;

      // deal with loose end.
      if (EndTime <= Next_Point->TimeX) {
        if (Prev_Point->TimeX <= EndTime) {// EndTime is inside this box.
          this->Cursor_Point.CopyFrom(*Prev_Point);// this section should always be executed, due to time clipping
          Interpolate_ControlPoint(*Prev_Point, *Next_Point, EndTime, this->Cursor_Point);
        }
      }
      int EndSample = (int) (EndTime * this->SampleRate);// absolute
      this->Bone_Sample_Mark = EndSample;
    }
    /* ********************************************************************************* */
    void Render_To(SoundFloat EndTime, Wave& wave) override { // ready for test
      if (this->IsFinished) {
        wave.Init(0, 0, this->SampleRate, Filler);// we promise to return a blank wave
        return;
      }
      VoicePoint *Prev_Point = null, *Next_Point = null;
      EndTime = this->MyOffsetBox->MapTime(EndTime);// EndTime is now time internal to voice's own coordinate system
      SoundFloat UnMapped_Prev_Time = this->InheritedMap.UnMapTime(this->Cursor_Point.TimeX);// get start time in global coordinates
      this->Render_Sample_Count = 0;
      int NumBends = this->MyVoice->CPoints.size();
      EndTime = this->ClipTime(EndTime);
      SoundFloat UnMapped_EndTime = this->InheritedMap.UnMapTime(EndTime);

      int Sample_End = UnMapped_EndTime * (SoundFloat)this->SampleRate;
      if (true){
        wave.Init_Sample(this->Sample_Start, Sample_End, this->SampleRate, Filler);// wave times are in global coordinates because samples are always real time
      }else{
        wave.Init(UnMapped_Prev_Time, UnMapped_EndTime, this->SampleRate, Filler);// wave times are in global coordinates because samples are always real time
      }

      Prev_Point = &this->Cursor_Point;
      int pdex = this->Next_Point_Dex;

      if (true) {
        Next_Point = this->MyVoice->CPoints.at(pdex);
        while (Next_Point->TimeX < EndTime) {
          Render_Segment_Integral(*Prev_Point, *Next_Point, wave);
          pdex++;
          Prev_Point = Next_Point;
          Next_Point = this->MyVoice->CPoints.at(pdex);
        }
        this->Next_Point_Dex = pdex;
      } else {
        while (this->Next_Point_Dex < NumBends) {
          Next_Point = this->MyVoice->CPoints.at(this->Next_Point_Dex);
          if (EndTime < Next_Point->TimeX) {// repeat until control point time overtakes EndTime
            break;
          }
          this->Prev_Point_Dex = this->Next_Point_Dex - 1;
          Prev_Point = this->MyVoice->CPoints.at(this->Prev_Point_Dex);
          Render_Segment_Integral(*Prev_Point, *Next_Point, wave);
          this->Next_Point_Dex++;
        }
      }

      this->Prev_Point_Dex = this->Next_Point_Dex - 1;

      // render loose end.
      if (EndTime <= Next_Point->TimeX) {
        if (Prev_Point->TimeX <= EndTime) {// EndTime is inside this box.
          VoicePoint End_Cursor;// this section should always be executed, due to time clipping
          End_Cursor.CopyFrom(*Prev_Point);
          Interpolate_ControlPoint(*Prev_Point, *Next_Point, EndTime, End_Cursor);
          Render_Segment_Integral(*Prev_Point, End_Cursor, wave);
          this->Cursor_Point.CopyFrom(End_Cursor);
        }
      }
      wave.Amplify(this->MyOffsetBox->LoudnessFactor);
      UnMapped_Prev_Time = UnMapped_EndTime;

      this->Sample_Start = Sample_End;
    }

    /* ********************************************************************************* */
    void Skip_To_Sample(int EndSample) override {
    }
    /* ********************************************************************************* */
    void Render_To_Sample(int EndSample, Wave& wave) override {
    }
    /* ********************************************************************************* */
    SoundFloat virtual GetWaveForm(SoundFloat SubTimeAbsolute) {// not used currently
      return Math::sin(SubTimeAbsolute * this->MyVoice->BaseFreq * Globals::TwoPi);
    }
    /* ********************************************************************************* */
    SoundFloat ClipTime(SoundFloat EndTime) {
      if (EndTime < Cursor_Point.TimeX) {
        EndTime = Cursor_Point.TimeX;// clip time
      }
      int FinalIndex = this->MyVoice->CPoints.size() - 1;
      VoicePoint *Final_Point = this->MyVoice->CPoints.at(FinalIndex);
      if (EndTime > Final_Point->TimeX) {
        this->IsFinished = true;
        EndTime = Final_Point->TimeX;// clip time
      }
      return EndTime;
    }
    /* ********************************************************************************* */
    OffsetBoxBase* Get_OffsetBox() {
      return this->MyOffsetBox;
    }
    /* ********************************************************************************* */
    void Render_Range(int dex0, int dex1, Wave& wave) {
      VoicePoint *pnt0, *pnt1;
      this->Render_Sample_Count = 0;
      for (int pcnt = dex0; pcnt < dex1; pcnt++) {
        pnt0 = this->MyVoice->CPoints.at(pcnt);
        pnt1 = this->MyVoice->CPoints.at(pcnt + 1);
        Render_Segment_Integral(*pnt0, *pnt1, wave);
      }
    }
    /* ********************************************************************************* */
    static void Interpolate_ControlPoint(VoicePoint& pnt0, VoicePoint& pnt1, SoundFloat RealTime, VoicePoint& PntMid) {
      SoundFloat FrequencyFactorStart = pnt0.GetFrequencyFactor();
      SoundFloat TimeRange = pnt1.TimeX - pnt0.TimeX;
      SoundFloat TimeAlong = RealTime - pnt0.TimeX;
      SoundFloat OctaveRange = pnt1.OctaveY - pnt0.OctaveY;
      SoundFloat OctaveRate = OctaveRange / TimeRange;// octaves per second
      SoundFloat SubTimeLocal;
      if (OctaveRate == 0.0){
        SubTimeLocal = TimeAlong;
      }else{
        SubTimeLocal = Voice::Integral(OctaveRate, TimeAlong);
      }
      PntMid.TimeX = RealTime;
      PntMid.SubTime = pnt0.SubTime + (FrequencyFactorStart * SubTimeLocal);

      // not calculus here
      PntMid.OctaveY = pnt0.OctaveY + (TimeAlong * OctaveRate);
      SoundFloat LoudRange = pnt1.LoudnessFactor - pnt0.LoudnessFactor;
      SoundFloat LoudAlong = TimeAlong * LoudRange / TimeRange;
      PntMid.LoudnessFactor = pnt0.LoudnessFactor + LoudAlong;
    }
    /* ********************************************************************************* */
    void Render_Segment_Iterative(VoicePoint& pnt0, VoicePoint& pnt1, Wave& wave) {// stateful iterative approach, not ready for test
      SoundFloat SRate = this->SampleRate;
      SoundFloat Time0 = this->InheritedMap.UnMapTime(pnt0.TimeX);
      SoundFloat Time1 = this->InheritedMap.UnMapTime(pnt1.TimeX);
      SoundFloat FrequencyFactorInherited = this->InheritedMap.GetFrequencyFactor();// inherit transposition
      int EndSample = (int) (Time1 * (SoundFloat)SRate);// absolute

      SoundFloat SubTime0 = pnt0.SubTime * this->InheritedMap.ScaleX;// tempo rescale
      SoundFloat TimeRange = pnt1.TimeX - pnt0.TimeX;
      SoundFloat FrequencyFactorStart = pnt0.GetFrequencyFactor();
      FrequencyFactorStart *= FrequencyFactorInherited;// inherit transposition
      SoundFloat Octave0 = this->InheritedMap.OctaveY + pnt0.OctaveY, Octave1 = this->InheritedMap.OctaveY + pnt1.OctaveY;

      SoundFloat OctaveRange = Octave1 - Octave0;
      SoundFloat OctaveRate = OctaveRange / TimeRange;// octaves per second bend
      OctaveRate += this->Inherited_OctaveRate;// inherit note bend
      SoundFloat LoudnessRange = pnt1.LoudnessFactor - pnt0.LoudnessFactor;
      SoundFloat LoudnessRate = LoudnessRange / TimeRange;
      int NumSamples = EndSample - this->Bone_Sample_Mark;

      SoundFloat TimeAlong, CurrentLoudness, Amplitude;

      SoundFloat CurrentOctaveLocal, CurrentFrequency, CurrentFrequencyFactorAbsolute, CurrentFrequencyFactorLocal;
      SoundFloat SubTimeIterate;

      if (true){
        ldouble Frequency0 = std::pow(2.0, Octave0);
        ldouble Frequency1 = std::pow(2.0, Octave1);
        ldouble FrequencyRatio = Frequency1/Frequency0;
        ldouble Root = std::pow(FrequencyRatio, 1.0/(ldouble)NumSamples);

        ldouble SubTimeLocal = pnt0.SubTime * this->InheritedMap.ScaleX;// tempo rescale
        SubTimeIterate = pnt0.SubTime * this->InheritedMap.ScaleX;// tempo rescale
        ldouble Snowball = 1.0;// frequency, pow(anything, 0)
        for (int SampleCnt = this->Bone_Sample_Mark; SampleCnt < EndSample; SampleCnt++){
          TimeAlong = (SampleCnt / SRate) - Time0;
          CurrentLoudness = pnt0.LoudnessFactor + (TimeAlong * LoudnessRate);
          CurrentFrequencyFactorAbsolute = (FrequencyFactorStart * Snowball);// legacy

          Amplitude = this->GetWaveForm(SubTimeIterate);
          wave.Set_Abs(SampleCnt, Amplitude * CurrentLoudness);
          SubTimeIterate += (CurrentFrequencyFactorAbsolute) / SRate;// legacy
          SubTimeLocal += CurrentFrequencyFactorAbsolute / SRate;
          Snowball *= Root;
          this->Render_Sample_Count++;
        }
      }else{// -----------------------------------
        SubTimeIterate = (pnt0.SubTime * BaseFreq * Globals::TwoPi);
        SoundFloat SampleDuration = 1.0 / SRate;
        for (int scnt = 0; scnt < NumSamples; scnt++) {
          TimeAlong = scnt * SampleDuration;
          CurrentOctaveLocal = TimeAlong * OctaveRate;
          CurrentFrequencyFactorLocal = std::pow(2.0, CurrentOctaveLocal); // to convert to absolute, do pnt0.SubTime + (FrequencyFactorStart * CurrentFrequencyFactorLocal);
          CurrentFrequencyFactorAbsolute = (FrequencyFactorStart * CurrentFrequencyFactorLocal);
          CurrentLoudness = pnt0.LoudnessFactor + (TimeAlong * LoudnessRate);
          CurrentFrequency = BaseFreq * CurrentFrequencyFactorAbsolute;// do we really need to include the base frequency in the summing?
          Amplitude = std::sin(SubTimeIterate);
          // Amplitude = this->GetWaveForm(SubTimeIterate);
          wave.Set(this->Render_Sample_Count, Amplitude * CurrentLoudness);
          SubTimeIterate += (CurrentFrequency * Globals::TwoPi) / SRate;
          this->Render_Sample_Count++;
        }
      }

      this->Bone_Sample_Mark = EndSample;
    }
    /* ********************************************************************************* */
    void Render_Segment_Integral(VoicePoint& pnt0, VoicePoint& pnt1, Wave& wave) {// stateless calculus integral approach
      if (Voice_Iterative){
        Render_Segment_Iterative(pnt0, pnt1, wave); return;
      }
      SoundFloat SRate = this->SampleRate;
      SoundFloat Time0 = this->InheritedMap.UnMapTime(pnt0.TimeX);
      SoundFloat Time1 = this->InheritedMap.UnMapTime(pnt1.TimeX);
      SoundFloat FrequencyFactorInherited = this->InheritedMap.GetFrequencyFactor();// inherit transposition
      int EndSample = (int) (Time1 * (SoundFloat)SRate);// absolute

      SoundFloat SubTime0 = pnt0.SubTime * this->InheritedMap.ScaleX;// tempo rescale
      SoundFloat TimeRange = Time1 - Time0;
      SoundFloat FrequencyFactorStart = pnt0.GetFrequencyFactor();
      SoundFloat Octave0 = this->InheritedMap.OctaveY + pnt0.OctaveY, Octave1 = this->InheritedMap.OctaveY + pnt1.OctaveY;

      SoundFloat OctaveRange = Octave1 - Octave0;
      SoundFloat OctaveRate = OctaveRange / TimeRange;// octaves per second bend
      OctaveRate += this->Inherited_OctaveRate;// inherit note bend
      SoundFloat LoudnessRange = pnt1.LoudnessFactor - pnt0.LoudnessFactor;
      SoundFloat LoudnessRate = LoudnessRange / TimeRange;
      SoundFloat SubTimeLocal, SubTimeAbsolute;

      SoundFloat TimeAlong;
      SoundFloat CurrentLoudness;
      SoundFloat Amplitude;
      int SampleCnt;
      SoundFloat PreCalc0 = (SubTime0 * FrequencyFactorInherited);// evaluate this outside the loop to optimize
      SoundFloat PreCalc1 = (FrequencyFactorStart * FrequencyFactorInherited);
      if (OctaveRate == 0.0) {// no bends, don't waste time on calculus
        for (SampleCnt = this->Bone_Sample_Mark; SampleCnt < EndSample; SampleCnt++) {
          TimeAlong = (SampleCnt / SRate) - Time0;
          CurrentLoudness = pnt0.LoudnessFactor + (TimeAlong * LoudnessRate);
          SubTimeLocal = TimeAlong;
          SubTimeAbsolute = (SubTime0 + (FrequencyFactorStart * SubTimeLocal)) * FrequencyFactorInherited;
          Amplitude = this->GetWaveForm(SubTimeAbsolute);
          //wave.Set(this->Render_Sample_Count, Amplitude * CurrentLoudness);
          wave.Set_Abs(SampleCnt, Amplitude * CurrentLoudness);
          this->Render_Sample_Count++;
        }
      }else{
        for (SampleCnt = this->Bone_Sample_Mark; SampleCnt < EndSample; SampleCnt++) { // look into -ffast-math
          TimeAlong = (SampleCnt / SRate) - Time0;
          CurrentLoudness = pnt0.LoudnessFactor + (TimeAlong * LoudnessRate);
          SubTimeLocal = Voice::Integral(OctaveRate, TimeAlong);
          SubTimeAbsolute = (SubTime0 + (FrequencyFactorStart * SubTimeLocal)) * FrequencyFactorInherited;
          //SubTimeAbsolute = (PreCalc0 + (PreCalc1 * SubTimeLocal));// optimized, hardly notice the difference
          Amplitude = this->GetWaveForm(SubTimeAbsolute);
          //wave.Set(this->Render_Sample_Count, Amplitude * CurrentLoudness);
          wave.Set_Abs(SampleCnt, Amplitude * CurrentLoudness);
          this->Render_Sample_Count++;
        }
      }
      this->Bone_Sample_Mark = EndSample;
    }
    /* ********************************************************************************* */
    boolean Create_Me() override {
      return true;
    }
    void Delete_Me() override {// IDeletable
      SingerBase::Delete_Me();
      this->Cursor_Point.Delete_Me();
      this->MyVoice = null;// wreck everything so we crash if we try to use a dead object
      this->Phase = this->Cycles = this->SubTime = Double::NEGATIVE_INFINITY;// Double_NEGATIVE_INFINITY;
      this->Current_Octave = this->Current_Frequency = Double::NEGATIVE_INFINITY;// Double_NEGATIVE_INFINITY;
      this->BaseFreq = Double::NEGATIVE_INFINITY;// Double_NEGATIVE_INFINITY;
      this->Prev_Point_Dex = this->Next_Point_Dex = Integer::MIN_VALUE;
      this->Render_Sample_Count = this->Bone_Sample_Mark = Integer::MIN_VALUE;
    }
  };
  /* ********************************************************************************* */
  class Voice_OffsetBox: public OffsetBoxBase {// location box to transpose in pitch, move in time, etc.
  public:
    Voice* VoiceContent;
    String ObjectTypeName = {"Voice_OffsetBox"};// for serialization
    /* ********************************************************************************* */
    Voice_OffsetBox() {
      OffsetBoxBase();
      this->Create_Me();
      this->Clear();
    }
    ~Voice_OffsetBox(){ this->Delete_Me(); }
    /* ********************************************************************************* */
    Voice* GetContent() override {
      return VoiceContent;
    }
    /* ********************************************************************************* */
    void Attach_Songlet(Voice *songlet) {// for serialization
      this->MySonglet = this->VoiceContent = songlet;
      songlet->Ref_Songlet();
    }
    /* ********************************************************************************* */
    Voice_Singer* Spawn_Singer() override {// for render time.  always always always override this
      Voice_Singer *Singer = this->VoiceContent->Spawn_Singer();
      Singer->MyOffsetBox = this;
      return Singer;
    }
    /* ********************************************************************************* */
    Voice_OffsetBox* Clone_Me() override {// always override this thusly
      Voice_OffsetBox *child = new Voice_OffsetBox();// clone
      child->Copy_From(*this);
      child->VoiceContent = this->VoiceContent;// iffy
      return child;
    }
    /* ********************************************************************************* */
    Voice_OffsetBox* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
      Voice_OffsetBox *child = this->Clone_Me();
      child->Attach_Songlet(this->VoiceContent->Deep_Clone_Me(HitTable));
      return child;
    }
    /* ********************************************************************************* */
    void BreakFromHerd(CollisionLibrary& HitTable) override {// for compose time. detach from my songlet and attach to an identical but unlinked songlet
      Voice *clone = this->VoiceContent->Deep_Clone_Me(HitTable);
      if (this->VoiceContent->UnRef_Songlet() <= 0) {
        delete this->VoiceContent;
        this->VoiceContent = null;
      }
      this->Attach_Songlet(clone);
    }
    /* ********************************************************************************* */
    boolean Create_Me() override {// IDeletable
      return true;
    }
    void Delete_Me() override {// IDeletable
      OffsetBoxBase::Delete_Me();
      if (this->VoiceContent != null) {
        if (this->VoiceContent->UnRef_Songlet() <= 0) {
          delete this->VoiceContent;
          this->VoiceContent = null;
        }
      }
    }
    /* ********************************************************************************* */
    JsonParse::Node* Export(CollisionLibrary& HitTable) {
      return nullptr;
    }
    void ShallowLoad(JsonParse::Node& phrase) {}
    void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
    ISonglet* Spawn_And_Attach_Songlet() {
      return nullptr;
    }
    /* ********************************************************************************* */
    class Factory{//: public IFactory {// for serialization
    public:
//      Voice_OffsetBox* Create(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {
//        return nullptr;
//      }
    };
  };
  /* ********************************************************************************* */
  Voice_OffsetBox* Spawn_OffsetBox() override {// for compose time
    Voice_OffsetBox *vbox = new Voice_OffsetBox();// Spawn an OffsetBox specific to this type of phrase.
    vbox->Attach_Songlet(this);
    return vbox;
  }
  /* ********************************************************************************* */
  Voice_Singer* Spawn_Singer() override {// for render time
    // Deliver one of my singers while exposing specific object class.
    // Handy if my parent's singers know what class I am and want special access to my particular type of singer.
    Voice_Singer *singer = new Voice_Singer();// Spawn a singer specific to this type of phrase.
    singer->MyVoice = this;
    singer->Set_Project(this->MyProject);// inherit project
    singer->BaseFreq = this->BaseFreq;
    return singer;
  }
};

#endif // Voice_hpp
