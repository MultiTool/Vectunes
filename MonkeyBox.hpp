#ifndef MonkeyBox_hpp
#define MonkeyBox_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "IDrawable.hpp"
#include "CajaDelimitadora.hpp"
#include "IDeletable.hpp"
#include "ITextable.hpp"
#include "JsonParse.hpp"
#include "ISonglet.hpp"

/**
 *
 * @author MultiTool
 *
 // * MonkeyBox will be a base class for both OffsetBox and Voice Point.
 // * It is any movable point that can contain other movable points.
 // * The name is a place holder until something better comes to mind.
 *
 */
//class IDrawable;// forward
//class IDrawable::IMoveable; // forward
class CollisionLibrary;// forward
class ISonglet;// forward
class MonkeyBox: public IMoveable, public IDeletable, public ITextable {// location box to transpose in pitch, move in time, etc.  //IMonkeyBox,
public:
  double TimeX = 0, OctaveY = 0, LoudnessFactor = 1.0;// all of these are in parent coordinates
  double ScaleX = 1.0, ScaleY = 1.0; // to be used for pixels per second, pixels per octave
  // ScaleY is 'temper' context, which we will NEVER use unless we want to make ugly anharmonic noise.
  String TimeXName = "TimeX", OctaveYName = "OctaveY", LoudnessFactorName = "LoudnessFactor", ScaleXName = "ScaleX", ScaleYName = "ScaleY";// for serialization
  CajaDelimitadora MyBounds;
  ISonglet* MyParentSong;// can do this but not used yet

  // graphics support, will move to separate object
  //double OctavesPerRadius = 0.03;
  double OctavesPerRadius = 0.01;
  boolean IsSelected = false;

  /* ********************************************************************************* */
  MonkeyBox() {this->Create_Me();}
  ~MonkeyBox(){this->Delete_Me();}
  /* ********************************************************************************* */
  MonkeyBox* Clone_Me() override {
    MonkeyBox *child = new MonkeyBox();// clone
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  MonkeyBox* Deep_Clone_Me(CollisionLibrary& HitTable) {
    MonkeyBox *child = this->Clone_Me();
    return child;
  }
  /* ********************************************************************************* */
  void Copy_From(MonkeyBox& donor) {
    this->TimeX = donor.TimeX;
    this->OctaveY = donor.OctaveY;
    this->LoudnessFactor = donor.LoudnessFactor;
    this->ScaleX = donor.ScaleX;
    this->ScaleY = donor.ScaleY;
    this->MyParentSong = donor.MyParentSong;
    this->OctavesPerRadius = donor.OctavesPerRadius;
    this->MyBounds.Copy_From(donor.MyBounds);
  }
  /* ********************************************************************************* */
  void Clear() {// set all coordinates to identity, no transformation for content
    TimeX = OctaveY = 0.0;
    LoudnessFactor = 1.0;
    ScaleX = ScaleY = 1.0;
  }
  /* ********************************************************************************* */
  void CopyTo_Point2D(Point2D& pnt) {
    pnt.x = TimeX; pnt.y = OctaveY;
  }
  /* ********************************************************************************* */
  virtual double Get_Max_Amplitude() {// always override this
    return this->LoudnessFactor;
  }
  /* ********************************************************************************* */
  void Compound(MonkeyBox& donor) {
    this->TimeX += (this->ScaleX * donor.TimeX);// to do: combine matrices here.
    this->OctaveY += (this->ScaleY * donor.OctaveY);
    this->LoudnessFactor *= donor.LoudnessFactor;
    this->ScaleX *= donor.ScaleX;
    this->ScaleY *= donor.ScaleY;
  }
  /* ********************************************************************************* */
  void RescaleTimeX(double Factor) {
    this->ScaleX = Factor;
  }
  /* ********************************************************************************* */
  void Rebase_Time(double Time) {
    this->TimeX = Time;
    double RelativeMinBound = this->MyBounds.Min.x;// preserve the relative relationship of my bounds and my origin.
    this->MyBounds.Rebase_Time(Time + RelativeMinBound);
  }
  // <editor-fold defaultstate="collapsed" desc="Mappings and Unmappings">
  /* ********************************************************************************* */
  double MapTime(double ParentTime) {// convert time coordinate from my parent's frame to my child's frame
    return ((ParentTime - this->TimeX) / ScaleX); // in the long run we'll probably use a matrix
  }
  /* ********************************************************************************* */
  double UnMapTime(double ChildTime) {// convert time coordinate from my child's frame to my parent's frame
    return this->TimeX + ((ChildTime) * ScaleX);
  }
  /* ********************************************************************************* */
  double MapPitch(double ParentPitch) {// convert octave coordinate from my parent's frame to my child's frame
    return ((ParentPitch - this->OctaveY) / ScaleY);
  }
  /* ********************************************************************************* */
  double UnMapPitch(double ChildPitch) {// convert octave coordinate from my child's frame to my parent's frame
    return this->OctaveY + ((ChildPitch) * ScaleY);
  }
  /* ********************************************************************************* */
  void MapTo(double XLoc, double YLoc, Point2D& results) {
    results.setLocation(this->MapTime(XLoc), this->MapPitch(YLoc));
  }
  /* ********************************************************************************* */
  void UnMap(double XLoc, double YLoc, Point2D& results) {
  }
  /* ********************************************************************************* */
  void MapTo(Point2D& pnt, Point2D& results) {
    results.x = this->MapTime(pnt.x);
    results.y = this->MapPitch(pnt.y);
  }
  /* ********************************************************************************* */
  void UnMap(Point2D& pnt, Point2D& results) {
    results.x = this->UnMapTime(pnt.x);
    results.y = this->UnMapPitch(pnt.y);
  }
  /* ********************************************************************************* */
  void MapTo(CajaDelimitadora& source, CajaDelimitadora& results) {
    this->MapTo(source.Min, results.Min);
    this->MapTo(source.Max, results.Max);
    results.Sort_Me();
  }
  /* ********************************************************************************* */
  void UnMap(CajaDelimitadora& source, CajaDelimitadora& results) {
    this->UnMap(source.Min, results.Min);
    this->UnMap(source.Max, results.Max);
    results.Sort_Me();
  }
  // </editor-fold>
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) {}
  /* ********************************************************************************* */
  void Draw_Dot(IDrawingContext& ParentDC, Color& col) {}
  /* ********************************************************************************* */
  void Draw_Dot2(IDrawingContext& DC, double XCtr, double YCtr, double OctavesPerRadius, boolean Selected, Color& col) {}
  /* ********************************************************************************* */
  CajaDelimitadora* GetBoundingBox() override {// IDrawable
    return &(this->MyBounds);
  }
  void UpdateBoundingBox() override {// IDrawable
  }
  void UpdateBoundingBoxLocal() override {// IDrawable
  }
  void GoFishing(IGrabber& Scoop) override {// IDrawable
  }
  void MoveTo(double XLoc, double YLoc) override {// IMoveable
  }
  double GetX() override {// IMoveable
    return 0;
  }
  double GetY() override {// IMoveable
    return 0;
  }
  boolean HitsMe(double XLoc, double YLoc) override {// IMoveable
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
  void SetSelected(boolean Selected) override {// IMoveable
    this->IsSelected = Selected;
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {// IDeletable
    return 0;
  }
  void Delete_Me() override {// IDeletable
    this->TimeX = Double_NEGATIVE_INFINITY; this->OctaveY = Double_NEGATIVE_INFINITY;
    this->LoudnessFactor = Double_NEGATIVE_INFINITY;
    this->ScaleX = ScaleY = Double_NEGATIVE_INFINITY;
  }
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) override {// ITextable
    return nullptr;
  }
  void ShallowLoad(JsonParse::Node& phrase) override {// ITextable
  }
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) override {// ITextable
  }
};

#endif // MonkeyBox_hpp
