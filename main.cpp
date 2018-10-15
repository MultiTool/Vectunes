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

    Wave wave;

    vsing->Render_To(1.0, wave);

    double *wav = wave.GetWave();

//    for (int wcnt=0;wcnt<wave.NumSamples;wcnt++){
//       double amp = wav[wcnt];
//       printf("amp:%f\n", amp);
//    }

    wave.Normalize();
    wave.SaveToWav("example.wav");

    delete vobox;

    delete vsing;

    delete voz;
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
