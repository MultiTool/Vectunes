#ifndef Wave_hpp
#define Wave_hpp

#include <iostream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>  // std::setprecision
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"

//using namespace std;

/**
 *
 * @author MultiTool
 */
class Wave{//: public IDeletable {
public:
  int NumSamples;
  int StartDex = 0;// startdex is offset of 'virtual samples' before our wave array begins
  int SampleRate;
  SoundFloat StartTime = 0;
  SoundFloat EndTime = 0;// for debugging
  std::vector<SoundFloat> wave;// = {7, 5, 16, 8};
  boolean Debugging = false;
  SoundFloat Debug_Fill = 4;
  /* ********************************************************************************* */
  Wave() {
    this->NumSamples = 0;
    this->wave.resize(this->NumSamples);
    this->StartTime = 0.0;
    this->StartDex = 0;
    this->Create_Me();
  }
  ~Wave(){this->Delete_Me();}
  /* ********************************************************************************* */
  void Rebase_Time(SoundFloat TimeBase) {
    SoundFloat TimeRange = this->EndTime - this->StartTime;
    this->StartTime = TimeBase;// wave start time is the offset of wave[0] from time 0.
    this->EndTime = this->StartTime + TimeRange;
    this->StartDex = (int) (this->StartTime * this->SampleRate);// StartDex is the number of empty samples from Time=0 to wave[0]
  }
  /* ********************************************************************************* */
  void Shift_Timebase(SoundFloat TimeDif) {
    this->StartTime += TimeDif;// wave start time is the offset of wave[0] from time 0.
    this->EndTime += TimeDif;
    this->StartDex = (int) (this->StartTime * this->SampleRate);// StartDex is the number of empty samples from Time=0 to wave[0]
  }
  /* ********************************************************************************* */
  void Init(int SizeInit, int SampleRate0, SoundFloat Filler) {// unused and untested
    this->NumSamples = SizeInit;
    this->SampleRate = SampleRate0;
    this->wave.resize(SizeInit);
    std::fill(this->wave.begin(), this->wave.end(), Filler);
    this->StartTime = 0.0;
    this->StartDex = 0;
    this->StartTime = 0;// defaults
    this->EndTime = StartTime + (((SoundFloat) this->NumSamples) / (SoundFloat) this->SampleRate);
  }
  /* ********************************************************************************* */
  void Init(SoundFloat StartTime0, SoundFloat EndTime0, int SampleRate0, SoundFloat Filler) {
    this->StartTime = StartTime0;// wave start time is the offset of wave[0] from time 0.
    this->EndTime = EndTime0;
    this->SampleRate = SampleRate0;

    int SampleStart = (StartTime0 * (SoundFloat)SampleRate0);
    int SampleEnd = (EndTime0 * (SoundFloat)SampleRate0);
    int nsamps = SampleEnd - SampleStart;

    this->StartDex = (int) (this->StartTime * SampleRate0);// StartDex is the number of empty samples from Time=0 to wave[0]
    this->NumSamples = nsamps;
    this->wave.resize(nsamps);
    std::fill(this->wave.begin(), this->wave.end(), Filler);
  }
  /* ********************************************************************************* */
  void Init_Sample(int SampleStart, int SampleEnd, int SampleRate0, SoundFloat Filler) {
    this->StartTime = ((SoundFloat)SampleStart)/(SoundFloat)SampleRate0;// wave start time is the offset of wave[0] from time 0.
    this->EndTime = ((SoundFloat)SampleEnd)/(SoundFloat)SampleRate0;
    this->SampleRate = SampleRate0;
    int nsamps = SampleEnd - SampleStart;

    this->StartDex = SampleStart;// StartDex is the number of empty samples from Time=0 to wave[0]
    this->NumSamples = nsamps;
    this->wave.resize(nsamps);
    std::fill(this->wave.begin(), this->wave.end(), Filler);
  }
  /* ********************************************************************************* */
  void Ingest(std::vector<SoundFloat>& Sample, int SampleRate0) {
    int len = Sample.size();//->length;
    SoundFloat Duration = ((SoundFloat) len) / (SoundFloat) SampleRate0;
    this->Init(0, Duration, SampleRate0, 0.0);
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
    int AbsoluteMeStop, AbsoluteYouStop;
    AbsoluteMeStop = this->StartDex + this->NumSamples;
    AbsoluteYouStop = other.StartDex + other.NumSamples;
    if (AbsoluteMeStop < AbsoluteYouStop) {
      MeStop = this->NumSamples;
      YouStop = AbsoluteMeStop - other.StartDex;
    } else {
      MeStop = AbsoluteYouStop - this->StartDex;
      YouStop = other.NumSamples;
    }
    int ocnt = YouStart;
    for (int cnt = MeStart; cnt < MeStop; cnt++) {
      this->wave[cnt] += other.wave[ocnt++];
    }
  }
  /* ********************************************************************************* */
  void Amplify(SoundFloat LoudnessFactor) {
    for (uint_fast64_t cnt = 0; cnt < this->wave.size(); cnt++) {
      this->wave[cnt] *= LoudnessFactor;
    }
  }
  /* ********************************************************************************* */
  void Fill(SoundFloat Stuffing) {
    int len = this->wave.size();
    for (int cnt = 0; cnt < len; cnt++) {
      this->wave[cnt] = Stuffing;
    }
  }
  /* ********************************************************************************* */
  void Diff(Wave& other, Wave& result) {
    result.Init(this->NumSamples, this->SampleRate, 0.0);
    for (int cnt = 0; cnt < this->NumSamples; cnt++) {
      result.wave[cnt] = this->wave[cnt] - other.wave[cnt];
    }
  }
  /* ********************************************************************************* */
  SoundFloat GetMaxAmp() {
    int len = this->wave.size();
    SoundFloat MaxAmp = 0.0;
    SoundFloat AbsVal;
    for (int cnt = 0; cnt < len; cnt++) {
      if (MaxAmp < (AbsVal = Math::abs(this->wave[cnt]))) {
        MaxAmp = AbsVal;
      }
    }
    return MaxAmp;
  }
  /* ********************************************************************************* */
  void Normalize() {
    SoundFloat MaxAmp = 0.005;//Globals::Fudge;// avoid divide by zero
    SoundFloat AbsVal;
    int len = this->wave.size();
    for (int cnt = 0; cnt < len; cnt++) {
      if (MaxAmp < (AbsVal = Math::abs(this->wave[cnt]))) {
        MaxAmp = AbsVal;
      }
    }
    this->Amplify(1.0 / MaxAmp);
  }
  /* ********************************************************************************* */
  void Center() {// Make sure integral of wave is 0.
    SoundFloat Avg = 0.0, Sum = 0.0;
    int len = this->wave.size();
    for (int cnt = 0; cnt < len; cnt++) {
      Sum += this->wave[cnt];
    }
    Avg = Sum / (SoundFloat) len;
    for (int cnt = 0; cnt < len; cnt++) {
      this->wave[cnt] -= Avg;
    }
  }
  /* ********************************************************************************* */
  void Append(Wave& other) {// to do: fix this and rename it Splice()
    int StartPlace = other.StartDex;
    int nextsize = StartPlace + other.NumSamples;
    //System.arraycopy(other.wave, 0, this->wave, StartPlace, other.NumSamples);
    this->wave.insert(std::begin(this->wave) + StartPlace, std::begin(other.wave), std::end(other.wave));
    //this->wave.insert(std::end(this->wave), std::begin(other.wave), std::end(other.wave));
    //this->wave.insert(this->wave.end(), other.wave.begin(), other.wave.end());
    this->NumSamples = nextsize;
  }
  /* ********************************************************************************* */
  void Append2(Wave& other) {// simple append for testing, ignores time offsets
    int MySize = this->wave.size();
    int OtherSize = other.wave.size();
    int nextsize = this->NumSamples + other.NumSamples;
    //this->wave.resize(nextsize);// may be redundant
    //this->wave.insert(std::end(this->wave), std::begin(other.wave), std::end(other.wave));
    this->wave.insert(this->wave.end(), other.wave.begin(), other.wave.end());
    //this->wave.insert(this->wave + this->NumSamples, other.wave.begin(), other.wave.end());
    this->NumSamples = this->wave.size();// nextsize;
  }
  /* ********************************************************************************* */
  SoundFloat Get(int dex) {
    return this->wave[dex];
  }
  void Set(int dex, SoundFloat value) {
    int sz = wave.size();// to do: replace this approach with something more efficient
    if (sz-1 < dex){
      printf("out of range! size:%lu, dex:%i\n\n",wave.size(), dex);
      //this->wave.insert(this->wave.begin()+dex, value); this->NumSamples = this->wave.size();
    }else{
      this->wave[dex] = value;
    }
  }
  void Set_Abs(int dex, SoundFloat value) {// set with index based on absolute time == 0 (beginning of whole composition)
    int sz = wave.size();
    dex -= this->StartDex;// to do: replace this approach with something more efficient
    if (dex < 0 || sz-1 < dex){
      printf("Set_Abs out of range! size:%lu, dex:%i\n\n",wave.size(), dex);
      //this->wave.insert(this->wave.begin()+dex, value); this->NumSamples = this->wave.size();
    }else{
      this->wave[dex] = value;
    }
  }
  /* ********************************************************************************* */
  int GetFinalDex() {
    return this->StartDex + this->wave.size();
  }
  /* ******************************************************************* */
  SoundFloat GetResample(SoundFloat TimeSeconds) {
    TimeSeconds *= this->SampleRate;// convert to fractional sample index
    int Dex0 = (int) Math::floor(TimeSeconds);
    int Dex1 = Dex0 + 1;
    if (Dex1 >= this->wave.size()) {
      return 0.0;
    }
    SoundFloat amp0 = this->wave[Dex0];
    SoundFloat amp1 = this->wave[Dex1];
    SoundFloat FullAmpDelta = amp1 - amp0;
    SoundFloat Fraction = (TimeSeconds - Dex0);// always in the range of 0<=Fraction<1
    return amp0 + (Fraction * FullAmpDelta);
  }
  /* ******************************************************************* */
  SoundFloat GetResampleLooped(SoundFloat TimeSeconds) {
    SoundFloat SampleDex = TimeSeconds * this->SampleRate;
    SoundFloat SampleDexFlat = Math::floor(SampleDex);
    int Dex0 = ((int) SampleDexFlat) % this->NumSamples;
    int Dex1 = (Dex0 + 1) % this->NumSamples;
    SoundFloat amp0 = this->wave[Dex0];
    SoundFloat amp1 = this->wave[Dex1];
    SoundFloat FullAmpDelta = amp1 - amp0;
    SoundFloat Fraction = (SampleDex - SampleDexFlat);// always in the range of 0<=Fraction<1
    return amp0 + (Fraction * FullAmpDelta);
  }
  /* ******************************************************************* */
  void MorphToWave(Wave& other, SoundFloat Factor, Wave& results) {
    SoundFloat val0, val1;
    SoundFloat InvFactor = 1.0 - Factor;
    int MinSamples = Math::min(this->NumSamples, Math::min(other.NumSamples, results.NumSamples));
    for (int cnt = 0; cnt < MinSamples; cnt++) {
      val0 = this->wave[cnt];
      val1 = other.wave[cnt];
      results.wave[cnt] = (val0 * InvFactor) + (val1 * Factor);
    }
  }
  /* ********************************************************************************* */
  void WhiteNoise_Fill() {
    SoundFloat val;
    for (int SampCnt = 0; SampCnt < this->NumSamples; SampCnt++) {
      val = Math::frand() * 2.0 - 1.0;// white noise
      this->wave[SampCnt] = val;
    }
    this->Center();
  }
  /* ********************************************************************************* */
  void Sawtooth_Fill() {
    SoundFloat val;
    SoundFloat FractAlong = 0;
    for (int SampCnt = 0; SampCnt < this->NumSamples; SampCnt++) {
      FractAlong = ((SoundFloat) SampCnt) / (SoundFloat) this->NumSamples;
      val = 1.0 - (FractAlong * 2.0);
      this->wave[SampCnt] = val;
    }
    this->Center();
  }
  /* ********************************************************************************* */
  SoundFloat* GetWave() {
    //return this->wave;
    return &this->wave[0];
  }
  /* ********************************************************************************* */
  void SaveWaveToCsv(String& filename, Wave& wave) {
    SoundFloat val;
    std::ofstream outfile;
    outfile.open(filename);
    outfile << std::fixed << std::setprecision(6);// DecimalFormat("#.######");
    for (int SampCnt = 0; SampCnt < wave.NumSamples; SampCnt++) {
      val = wave.wave[SampCnt];
      outfile << val << ", ";
      if ((SampCnt + 1) % 20 == 0) {
        outfile << std::endl;  // write newline
      }
    }
    outfile.close();
  }
  /* ********************************************************************************* */
  template <typename Word> // from http://www.cplusplus.com/forum/beginner/166954/
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
  int SaveToWav(const String& FileName){// from http://www.cplusplus.com/forum/beginner/166954/
    Wave clone;
    clone.Copy_From(*this);
    clone.Normalize();

    std::ofstream f(FileName, std::ios::binary );

    // Write the file headers
    f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
    write_word( f,     16, 4 );  // no extension data
    write_word( f,      1, 2 );  // PCM - integer samples
    if (false){
      write_word( f,      2, 2 );  // two channels (stereo file)
    }else{
      write_word( f,      1, 2 );  // one channel (mono file)
    }
    write_word( f,  44100, 4 );  // samples per second (Hz)
    write_word( f, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
    write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
    write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

    // Write the data chunk header
    size_t data_chunk_pos = f.tellp();
    f << "data----";  // (chunk size to be filled in later)

    if (true){
      SoundFloat amplitude = 32000;// whatever
      for (int n = 0; n < clone.wave.size(); n++){
        SoundFloat value     = clone.wave.at(n);
        //write_word( f, (int)(amplitude * value), 2 );// stereo
        write_word( f, (int)(amplitude * value), 2 );
      }
    }else{
      // Write the audio samples
      // (We'll generate a single C4 note with a sine wave, fading from left to right)
      constexpr SoundFloat two_pi = 6.283185307179586476925286766559;
      constexpr SoundFloat max_amplitude = 32760;  // "volume"

      SoundFloat hz        = 44100;    // samples per second
      SoundFloat frequency = 261.626;  // middle C
      SoundFloat seconds   = 2.5;      // time

      int N = hz * seconds;  // total number of samples
      for (int n = 0; n < N; n++){
        SoundFloat amplitude = (SoundFloat)n / N * max_amplitude;
        SoundFloat value     = sin( (two_pi * n * frequency) / hz );
        write_word( f, (int)(                 amplitude  * value), 2 );
        write_word( f, (int)((max_amplitude - amplitude) * value), 2 );
      }
    }
    // (We'll need the final file size to fix the chunk sizes above)
    size_t file_length = f.tellp();

    // Fix the data chunk header to contain the data size
    f.seekp( data_chunk_pos + 4 );
    write_word( f, file_length - data_chunk_pos + 8 );

    // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
    f.seekp( 0 + 4 );
    write_word( f, file_length - 8, 4 );
  }
  /* ********************************************************************************* */
  boolean Create_Me() {
    return 0;
  }
  void Delete_Me() { // wreck everything
    this->wave.resize(0);
    this->NumSamples = this->StartDex = this->SampleRate = Integer_MIN_VALUE;
    this->StartTime = this->EndTime = Double_NEGATIVE_INFINITY;
  }
  /* ********************************************************************************* */
  Wave* Clone_Me() {
    return nullptr;
  }
  void Copy_From(const Wave& donor) {
    this->NumSamples = donor.NumSamples;
    this->StartDex = donor.StartDex;
    this->SampleRate = donor.SampleRate;
    this->StartTime = donor.StartTime;
    this->EndTime = donor.EndTime;
    this->Debugging = donor.Debugging;
    this->Debug_Fill = donor.Debug_Fill;
    this->wave = donor.wave;
  }
  String* Export() {
    return nullptr;
  }
  void Consume(String& text) {}
};


#endif
