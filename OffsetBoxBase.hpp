#ifndef OffsetBoxBase_hpp
#define OffsetBoxBase_hpp

#include "Globals.hpp"
#include "MonkeyBox.hpp"

class CajaDelimitadora;// forward
class ISonglet;// forward
class SingerBase;// forward

class Factory;// forward
class OffsetBoxBase: public MonkeyBox { // IMoveable, IDeletable {// location box to transpose in pitch, move in time, etc.
public:
  Factory* MyFactory = InitFactory();
  String ContentName = "Content";
  /* ********************************************************************************* */
  OffsetBoxBase() {
    this->Create_Me();
    this->MyBounds.Reset();
  }
  ~OffsetBoxBase(){this->Delete_Me();}
  /* ********************************************************************************* */
  OffsetBoxBase* Clone_Me() override {// ICloneable
    OffsetBoxBase *child = new OffsetBoxBase();// clone
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  OffsetBoxBase* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    OffsetBoxBase *child = this->Clone_Me();
    return child;
  }
  /* ********************************************************************************* */
  virtual void BreakFromHerd(CollisionLibrary& HitTable) {}
  /* ********************************************************************************* */
  void Copy_From(OffsetBoxBase& donor) {}
  /* ********************************************************************************* */
//  double Get_Max_Amplitude() override {
//    return 0;
//  }
  double Get_Max_Amplitude() override {// always override this
    return this->LoudnessFactor;
  }
  /* ********************************************************************************* */
  virtual void Rebase_Time(double Time) {}
  /* ********************************************************************************* */
  virtual SingerBase* Spawn_Singer() {// always always always override this
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
//  virtual CajaDelimitadora* GetBoundingBox() {// IDrawable, not needed for monkeybox, fix this
//    return &(this->MyBounds);
//  }
  void UpdateBoundingBox() override {}
  void UpdateBoundingBoxLocal() override {}
  void GoFishing(IGrabber& Scoop) override {}
  void MoveTo(double XLoc, double YLoc) override {}
  double GetX() override {return this->TimeX;}
  double GetY() override {return this->OctaveY;}
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
  boolean Create_Me() override {// not needed probably
    return MonkeyBox::Create_Me();
  }
  void Delete_Me() override { MonkeyBox::Delete_Me(); }
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

#endif // OffsetBoxBase_hpp
