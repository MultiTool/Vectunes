#ifndef VoicePoint_hpp
#define VoicePoint_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "MonkeyBox.hpp"
#include "IDrawingContext.hpp"
#include "ITextable.hpp"
#include "IDeletable.hpp"

/**
 *
 * @author MultiTool
*/

class LoudnessHandle;// forward
class Voice;// forward
//class DrawingContext;// forward
class IDeletable;// forward
/* ********************************************************************************* */
class VoicePoint: public MonkeyBox {
public:
  double SubTime = 0.0;// SubTime is cumulative subjective time.

  // graphics support, will move to separate object
  double OctavesPerLoudness = 0.125;// to do: loudness will have to be mapped to screen. not a pixel value right?
  LoudnessHandle *UpHandle, *DownHandle;
  String UpHandleName = {"UpHandle"}, DownHandleName = {"DownHandle"};
  Voice* MyParentVoice = null;
  /* ********************************************************************************* */
  VoicePoint() {}
  /* ********************************************************************************* */
  ~VoicePoint() { this->Delete_Me(); }
  /* ********************************************************************************* */
  void RefParent(Voice& Parent) {}
  /* ********************************************************************************* */
  void CopyFrom(VoicePoint& source) {}
  /* ********************************************************************************* */
  double GetFrequencyFactor() {
    return 0;
  }
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {}
  /* ********************************************************************************* */
  CajaDelimitadora* GetBoundingBox() override {
    return nullptr;
  }
  /* ********************************************************************************* */
  void UpdateBoundingBox() override {}
  void UpdateBoundingBoxLocal() override {}
  /* ********************************************************************************* */
  void GoFishing(IGrabber& Scoop) override {}
  boolean HitsMe(double XLoc, double YLoc) override {
    return 0;
  }
  /* ********************************************************************************* */
  VoicePoint* Clone_Me() override {
    return nullptr;
  }
  /* ********************************************************************************* */
  VoicePoint* Deep_Clone_Me(CollisionLibrary& HitTable) override {
    return nullptr;
  }
  /* ********************************************************************************* */
  void Copy_From(VoicePoint& donor) {}
  /* ********************************************************************************* */
  boolean Create_Me() override {
    return 0;
  }
  void Delete_Me() override {}
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) {
    return nullptr;
  }
  void ShallowLoad(JsonParse::Node& phrase) {}
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
};

  /* ********************************************************************************* */
  class LoudnessHandle: public IMoveable, IDeletable, ITextable {// probably should not be ITextable
  public:
    CajaDelimitadora MyBounds;
    VoicePoint* ParentPoint;
    double OctavesPerRadius = 0.007;
    boolean IsSelected = false;
    /* ********************************************************************************* */
    void MoveTo(double XLoc, double YLoc) {}
    double GetX() override {
      return 0;
    }
    double GetY() override {
      return 0;
    }
    boolean HitsMe(double XLoc, double YLoc) override {
      return 0;
    }
    void SetSelected(boolean Selected) {}
    void Draw_Me(IDrawingContext& ParentDC) override {}
    CajaDelimitadora* GetBoundingBox() {
      return nullptr;
    }
    void UpdateBoundingBox() override {}
    void UpdateBoundingBoxLocal() override {}
    void GoFishing(IGrabber& Scoop) override {}
    /* ********************************************************************************* */
    LoudnessHandle* Clone_Me() override {
      return nullptr;
    }
    /* ********************************************************************************* */
    LoudnessHandle* Deep_Clone_Me(CollisionLibrary& HitTable) override {
      return nullptr;
    }
    /* ********************************************************************************* */
    boolean Create_Me() override {
      return 0;
    }
    void Delete_Me() override {}
    /* ********************************************************************************* */
    JsonParse::Node* Export(CollisionLibrary& HitTable) {
      return nullptr;
    }
    void ShallowLoad(JsonParse::Node& phrase) {}
    void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
  };


#endif
