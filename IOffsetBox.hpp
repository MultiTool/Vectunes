#ifndef IOffsetBox_hpp
#define IOffsetBox_hpp

#include "Globals.hpp"
#include "MonkeyBox.hpp"

class CajaDelimitadora;// forward
class ISonglet;// forward
class ISinger;// forward

#if false

class IOffsetBox{//: public MonkeyBox {
public:
  double TimeX = 0, OctaveY = 0, LoudnessFactor = 1.0;// all of these are in parent coordinates
  double ScaleX = 1.0, ScaleY = 1.0;// to be used for pixels per second, pixels per octave
  double ChildXorg = 0, ChildYorg = 0;// These are only non-zero for graphics. Audio origins are always 0,0.
  CajaDelimitadora *MyBounds;
  ISonglet *MyParentSong;// can do this but not used yet

// graphics support, will move to separate object
  double OctavesPerRadius = 0.03;
};

#else

class Factory;// forward
class IOffsetBox: public MonkeyBox { //implements IDrawable.IMoveable, IDeletable {// location box to transpose in pitch, move in time, etc.  //IOffsetBox,
public:
  Factory* MyFactory = InitFactory();
  String ContentName = "Content";
  /* ********************************************************************************* */
  IOffsetBox() {
    this->Create_Me();
    //MyBounds = new CajaDelimitadora();
    this->MyBounds.Reset();
  }
  ~IOffsetBox(){this->Delete_Me();}
  /* ********************************************************************************* */
  IOffsetBox* Clone_Me() override {// ICloneable
    IOffsetBox *child = new IOffsetBox();
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  IOffsetBox* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    IOffsetBox *child = this->Clone_Me();
    return child;
  }
  /* ********************************************************************************* */
  virtual void BreakFromHerd(CollisionLibrary& HitTable) {}
  /* ********************************************************************************* */
  void Copy_From(IOffsetBox& donor) {}
  /* ********************************************************************************* */
  double Get_Max_Amplitude() {
    return 0;
  }
  /* ********************************************************************************* */
  void Rebase_Time(double Time) {}
  /* ********************************************************************************* */
  virtual ISinger* Spawn_Singer() {// always always always override this
    throw std::runtime_error("Not supported yet.");
    return nullptr;
  }
  /* ********************************************************************************* */
  virtual void Rescale_TimeX(double Factor) {
    this->ScaleX = Factor;
  }
  /* ********************************************************************************* */
  virtual ISonglet* GetContent() {// always always always override this
    throw std::runtime_error("Not supported yet.");
    return nullptr;
  }
  // <editor-fold defaultstate="collapsed" desc="IDrawable and IMoveable">
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {// IDrawable
  }
  /* ********************************************************************************* */
  virtual void Draw_Dot(IDrawingContext& DC, Color& col) {}
  /* ********************************************************************************* */
  virtual void Draw_My_Bounds(IDrawingContext& ParentDC) {}
  /* ********************************************************************************* */
  virtual CajaDelimitadora* GetBoundingBox() {
    return nullptr;
  }
  void UpdateBoundingBox() {}
  void UpdateBoundingBoxLocal() {}
  void GoFishing(IGrabber& Scoop) {}
  void MoveTo(double XLoc, double YLoc) {}
  double GetX(){return this->TimeX;}
  double GetY(){return this->OctaveY;}
  boolean HitsMe(double XLoc, double YLoc) override {// IDrawable.IMoveable
    //System.out.print("HitsMe:");
    if (this->MyBounds.Contains(XLoc, YLoc)) {// redundant test
      double dist = Math::hypot(XLoc - this->TimeX, YLoc - this->OctaveY);
      if (dist <= this->OctavesPerRadius) {
        //System.out.println("true");
        return true;
      }
    }
    //System.out.println("false");
    return false;
  }
  // </editor-fold>
  /* ********************************************************************************* */
  boolean Create_Me() {
    return 0;
  }
  void Delete_Me() {}
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
  Factory* InitFactory() {// for serialization
    return nullptr;
  }
#if false
  /* ********************************************************************************* */
  class Factory: public IFactory {// for serialization
  public:
    OffsetBox* Create(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {
      return nullptr;
    }
  };
#endif // false

};
#endif // true

#endif // IOffsetBox_hpp
