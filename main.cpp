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

#include "GroupBox.hpp"

using namespace std;

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

void MegaChop_Add(SingerBase *singer, const String& FileNameChopped, const String& FileNameWhole){// test random chops
  Wave chunk, Glued, Whole;
  singer->Start();
  double Time = 0;
  while (!singer->IsFinished){
    Time += 0.01;
    //Time += 0.01 * Math::frand();
    //Time += 0.02;// * Math::frand();
    singer->Render_To(Time, chunk);
    Glued.Append2(chunk);
  }
  Glued.SaveToWav(FileNameChopped);
  singer->Start();
  singer->Render_To(Time, Whole);
  Glued.Amplify(-1.0);
  Whole.Overdub(Glued);// If everything works, the 'whole' file waveform should be a flat line - no difference between whole and chopped.
  Whole.SaveToWav(FileNameWhole);
}

int main() {
  {
    Outer *ouch = new Outer();
    Outer::Inner *een = ouch->Spawn();
    cout << "een->NumberIn:" << een->NumberIn << endl;
    delete een;
    delete ouch;
  }

  cout << Math::PI << endl;
  {
    Config conf;
    MetricsPacket metrics;
    metrics.MaxDuration = 0.0;
    metrics.MyProject = &conf;
    metrics.FreshnessTimeStamp = 1;

    Voice *voz;
    voz = new Voice();
    voz->Update_Guts(metrics);

    {// add voice bend points
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
    Voice::Voice_OffsetBox *vobox = voz->Spawn_OffsetBox();
    Voice::Voice_Singer *vsing = vobox->Spawn_Singer();

    //Voice::Voice_Singer *vsing = voz->Spawn_Singer();
    cout << "Current_Frequency:" << vsing->Current_Frequency << endl;
    vsing->Start();
    MegaChop_Add(vsing, "ChoppedVoice.wav", "WholeVoice.wav");
    delete vsing;

    GroupBox *gb = new GroupBox();
    // gb->Set_Project(&conf);
    gb->Add_SubSong(vobox);

    metrics.Reset();
    gb->Update_Guts(metrics);

    GroupBox::Group_OffsetBox *grobox = gb->Spawn_OffsetBox();
    //grobox->Set_Project(&conf);
    //ISonglet::Unref(gb);
    //delete gb;// automatically deleted by grobox

    GroupBox::Group_Singer *gsing = grobox->Spawn_Singer();
    MegaChop_Add(gsing, "ChoppedGroup.wav", "WholeGroup.wav");
    delete gsing;
    delete grobox;
    delete vobox;

    // delete voz;// voice is deleted automatically when we delete vobox
  }
  {
    GroupBox *gb1 = new GroupBox();
    GroupBox::Group_OffsetBox *grobox1 = gb1->Spawn_OffsetBox();
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
