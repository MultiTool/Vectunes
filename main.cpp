#include <iostream>

#include "ICloneable.hpp"
#include "ISonglet.hpp"
#include "CajaDelimitadora.hpp"

#include "IDrawable.hpp"
#include "MonkeyBox.hpp"
#include "IOffsetBox.hpp"
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

  Voice *voz;
  voz = new Voice();
  Voice::Voice_Singer *vs = voz->Spawn_Singer();
  delete vs;

  Voice::Voice_OffsetBox *vobox = voz->Spawn_OffsetBox();
  delete vobox;

  delete voz;

  IOffsetBox *obox = new IOffsetBox();

  IOffsetBox *oboxkid = obox->Clone_Me();

  Wave *wav = new Wave();
  cout << "Hello world!" << endl;
  delete wav;

  delete obox;
  delete oboxkid;

  return 0;
}
