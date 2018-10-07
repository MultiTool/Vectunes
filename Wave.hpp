#ifndef Wave_hpp
#define Wave_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"

/**
 *
 * @author MultiTool
 */
class Wave{//: public IDeletable {
public:
  int NumSamples;
  int Current_Index;
  int StartDex = 0;
  int SampleRate;
  double StartTime = 0;
  double EndTime = 0;// for debugging
  //double* wave;
  std::vector<double> wave;// = {7, 5, 16, 8};
  boolean Debugging = false;
  double Debug_Fill = 4;
  /* ********************************************************************************* */
  Wave() {
    this->NumSamples = 0;
    //wave = new double[this->NumSamples];
    this->StartTime = 0.0;
    this->StartDex = 0;
    this->Current_Index = 0;
    this->Create_Me();
  }
  ~Wave(){this->Delete_Me();}
  /* ********************************************************************************* */
  void Rebase_Time(double TimeBase) {
    double TimeRange = this->EndTime - this->StartTime;
    this->StartTime = TimeBase;// wave start time is the offset of wave[0] from time 0.
    this->EndTime = this->StartTime + TimeRange;
    this->StartDex = (int) (this->StartTime * this->SampleRate);// StartDex is the number of empty samples from Time=0 to wave[0]
  }
  /* ********************************************************************************* */
  void Shift_Timebase(double TimeDif) {
    this->StartTime += TimeDif;// wave start time is the offset of wave[0] from time 0.
    this->EndTime += TimeDif;
    this->StartDex = (int) (this->StartTime * this->SampleRate);// StartDex is the number of empty samples from Time=0 to wave[0]
  }
  /* ********************************************************************************* */
  void Init(int SizeInit, int SampleRate0) {
    this->NumSamples = SizeInit;
    this->SampleRate = SampleRate0;
    //wave = new double[SizeInit];
    this->StartTime = 0.0;
    this->StartDex = 0;
    this->Current_Index = 0;
    this->StartTime = 0;// defaults
    this->EndTime = StartTime + (((double) this->NumSamples) / (double) this->SampleRate);
  }
  /* ********************************************************************************* */
  void Init(double StartTime0, double EndTime0, int SampleRate0) {
    this->StartTime = StartTime0;// wave start time is the offset of wave[0] from time 0.
    this->EndTime = EndTime0;
    this->SampleRate = SampleRate0;
    double TimeSpan = EndTime0 - StartTime0;
    int nsamps = (int) Math::ceil(TimeSpan * SampleRate0);
    this->StartDex = (int) (this->StartTime * SampleRate0);// StartDex is the number of empty samples from Time=0 to wave[0]
    this->NumSamples = nsamps;
    //wave = new double[nsamps + 1];// plus 1 because converting from double to int truncates.
    wave.resize(nsamps + 1);// plus 1 because converting from double to int truncates.
    this->Current_Index = 0;
  }
  /* ********************************************************************************* */
  void Ingest(std::vector<double>& Sample, int SampleRate0) {
    int len = Sample.size();//->length;
    double Duration = ((double) len) / (double) SampleRate0;
    this->Init(0, Duration, SampleRate0);
    for (int cnt = 0; cnt < len; cnt++) {
      this->wave[cnt] = Sample[cnt];
    }
  }
  /* ********************************************************************************* */
  void Overdub(Wave& other) {
    int MeStart, YouStart, MeStop, YouStop;
    if (other.StartDex > this->StartDex) {
      MeStart = other.StartDex - this->StartDex;
      YouStart = 0;
    } else {
      MeStart = 0;
      YouStart = this->StartDex - other.StartDex;
    }
    double TestMeStop, TestYouStop;
    TestMeStop = this->StartDex + this->NumSamples;
    TestYouStop = other.StartDex + other.NumSamples;
    if (TestMeStop < TestYouStop) {
      MeStop = this->NumSamples;
      YouStop = (this->StartDex + this->NumSamples) - other.StartDex;
    } else {
      MeStop = (other.StartDex + other.NumSamples) - this->StartDex;
      YouStop = other.NumSamples;
    }
    int ocnt = YouStart;
    for (int cnt = MeStart; cnt < MeStop; cnt++) {
      this->wave[cnt] += other.wave[ocnt++];
    }
  }
  /* ********************************************************************************* */
  void Amplify(double LoudnessFactor) {
    for (uint_fast64_t cnt = 0; cnt < this->wave.size(); cnt++) {
      this->wave[cnt] *= LoudnessFactor;
    }
  }
  /* ********************************************************************************* */
  void Fill(double Stuffing) {
    int len = this->wave.size();
    for (int cnt = 0; cnt < len; cnt++) {
      this->wave[cnt] = Stuffing;
    }
  }
  /* ********************************************************************************* */
  void Diff(Wave& other, Wave& result) {
    result.Init(this->NumSamples, this->SampleRate);
    for (int cnt = 0; cnt < this->NumSamples; cnt++) {
      result.wave[cnt] = this->wave[cnt] - other.wave[cnt];
    }
  }
  /* ********************************************************************************* */
  double GetMaxAmp() {
    int len = this->wave.size();
    double MaxAmp = 0.0;
    double AbsVal;
    for (int cnt = 0; cnt < len; cnt++) {
      if (MaxAmp < (AbsVal = Math::abs(this->wave[cnt]))) {
        MaxAmp = AbsVal;
      }
    }
    return MaxAmp;
  }
  /* ********************************************************************************* */
  void Normalize() {
    double MaxAmp = 0.0;
    double AbsVal;
    int len = this->wave.size();
    for (int cnt = 0; cnt < len; cnt++) {
      if (MaxAmp < (AbsVal = Math::abs(this->wave[cnt]))) {
        MaxAmp = AbsVal;
      }
    }
    this->Amplify(1.0 / MaxAmp);
  }
  /* ********************************************************************************* */
  void Center() {
    double Avg = 0.0, Sum = 0.0;
    int len = this->wave.size();
    for (int cnt = 0; cnt < len; cnt++) {
      Sum += this->wave[cnt];
    }
    Avg = Sum / (double) len;
    for (int cnt = 0; cnt < len; cnt++) {
      this->wave[cnt] -= Avg;
    }
  }
  /* ********************************************************************************* */
  void Append(Wave& other) {
    int StartPlace = other.StartDex;
    int nextsize = StartPlace + other.NumSamples;
    this->wave.resize(nextsize);// may be redundant
    //System.arraycopy(other.wave, 0, this->wave, StartPlace, other.NumSamples);
    this->wave.insert(std::end(this->wave), std::begin(other.wave), std::end(other.wave));
    this->NumSamples = nextsize;
  }
  /* ********************************************************************************* */
  double Get(int dex) {
    return this->wave[dex];
  }
  void Set(int dex, double value) {
    this->wave[dex] = value;
  }
  /* ******************************************************************* */
  double GetResample(double TimeSeconds) {
    TimeSeconds *= this->SampleRate;// convert to fractional sample index
    int Dex0 = (int) Math::floor(TimeSeconds);
    int Dex1 = Dex0 + 1;
    if (Dex1 >= this->wave.size()) {
      return 0.0;
    }
    double amp0 = this->wave[Dex0];
    double amp1 = this->wave[Dex1];
    double FullAmpDelta = amp1 - amp0;
    double Fraction = (TimeSeconds - Dex0);// always in the range of 0<=Fraction<1
    return amp0 + (Fraction * FullAmpDelta);
  }
  /* ******************************************************************* */
  double GetResampleLooped(double TimeSeconds) {
    double SampleDex = TimeSeconds * this->SampleRate;
    double SampleDexFlat = Math::floor(SampleDex);
    int Dex0 = ((int) SampleDexFlat) % this->NumSamples;
    int Dex1 = (Dex0 + 1) % this->NumSamples;
    double amp0 = this->wave[Dex0];
    double amp1 = this->wave[Dex1];
    double FullAmpDelta = amp1 - amp0;
    double Fraction = (SampleDex - SampleDexFlat);// always in the range of 0<=Fraction<1
    return amp0 + (Fraction * FullAmpDelta);
  }
  /* ******************************************************************* */
  void MorphToWave(Wave& other, double Factor, Wave& results) {
    double val0, val1;
    double InvFactor = 1.0 - Factor;
    int MinSamples = Math::min(this->NumSamples, Math::min(other.NumSamples, results.NumSamples));
    for (int cnt = 0; cnt < MinSamples; cnt++) {
      val0 = this->wave[cnt];
      val1 = other.wave[cnt];
      results.wave[cnt] = (val0 * InvFactor) + (val1 * Factor);
    }
  }
  /* ********************************************************************************* */
  void WhiteNoise_Fill() {
    double val;
    for (int SampCnt = 0; SampCnt < this->NumSamples; SampCnt++) {
      //val = Globals::RandomGenerator->NextDouble() * 2.0 - 1.0;// white noise
      val = Math::frand() * 2.0 - 1.0;// white noise
      this->wave[SampCnt] = val;
    }
    this->Center();
  }
  /* ********************************************************************************* */
  void Sawtooth_Fill() {
    double val;
    double FractAlong = 0;
    for (int SampCnt = 0; SampCnt < this->NumSamples; SampCnt++) {
      FractAlong = ((double) SampCnt) / (double) this->NumSamples;
      val = 1.0 - (FractAlong * 2.0);
      this->wave[SampCnt] = val;
    }
    this->Center();
  }
  /* ********************************************************************************* */
  double* GetWave() {
    //return this->wave;
    return &this->wave[0];
  }
  /* ********************************************************************************* */
  void SaveWaveToCsv(String& filename, Wave& wave) {
    double val;
//    DecimalFormat df = new DecimalFormat("#.######");
//
//    FileWriter writer = new FileWriter(filename, true);
//    for (int SampCnt = 0; SampCnt < wave.NumSamples; SampCnt++) {
//      val = wave.wave[SampCnt];
//      writer.write(df.format(val) + ", ");
//      //writer.write(String.format("%.06f", val) + ", ");
//      if ((SampCnt + 1) % 20 == 0) {
//        writer.write("\r\n");   // write new line
//      }
//    }
//    writer.close();
  }
  /* ********************************************************************************* */
  boolean Create_Me() {
    return 0;
  }
  void Delete_Me() { // wreck everything
    this->wave.resize(0);
    this->NumSamples = this->Current_Index = this->StartDex = this->SampleRate = Integer_MIN_VALUE;
    this->StartTime = this->EndTime = Double_NEGATIVE_INFINITY;
  }
  /* ********************************************************************************* */
  Wave* Clone_Me() {
    return nullptr;
  }
  void Copy_From(Wave& donor) {}
  String* Export() {
    return nullptr;
  }
  void Consume(String& text) {}
};


#endif
