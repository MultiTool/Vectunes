#ifndef VoicePoint_hpp
#define VoicePoint_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "IDeletable.hpp"
#include "MonkeyBox.hpp"
#include "IDrawingContext.hpp"
#include "ITextable.hpp"
#include "IGrabber.hpp"

//#include "Voice.hpp"  {//
/**
 *
 * @author MultiTool
*/

class LoudnessHandle;// forward
class Voice;// forward
//class DrawingContext;// forward
//class IDeletable;// forward
/* ********************************************************************************* */
class VoicePoint: public MonkeyBox {
public:
  double SubTime = 0.0;// SubTime is cumulative subjective time.

  // graphics support, will move to separate object
  double OctavesPerLoudness = 0.125;// to do: loudness will have to be mapped to screen. not a pixel value right?
  String UpHandleName = {"UpHandle"}, DownHandleName = {"DownHandle"};
  Voice* MyParentVoice = null;
  /* ********************************************************************************* */
  VoicePoint() {
    this->Create_Me();
  }
  /* ********************************************************************************* */
  ~VoicePoint() { this->Delete_Me(); }
  /* ********************************************************************************* */
  class LoudnessHandle: public IMoveable {//, public IDeletable {//, ITextable {// probably should not be ITextable
  public:
    CajaDelimitadora MyBounds;
    VoicePoint* ParentPoint;
    double OctavesPerRadius = 0.007;
    boolean IsSelected = false;
    /* ********************************************************************************* */
    LoudnessHandle(){ this->Create_Me(); }
    ~LoudnessHandle(){ this->Delete_Me(); }
    /* ********************************************************************************* */
    void MoveTo(double XLoc, double YLoc) {// IMoveable
      if (XLoc >= 0) {// don't go backward in time
        this->ParentPoint->TimeX = XLoc;
      }
      double RelativeY = YLoc - this->ParentPoint->OctaveY;
      if (RelativeY >= 0) {// non negative loudness
        RelativeY /= this->ParentPoint->OctavesPerLoudness;
        if (RelativeY <= 1.0) {// shouldn't amplify loudness above 1.0, so that we can keep wave clipping under control
          this->ParentPoint->LoudnessFactor = RelativeY;
        }
      }
    }
    double GetX() override {
      return this->ParentPoint->TimeX;
    }
    double GetY() override {
      double LoudnessHeight = this->ParentPoint->LoudnessFactor * this->ParentPoint->OctavesPerLoudness;// Map loudness to screen pixels.
      return this->ParentPoint->OctaveY + LoudnessHeight;
    }
    boolean HitsMe(double XLoc, double YLoc) override {// IMoveable
      // System.out.print("** LoudnessHandle HitsMe:");
      boolean Hit = false;
      if (this->MyBounds.Contains(XLoc, YLoc)) {
        // System.out.print(" InBounds ");
        double dist = Math::hypot(XLoc - this->GetX(), YLoc - (this->GetY() + this->OctavesPerRadius));
        if (dist <= this->OctavesPerRadius) {
          // System.out.print(" Hit!");
          Hit = true;
        } else {
          // System.out.print(" Missed!");
        }
      } else {
        // System.out.print(" OutBounds ");
      }
      return Hit;
    }
    void SetSelected(boolean Selected) {// IMoveable
      this->IsSelected = Selected;
    }
    void Draw_Me(IDrawingContext& ParentDC) override {
      // Control points have the same space as their parent, so no need to create a local map.
//      Point2D pnt = ParentDC.To_Screen(this->ParentPoint->TimeX, this->ParentPoint->OctaveY);
//      double RadiusPixels = Math::abs(ParentDC.GlobalOffset.ScaleY) * OctavesPerRadius;
//      double LoudnessHgt = this->ParentPoint->LoudnessFactor * this->ParentPoint->OctavesPerLoudness;
//      double YlocHigh = ParentDC.GlobalOffset.UnMapPitch(this->ParentPoint->OctaveY + LoudnessHgt) - RadiusPixels;// My handle rests *upon* the line I control, so I don't occlude my VoicePoint.
//
//      RadiusPixels = Math::ceil(RadiusPixels);
//      double DiameterPixels = RadiusPixels * 2.0;IDeletable
//
//      MonkeyBox.Draw_Dot2(ParentDC, pnt.x, YlocHigh, OctavesPerRadius, this->IsSelected, Globals.ToAlpha(Color.lightGray, 100));
//
//      if (false) {
//        ParentDC.gr.setColor(Globals.ToAlpha(Color.lightGray, 100));// control point just looks like a dot
//        ParentDC.gr.fillOval((int) (pnt.x - RadiusPixels), (int) (YlocHigh - RadiusPixels), (int) DiameterPixels, (int) DiameterPixels);
//        ParentDC.gr.setColor(Globals.ToAlpha(Color.darkGray, 200));
//        ParentDC.gr.drawOval((int) (pnt.x - RadiusPixels), (int) (YlocHigh - RadiusPixels), (int) DiameterPixels, (int) DiameterPixels);
//      }
    }
    CajaDelimitadora* GetBoundingBox() override {
      return &(this->MyBounds);
    }
    void UpdateBoundingBox() override {
      this->UpdateBoundingBoxLocal();
    }
    void UpdateBoundingBoxLocal() override {
      double XLoc = this->GetX();
      double YLoc = this->GetY() + this->OctavesPerRadius;// *upon* the line
      double MinX = XLoc - this->OctavesPerRadius;
      double MaxX = XLoc + this->OctavesPerRadius;
      double MinY = YLoc - this->OctavesPerRadius;
      double MaxY = YLoc + this->OctavesPerRadius;
      this->MyBounds.Assign(MinX, MinY, MaxX, MaxY);
    }
    void GoFishing(IGrabber& Scoop) override {
      // System.out.println();
      // System.out.print(" LoudnessHandle GoFishing: ");
      if (Scoop.Intersects(MyBounds)) {
        // System.out.print(" InBounds, ");
        Scoop.ConsiderLeaf(*this);
      }
      // System.out.println();
    }
    /* ********************************************************************************* */
    void Copy_From(LoudnessHandle &donor) {
      this->MyBounds.Copy_From(donor.MyBounds);
      this->ParentPoint = donor.ParentPoint;
      this->OctavesPerRadius = donor.OctavesPerRadius;
      //this->IsSelected = donor.IsSelected;
      this->ParentPoint = donor.ParentPoint;
    }
    /* ********************************************************************************* */
    LoudnessHandle* Clone_Me() override {// ICloneable
      LoudnessHandle *child = new LoudnessHandle();
      return child;
    }
    /* ********************************************************************************* */
    LoudnessHandle* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
      LoudnessHandle *child = new LoudnessHandle();
      child->OctavesPerRadius = this->OctavesPerRadius;
      child->ParentPoint = this->ParentPoint;
      child->MyBounds.Copy_From(this->MyBounds);
      return child;
    }
    /* ********************************************************************************* */
    boolean Create_Me() {//override {// IDeletable
      return true;
    }
    void Delete_Me() {//override {// IDeletable
      this->MyBounds.Delete_Me();// wreck everything
      this->OctavesPerRadius = Double_NEGATIVE_INFINITY;
      this->ParentPoint = null;
      this->IsSelected = false;
    }
    /* ********************************************************************************* */
    JsonParse::Node* Export(CollisionLibrary& HitTable) {
      return nullptr;
    }
    void ShallowLoad(JsonParse::Node& phrase) {}
    void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
  };
  /* ********************************************************************************* */
  LoudnessHandle UpHandle, DownHandle;
  /* ********************************************************************************* */
  void RefParent(Voice* Parent) {
    //this->MyParentSong = this->MyParentVoice = Parent;
    this->MyParentVoice = Parent;
  }
  /* ********************************************************************************* */
  void CopyFrom(VoicePoint& source) {
    MonkeyBox::Copy_From(source);
    this->SubTime = source.SubTime;
  }
  /* ********************************************************************************* */
  double GetFrequencyFactor() {
    return Math::pow(2.0, this->OctaveY);
  }
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {// IDrawable
    // Control points have the same space as their parent, so no need to create a local map.
//    Point2D pnt = ParentDC.To_Screen(this->TimeX, this->OctaveY);
//    double RadiusPixels = Math::abs(ParentDC.GlobalOffset.ScaleY) * OctavesPerRadius;
//    RadiusPixels = Math::ceil(RadiusPixels);
//    double DiameterPixels = RadiusPixels * 2.0;
//    this->UpHandle.Draw_Me(ParentDC);
//    MonkeyBox.Draw_Dot2(ParentDC, pnt.x, pnt.y, OctavesPerRadius, this->IsSelected, Globals.ToAlpha(Color.yellow, 200));
//    if (false) {
//      ParentDC.gr.setColor(Globals.ToAlpha(Color.yellow, 200));// control point just looks like a dot
//      ParentDC.gr.fillOval((int) (pnt.x - RadiusPixels), (int) (pnt.y - RadiusPixels), (int) DiameterPixels, (int) DiameterPixels);
//      ParentDC.gr.setColor(Globals.ToAlpha(Color.darkGray, 200));
//      ParentDC.gr.drawOval((int) (pnt.x - RadiusPixels), (int) (pnt.y - RadiusPixels), (int) DiameterPixels, (int) DiameterPixels);
//    }
  }
  /* ********************************************************************************* */
  CajaDelimitadora* GetBoundingBox() override {
    return &(this->MyBounds);
  }
  /* ********************************************************************************* */
  void UpdateBoundingBox() override {// IDrawable
    this->UpHandle.UpdateBoundingBox();
    this->UpdateBoundingBoxLocal();// Points have no children, nothing else to do.
  }
  void UpdateBoundingBoxLocal() override {// IDrawable
    double LoudnessHeight = LoudnessFactor * OctavesPerLoudness;// Map loudness to screen pixels.
    double MinX = TimeX - OctavesPerRadius;
    double MaxX = TimeX + OctavesPerRadius;
    double HeightRad = Math::max(OctavesPerRadius, LoudnessHeight);
    double MinY = OctaveY - HeightRad;
    double MaxY = OctaveY + HeightRad;
    this->MyBounds.Assign(MinX, MinY, MaxX, MaxY);

    this->MyBounds.Include(*(this->UpHandle.GetBoundingBox()));// Don't have to UnMap in this case because my points are already in my internal coordinates.
  }
  /* ********************************************************************************* */
  void GoFishing(IGrabber& Scoop) override {// IDrawable
    // System.out.print(" Point GoFishing: ");
    if (Scoop.Intersects(MyBounds)) {
      // System.out.print(" InBounds, ");
      Scoop.ConsiderLeaf(*this);
      this->UpHandle.GoFishing(Scoop);
    }
    // System.out.println();
  }
  boolean HitsMe(double XLoc, double YLoc) override {// IDrawable.IMoveable
    // System.out.print("** Point HitsMe:");
    boolean Hit = false;
    if (this->MyBounds.Contains(XLoc, YLoc)) {
      // System.out.print(" InBounds ");
      double dist = Math::hypot(XLoc - this->TimeX, YLoc - this->OctaveY);
      if (dist <= this->OctavesPerRadius) {
        // System.out.print(" Hit!");
        Hit = true;
      } else {
        // System.out.print(" Missed!");
      }
    } else {
      // System.out.print(" OutBounds ");
    }
    return Hit;
  }
  /* ********************************************************************************* */
  VoicePoint* Clone_Me() override {// ICloneable
    VoicePoint *child = new VoicePoint();
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  VoicePoint* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    VoicePoint *child = new VoicePoint();
    child->Copy_From(*this);
    child->UpHandle.Copy_From(this->UpHandle);
    child->UpHandle.ParentPoint = child;

    child->DownHandle.Copy_From(this->DownHandle);
    child->DownHandle.ParentPoint = child;

//    (child->UpHandle = this->UpHandle.Deep_Clone_Me(HitTable)).ParentPoint = child; //child.UpHandle.ParentPoint = child;
//    (child->DownHandle = this->DownHandle.Deep_Clone_Me(HitTable)).ParentPoint = child; //child.DownHandle.ParentPoint = child;
    return child;
  }
  /* ********************************************************************************* */
  void Copy_From(VoicePoint& donor) {
    MonkeyBox::Copy_From(donor);
    this->SubTime = donor.SubTime;
    this->OctavesPerLoudness = donor.OctavesPerLoudness;
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {// IDeletable
    //this->UpHandle = new LoudnessHandle();
    this->UpHandle.ParentPoint = this;
    //this->DownHandle = new LoudnessHandle();
    this->DownHandle.ParentPoint = this;
    return true;
  }
  void Delete_Me() override {// IDeletable
    this->SubTime = Double_NEGATIVE_INFINITY;// wreck everything
    this->OctavesPerLoudness = Double_NEGATIVE_INFINITY;
    this->MyBounds.Delete_Me();
    this->UpHandle.Delete_Me();
    //this->UpHandle = null;
    this->DownHandle.Delete_Me();
    //this->DownHandle = null;
  }
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) {
    return nullptr;
  }
  void ShallowLoad(JsonParse::Node& phrase) {}
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
};


#endif
