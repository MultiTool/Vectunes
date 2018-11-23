#include <iostream>
#include <time.h>
#include <chrono>
#include <ctime>

#include "ICloneable.hpp"
#include "ISonglet.hpp"
#include "CajaDelimitadora.hpp"

#include "IDrawable.hpp"
#include "MonkeyBox.hpp"
#include "OffsetBoxBase.hpp"
#include "Voice.hpp"

#include "Line2D.hpp"
#include "Splines.hpp"

#include "Grabber.hpp"

#include "GroupSong.hpp"
#include "LoopSong.hpp"
#include "PatternMaker.hpp"

using namespace std;

/* ********************************************************************************* */
// perfect example
class Outer {
public:
  int NumberOut;
  Outer(){
    this->NumberOut = 999;
  }
  class Inner{
  public:
    class Eener{
    public:
      Outer *afuera;
      void Test(){
        this->afuera->LastOne();
      }
    };
    int NumberIn = 12;
    Outer *oot;
    void Test(){
      this->NumberIn = this->oot->NumberOut;
      this->oot->LastOne();
    }
  };
  Inner* Spawn(){
    Inner *ins = new Inner();
    ins->oot = this;
    ins->Test();
    return ins;
  }
  void LastOne(){
    cout << "LastOne" << endl;
  }
};
/* ********************************************************************************* */
void TestVector(){// crashes
  std::vector<int> vint = {0, 1, 2};
  std::vector<int> vgo = {10, 11, 12};

  int StartPlace = 10;
  vint.insert(std::begin(vint) + StartPlace, std::begin(vgo), std::end(vgo));
  for (int cnt=0;cnt<vint.size();cnt++){
    printf("cnt:%i, ", vint.at(cnt));
  }
  printf("\n");
  // cnt:10, cnt:11, cnt:12, cnt:0, cnt:1, cnt:2,
}
/* ********************************************************************************* */
void MegaChop(SingerBase *singer, const String& FileName){// test random chops
  Wave chunk, whole;
  singer->Start();
  SoundFloat Time = 0;
  while (!singer->IsFinished){
    Time += 0.01;// * Math::frand();
    singer->Render_To(Time, chunk);
    whole.Append2(chunk);
  }
  whole.SaveToWav(FileName);
}
/* ********************************************************************************* */
void MegaChop_Add(SingerBase *singer, const String& FileName){// test random chops
  Wave Chunk, Chopped, Whole;
  Chunk.SampleRate = Globals::SampleRate;
  Chopped.SampleRate = Globals::SampleRate;
  Whole.SampleRate = Globals::SampleRate;
  singer->Start();
  SoundFloat Time = 0;
  bool TimeRender = true;
  while (!singer->IsFinished){
    Time += 0.01;
    //Time += 0.01 * Math::frand();
    //Time += 0.02;// * Math::frand();
    if (TimeRender){
      singer->Render_To(Time, Chunk);
    }else{
      singer->Render_To_Sample(int(Time*(double)Globals::SampleRate), Chunk);
    }

    Chopped.Append2(Chunk);
  }

  Chopped.SaveToWav(FileName + ".Chopped.wav");

  singer->Start();
  if (TimeRender){
    singer->Render_To(Time, Whole);
  }else{
    singer->Render_To_Sample(int(Time*(double)Globals::SampleRate), Whole);
  }

  Whole.SaveToWav(FileName + ".Whole.wav");

  Chopped.Amplify(-1.0);
  Whole.Overdub(Chopped);// If everything works, the 'whole' file waveform should be a flat line - no difference between whole and chopped.
  Whole.SaveToWav(FileName + ".Diff.wav");
}
/* ********************************************************************************* */
void FillVoice(Voice *voz, SoundFloat Duration){// add voice bend points
  VoicePoint *vp0 = new VoicePoint();
  vp0->OctaveY = 5.0; vp0->TimeX = 0;
  voz->Add_Note(vp0);

  VoicePoint *vp1 = new VoicePoint();
  vp1->OctaveY = 8.0; vp1->TimeX = Duration * 0.5;
  voz->Add_Note(vp1);

  VoicePoint *vp2 = new VoicePoint();
  vp2->OctaveY = 2.0; vp2->TimeX = Duration;
  voz->Add_Note(vp2);

  voz->Recalc_Line_SubTime();
}
/* ********************************************************************************* */
void FillVoice(Voice *voz){// add voice bend points
  FillVoice(voz, 0.2);
}
/* ********************************************************************************* */
GroupSong* MakeChord(ISonglet *songlet){
  OffsetBoxBase *handle;
  GroupSong *gsong = new GroupSong();

  handle = songlet->Spawn_OffsetBox();
  handle->OctaveY = 0.0;
  gsong->Add_SubSong(handle);

  handle = songlet->Spawn_OffsetBox();
  handle->OctaveY = 0.5;
  gsong->Add_SubSong(handle);

  handle = songlet->Spawn_OffsetBox();
  handle->OctaveY = 1.0;
  gsong->Add_SubSong(handle);

  return gsong;
}
/* ********************************************************************************* */
void TestSpeed(ISonglet& song, int NumTrials){
  using namespace std::chrono;
  SoundFloat Time, Duration;
  Wave Chunk, Chopped;

  Duration = song.Get_Duration();
  std::cout << "Song Duration:" << Duration << " seconds\n";

  OffsetBoxBase *obox = song.Spawn_OffsetBox();
  SingerBase *singer = obox->Spawn_Singer();

  system_clock::time_point start = system_clock::now();// https://en.cppreference.com/w/cpp/chrono
  for (int tcnt=0;tcnt<NumTrials;tcnt++){
    cout << "Trial Number:" << tcnt << "\n";
    Time = 0;
    bool TimeRender = true;
    singer->Start();
    while (!singer->IsFinished){
      Time += 0.1;
      singer->Render_To(Time, Chunk);
      Chopped.Append2(Chunk);
    }
  }
  system_clock::time_point end = system_clock::now();

  delete singer;
  delete obox;

  duration<double> elapsed_seconds = end-start;
  // elapsed_seconds /= (SoundFloat)NumTrials;
  SoundFloat TestTime = elapsed_seconds.count() / (SoundFloat)NumTrials;
  std::time_t end_time = system_clock::to_time_t(end);
  std::cout << "finished speed test at " << std::ctime(&end_time) << "render time: " << TestTime << "s\n";
  std::cout << "Ratio:" << (TestTime / Duration) << " seconds per second\n";
}
/* ********************************************************************************* */
void TestSpeed(ISonglet& song){
  TestSpeed(song, 16);
}
/* ********************************************************************************* */
void CreateRandom(){// create a random composition
}
/* ********************************************************************************* */
void CreateBentTriad(){
  Config conf;
  MetricsPacket metrics;
  metrics.MaxDuration = 0.0; metrics.MyProject = &conf; metrics.FreshnessTimeStamp = 1;

  double NoteDuration = 2.0, Octave = 5.0;
  Voice *voz;
  voz = PatternMaker::Create_Bent_Note(0.0, NoteDuration, 0.0, 1.0);

  LoopSong *loop = PatternMaker::Create_Unbound_Triad_Rhythm(*voz);

  GroupSong *grsong = new GroupSong();// put in a group to raise it by 5 octaves.
  grsong->Add_SubSong(*loop, 0.0, Octave, 1.0);
  grsong->Update_Guts(metrics); grsong->Set_Project(&conf);// finalize

  GroupSong::Group_OffsetBox *grobox = grsong->Spawn_OffsetBox();
  GroupSong::Group_Singer *gsing = grobox->Spawn_Singer();
  Wave wave;
  gsing->Start();
  gsing->Render_To(grsong->Duration, wave);
  delete gsing;
  wave.SaveToWav("BentTriad.wav");

  TestSpeed(*grsong);
  delete grobox;
}
/* ********************************************************************************* */
void ComparePowers(){// Test for iterative render, faster than integral but accumulates errors
  int Divisions = 12;
  Divisions = 44100;
  Divisions = 100;
  ldouble OctaveStart = 0.2;//0.0;//
  ldouble OctaveEnd = OctaveStart+2.0; OctaveEnd = OctaveStart+1.5;
  ldouble OctaveRange = OctaveEnd - OctaveStart;

  ldouble FrequencyStart = std::pow(2.0, OctaveStart);
  ldouble FrequencyEnd = std::pow(2.0, OctaveEnd);
  ldouble FrequencyRatio = FrequencyEnd/FrequencyStart;
  ldouble Root = std::pow(FrequencyRatio, 1.0/(ldouble)Divisions);

  ldouble Snowball = 1.0;// frequency, pow(anything, 0)
  for (int cnt=0;cnt<=Divisions;cnt++){
    ldouble FractAlong = ((ldouble)cnt)/(ldouble)Divisions;
    ldouble OctAlong = OctaveStart + (OctaveRange * FractAlong);
    ldouble Power = std::pow(2.0, OctAlong);
    ldouble Iterative = FrequencyStart*Snowball;
    ldouble Error = Power - Iterative;
    //printf("FractAlong:%f, Power:%f, Iterative:%f, Error:%e\n", FractAlong, Power, Iterative, Error);
    printf("FractAlong:%Lf, Power:%Lf, Iterative:%Lf, Error:%Le\n", FractAlong, Power, Iterative, Error);
    //cout << "FractAlong:"<< FractAlong <<", Power:"<< Power <<", Iterative:"<< Iterative <<", Error:"<< Error <<"\n";
    Snowball *= Root;
  }
  /*
  So adding octave means multiplying frequency.
  Freq always starts at 1.0, octave at 0.0?
  Then we apply an offset by multiplying FrequencyStart by our current frequency, and
  adding OctaveStart to the current octave.
  */
  printf("\n");
}
/* ********************************************************************************* */
int main() {
  Voice_Iterative = true;// temporary for testing
  if (false){
    ComparePowers();
    return 0;
  }
  Config conf;
  MetricsPacket metrics;
  metrics.MaxDuration = 0.0;
  metrics.MyProject = &conf;
  metrics.FreshnessTimeStamp = 2;

  std::srand(std::time(nullptr)); // use current time as seed for random generator

  if (false){
    LoopSong* tree = PatternMaker::MakeRandom();
    GroupSong *grsong = new GroupSong();
    GroupSong::Group_OffsetBox *treebox = tree->Spawn_OffsetBox();
    treebox->OctaveY = 3.0;
    grsong->Add_SubSong(treebox);

    grsong->Update_Guts(metrics); grsong->Set_Project(&conf);// finalize

    GroupSong::Group_OffsetBox *grobox = grsong->Spawn_OffsetBox();
    GroupSong::Group_Singer *gsing = grobox->Spawn_Singer();
    Wave wave;
    gsing->Start();
    printf("Render_To:\n");
    gsing->Render_To(tree->Duration, wave);
    printf("Render_To done.\n");
    delete gsing;
    printf("SaveToWav:\n");
    wave.SaveToWav("RandomTree.wav");
    printf("SaveToWav done.\n");
    TestSpeed(*grsong, 1);
    delete grobox;

    return 0;
  }

  if (false){
    CreateBentTriad();
    //return 0;
  }

  if (false){// create bent minor triad
    Voice *voz;
    if (true){
      voz = PatternMaker::Create_Bent_Note(0.0, 1.0, 0.0, 1.0);
    }else{
      voz = new Voice();
      FillVoice(voz, 1.0);// add voice bend points
    }

    GroupSong *triad = PatternMaker::MakeMinor(*voz, 12 * 5);
    triad->Update_Guts(metrics);
    triad->Set_Project(&conf);

    Wave wave;
    GroupSong::Group_OffsetBox *triadbox = triad->Spawn_OffsetBox();
    GroupSong::Group_Singer *gsing = triadbox->Spawn_Singer();
    gsing->Start();
    gsing->Render_To(triad->Duration, wave);
    delete gsing;

    wave.SaveToWav("Minor.wav");

    delete triadbox;
  }

  if (true) {// Voice and  Group
    Voice *voz;
    voz = new Voice();
    voz->Update_Guts(metrics);
    voz->Set_Project(&conf);

    //FillVoice(voz, 2000.0);// add voice bend points, 33 min 20 sec
    FillVoice(voz, 1.0);// add voice bend points, 1 min
    //FillVoice(voz, 0.2);// add voice bend points

    Voice::Voice_OffsetBox *vobox = voz->Spawn_OffsetBox();
    vobox->TimeX = 0.27;//0.11;// 0.0;
    if (true){
      Voice::Voice_Singer *vsing = vobox->Spawn_Singer();

      //Voice::Voice_Singer *vsing = voz->Spawn_Singer();
      cout << "Current_Frequency:" << vsing->Current_Frequency << endl;
      vsing->Start();
      MegaChop_Add(vsing, "Voice");
      delete vsing;
    }
    GroupSong *gsong = new GroupSong();
    gsong->Add_SubSong(vobox);

    metrics.Reset();
    gsong->Update_Guts(metrics);
    gsong->Set_Project(&conf);

    GroupSong::Group_OffsetBox *grobox = gsong->Spawn_OffsetBox();
    //grobox->Set_Project(&conf);
    //ISonglet::Unref(gb);
    //delete gb;// automatically deleted by grobox

    GroupSong::Group_Singer *gsing = grobox->Spawn_Singer();
    MegaChop_Add(gsing, "Group");
    delete gsing;
    delete grobox;
    //delete vobox;// the group already deleted everything.

    // delete voz;// voice is deleted automatically when we delete vobox
  }

  if (false) {// Span - simple group with one delayed voice
    Voice *voz0;
    voz0 = new Voice();
    FillVoice(voz0);

    Voice::Voice_OffsetBox *vobox0 = voz0->Spawn_OffsetBox();
    vobox0->TimeX = 0.27;//0.11;

    GroupSong *gsong = new GroupSong();
    gsong->Add_SubSong(vobox0);

    gsong->Set_Project(&conf);
    gsong->Update_Guts(metrics);

    GroupSong::Group_OffsetBox *gobox = gsong->Spawn_OffsetBox();

    SingerBase *singer = gobox->Spawn_Singer();
    MegaChop_Add(singer, "Span");

    delete gsong;
  }

  if (false) {// Loop
    Voice *voz0;
    voz0 = new Voice();
    FillVoice(voz0);
    Voice::Voice_OffsetBox *vobox0 = voz0->Spawn_OffsetBox();

    GroupSong *chord = MakeChord(voz0);
    GroupSong::Group_OffsetBox *ChordHandle = chord->Spawn_OffsetBox();

    LoopSong *lsong = new LoopSong();

    //lsong->Add_SubSong(vobox0);
    lsong->Add_SubSong(ChordHandle);

    lsong->Set_Beats(100);
    //lsong->Set_Interval(0.10);// exposes loop/group chopping bug
    lsong->Set_Interval(0.25);
    GroupSong::Group_OffsetBox *lobox = lsong->Spawn_OffsetBox();

    lsong->Set_Project(&conf);
    lsong->Update_Guts(metrics);

    SingerBase *singer = lobox->Spawn_Singer();

    MegaChop_Add(singer, "Loop");
    delete singer;

    TestSpeed(*lsong);
    //finished loop computation at Thu Nov 08 09:30:09 2018
    //elapsed time: 4.09459s

    delete lsong;
  }
  {
    Outer *ouch = new Outer();
    Outer::Inner *een = ouch->Spawn();
    cout << "een->NumberIn:" << een->NumberIn << endl;
    delete een;
    delete ouch;
  }

  {
    GroupSong *gb1 = new GroupSong();
    GroupSong::Group_OffsetBox *grobox1 = gb1->Spawn_OffsetBox();
    delete grobox1;
    //ISonglet::Unref(gb);
    //delete gb;// automatically deleted by grobox
  }

  OffsetBoxBase *obox = new OffsetBoxBase();

  OffsetBoxBase *oboxkid = obox->Clone_Me();

  Wave *wav = new Wave();
  cout << "Hello world!" << endl;
  delete wav;

  delete obox;
  delete oboxkid;

  return 0;
}
