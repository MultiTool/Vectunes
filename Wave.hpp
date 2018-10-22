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
  double StartTime = 0;
  double EndTime = 0;// for debugging
  std::vector<double> wave;// = {7, 5, 16, 8};
  boolean Debugging = false;
  double Debug_Fill = 4;
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
  void Init(int SizeInit, int SampleRate0) {// unused and untested
    this->NumSamples = SizeInit;
    this->SampleRate = SampleRate0;
    this->wave.resize(SizeInit);
    std::fill(this->wave.begin(), this->wave.end(), 0.0);
    this->StartTime = 0.0;
    this->StartDex = 0;
    this->StartTime = 0;// defaults
    this->EndTime = StartTime + (((double) this->NumSamples) / (double) this->SampleRate);
  }
  /* ********************************************************************************* */
  void Init(double StartTime0, double EndTime0, int SampleRate0) {
    this->StartTime = StartTime0;// wave start time is the offset of wave[0] from time 0.
    this->EndTime = EndTime0;
    this->SampleRate = SampleRate0;

    int SampleStart = StartTime0 * (double)SampleRate0;
    int SampleEnd = EndTime0 * (double)SampleRate0;
    int nsamps = SampleEnd - SampleStart;

    this->StartDex = (int) (this->StartTime * SampleRate0);// StartDex is the number of empty samples from Time=0 to wave[0]
    this->NumSamples = nsamps;
    this->wave.resize(nsamps);
    std::fill(this->wave.begin(), this->wave.end(), 0.0);
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
  void Center() {// Make sure integral of wave is 0.
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
  double Get(int dex) {
    return this->wave[dex];
  }
  void Set(int dex, double value) {
    if (wave.size()-1 < dex){
      printf("out of range! size:%lu, dex:%i\n\n",wave.size(), dex);
      //this->wave.insert(this->wave.begin()+dex, value); this->NumSamples = this->wave.size();
    }else{
      this->wave[dex] = value;
    }
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
      double amplitude = 32000;// whatever
      for (int n = 0; n < this->wave.size(); n++){
        double value     = this->wave.at(n);
        //write_word( f, (int)(amplitude * value), 2 );// stereo
        write_word( f, (int)(amplitude * value), 2 );
      }
    }else{
      // Write the audio samples
      // (We'll generate a single C4 note with a sine wave, fading from left to right)
      constexpr double two_pi = 6.283185307179586476925286766559;
      constexpr double max_amplitude = 32760;  // "volume"

      double hz        = 44100;    // samples per second
      double frequency = 261.626;  // middle C
      double seconds   = 2.5;      // time

      int N = hz * seconds;  // total number of samples
      for (int n = 0; n < N; n++){
        double amplitude = (double)n / N * max_amplitude;
        double value     = sin( (two_pi * n * frequency) / hz );
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
  void Copy_From(const Wave& donor) {}
  String* Export() {
    return nullptr;
  }
  void Consume(String& text) {}
};


#endif
