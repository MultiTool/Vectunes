#include <iostream>

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
  double Time = 0;
  while (!singer->IsFinished){
    Time += 0.01;// * Math::frand();
    singer->Render_To(Time, chunk);
    whole.Append2(chunk);
  }
  whole.SaveToWav(FileName);
}
/* ********************************************************************************* */
void MegaChop_Add(SingerBase *singer, const String& FileName){// test random chops
  Wave chunk, Glued, Whole;
  chunk.SampleRate = Globals::SampleRate;
  Glued.SampleRate = Globals::SampleRate;
  Whole.SampleRate = Globals::SampleRate;
  singer->Start();
  double Time = 0;
  while (!singer->IsFinished){
    Time += 0.01;
    //Time += 0.01 * Math::frand();
    //Time += 0.02;// * Math::frand();
    singer->Render_To(Time, chunk);
    Glued.Append2(chunk);
  }

  Glued.SaveToWav(FileName + ".Chopped.wav");

  singer->Start();
  singer->Render_To(Time, Whole);
  Whole.SaveToWav(FileName + ".Whole.wav");

  Glued.Amplify(-1.0);
  Whole.Overdub(Glued);// If everything works, the 'whole' file waveform should be a flat line - no difference between whole and chopped.
  Whole.SaveToWav(FileName + ".Diff.wav");
}
/* ********************************************************************************* */
void FillVoice(Voice *voz){// add voice bend points
  VoicePoint *vp0 = new VoicePoint();
  vp0->OctaveY = 5.0; vp0->TimeX = 0;
  voz->Add_Note(vp0);

  VoicePoint *vp1 = new VoicePoint();
  vp1->OctaveY = 8.0; vp1->TimeX = 0.1;
  voz->Add_Note(vp1);

  VoicePoint *vp2 = new VoicePoint();
  vp2->OctaveY = 2.0; vp2->TimeX = 0.2;
  voz->Add_Note(vp2);

  voz->Recalc_Line_SubTime();
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
int main() {
  Config conf;
  MetricsPacket metrics;
  metrics.MaxDuration = 0.0;
  metrics.MyProject = &conf;
  metrics.FreshnessTimeStamp = 1;

  {
    Voice *voz0;
    voz0 = new Voice();
    FillVoice(voz0);

    Voice::Voice_OffsetBox *vobox0 = voz0->Spawn_OffsetBox();
    vobox0->TimeX = 0.11;

    GroupSong *gsong = new GroupSong();
    gsong->Add_SubSong(vobox0);

    gsong->Set_Project(&conf);
    gsong->Update_Guts(metrics);

    GroupSong::Group_OffsetBox *gobox = gsong->Spawn_OffsetBox();

    SingerBase *singer = gobox->Spawn_Singer();
    MegaChop_Add(singer, "Span");

    delete gsong;
    //return 0;
  }

  if (false) {
    Voice *voz0;
    voz0 = new Voice();
    FillVoice(voz0);
    //voz0->Update_Guts(metrics);
    //voz0->Set_Project(&conf);
    Voice::Voice_OffsetBox *vobox0 = voz0->Spawn_OffsetBox();

    GroupSong *chord = MakeChord(voz0);
    GroupSong::Group_OffsetBox *ChordHandle = chord->Spawn_OffsetBox();

    LoopSong *lsong = new LoopSong();
    lsong->Add_SubSong(vobox0);
    //lsong->Add_SubSong(ChordHandle);
    lsong->Set_Beats(2);
    lsong->Set_Interval(0.10);// exposes loop/group chopping bug
    //lsong->Set_Interval(0.25);
    GroupSong::Group_OffsetBox *lobox = lsong->Spawn_OffsetBox();

    lsong->Set_Project(&conf);
    lsong->Update_Guts(metrics);

    SingerBase *singer = lobox->Spawn_Singer();
    MegaChop_Add(singer, "Loop");

    delete lsong;
    //return 0;
  }
  {
    Outer *ouch = new Outer();
    Outer::Inner *een = ouch->Spawn();
    cout << "een->NumberIn:" << een->NumberIn << endl;
    delete een;
    delete ouch;
  }

  cout << Math::PI << endl;
  {
    Voice *voz;
    voz = new Voice();
    voz->Update_Guts(metrics);
    voz->Set_Project(&conf);

    FillVoice(voz);// add voice bend points

    Voice::Voice_OffsetBox *vobox = voz->Spawn_OffsetBox();
    Voice::Voice_Singer *vsing = vobox->Spawn_Singer();

    //Voice::Voice_Singer *vsing = voz->Spawn_Singer();
    cout << "Current_Frequency:" << vsing->Current_Frequency << endl;
    vsing->Start();
    MegaChop_Add(vsing, "Voice");
    delete vsing;

    GroupSong *gb = new GroupSong();
    // gb->Set_Project(&conf);
    gb->Add_SubSong(vobox);

    metrics.Reset();
    gb->Update_Guts(metrics);
    gb->Set_Project(&conf);

    GroupSong::Group_OffsetBox *grobox = gb->Spawn_OffsetBox();
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
