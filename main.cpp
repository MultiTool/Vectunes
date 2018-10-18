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
  Whole.Overdub(Glued);
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
    Voice *voz;
    voz = new Voice();
    Config conf;
    voz->Set_Project(&conf);

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

    Voice::Voice_OffsetBox *vobox = voz->Spawn_OffsetBox();
    Voice::Voice_Singer *vsing = vobox->Spawn_Singer();

    //Voice::Voice_Singer *vsing = voz->Spawn_Singer();
    cout << "Current_Frequency:" << vsing->Current_Frequency << endl;
    vsing->Start();

    Wave wave0, wave1, wave2;

    if (false){
      vsing->Render_To(0.05, wave0);
      vsing->Render_To(0.15, wave1);
      vsing->Render_To(0.20, wave2);

      wave0.SaveToWav("example0.wav");
      wave1.SaveToWav("example1.wav");
      wave2.SaveToWav("example2.wav");
    }

    //MegaChop(vsing, "Whole00.wav");
    //MegaChop(vsing, "Whole01.wav");
    //MegaChop(vsing, "Whole03.wav");

    MegaChop_Add(vsing, "Chopped.wav", "Whole.wav");

    delete vobox;

    delete vsing;

    // delete voz;// voice is deleted automatically when we delete vobox
  }
  {
      GroupBox *gb = new GroupBox();
      GroupBox::Group_OffsetBox *grobox = gb->Spawn_OffsetBox();
      delete grobox;
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
