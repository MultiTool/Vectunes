#ifndef CajaDelimitadora_hpp
#define CajaDelimitadora_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "IDeletable.hpp"
#include "ITextable.hpp"
#include "Point2D.hpp"

/**
 *
 * @author MultiTool
 */

class CollisionLibrary;// forward
/* ********************************************************************************* */
class CajaDelimitadora: public IDeletable, ICloneable {//, ITextable {// DIY BoundingBox
public:
  Point2D Min, Max;
  Point2D *Limits[2];
  /* ********************************************************************************* */
  CajaDelimitadora() {
    this->Limits[0] = &Min;
    this->Limits[1] = &Max;
  }
  /* ********************************************************************************* */
  ~CajaDelimitadora() {
    this->Delete_Me();
  }
  /* ********************************************************************************* */
  void Assign(SoundFloat MinX, SoundFloat MinY, SoundFloat MaxX, SoundFloat MaxY) {
    this->Min.Assign(MinX, MinY);
    this->Max.Assign(MaxX, MaxY);
    this->Sort_Me();
  }
  /* ********************************************************************************* */
  void AssignCorner(int CornerNum, SoundFloat MinLimit, SoundFloat MaxLimit) {}
  /* ********************************************************************************* */
  boolean Intersects(const CajaDelimitadora& other) {
    if (!this->LineFramed(this->Min.x, this->Max.x, other.Min.x, other.Max.x)) {
      return false;
    } else if (!this->LineFramed(this->Min.y, this->Max.y, other.Min.y, other.Max.y)) {
      return false;
    }
    return true;
  }
  /* ********************************************************************************* */
  boolean LineFramed(SoundFloat MyMin, SoundFloat MyMax, SoundFloat YourMin, SoundFloat YourMax) {
    if (YourMax < MyMin) {// if my min is less than your max AND my max is greater than your min
      return false;
    } else if (MyMax < YourMin) {
      return false;
    }
    return true;
  }
  /* ********************************************************************************* */
  void Rebase_Time(SoundFloat TimeBase) {
    SoundFloat TimeRange = this->Max.x - this->Min.x;
    this->Min.x = TimeBase;
    this->Max.x = this->Min.x + TimeRange;
  }
  /* ********************************************************************************* */
  void Copy_From(const CajaDelimitadora& donor) {
    this->Min.CopyFrom(donor.Min);
    this->Max.CopyFrom(donor.Max);
  }
  /* ********************************************************************************* */
  CajaDelimitadora* Clone_Me() override {// ICloneable
    CajaDelimitadora* child = new CajaDelimitadora();// clone
    child->Copy_From(*this);
    return child;
  }
  CajaDelimitadora* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    return nullptr;// never used with CajaDelimitadora. reconsider inheriting from ICloneable.
  }
  /* ********************************************************************************* */
  boolean ZeroCheck() { // for debugging
    if (this->Min.x == 0.0) {
      if (this->Min.y == 0.0) {
        if (this->Max.x == 0.0) {
          if (this->Max.y == 0.0) {
            return true;
          }
        }
      }
    }
    return false;
  }
  /* ********************************************************************************* */
  void Sort_Me() {// CajaDelimitadora bounds are ALWAYS to be sorted min->max, even if we are in an inverted space such as screen graphics.
    SoundFloat temp;
    if (this->Max.x < this->Min.x) {
      temp = this->Max.x;// swap
      this->Max.x = this->Min.x;
      this->Min.x = temp;
    }
    if (this->Max.y < this->Min.y) {
      temp = this->Max.y;// swap
      this->Max.y = this->Min.y;
      this->Min.y = temp;
    }
    this->ZeroCheck();
  }
  /* ********************************************************************************* */
  SoundFloat GetWidth() {
    return Math::abs(this->Max.x - this->Min.x);
  }
  /* ********************************************************************************* */
  SoundFloat GetHeight() {
    return Math::abs(this->Max.y - this->Min.y);
  }
  /* ********************************************************************************* */
  void Reset() {// reset for min, max comparisons
    this->Min.Assign(Double_POSITIVE_INFINITY, Double_POSITIVE_INFINITY);
    this->Max.Assign(Double_NEGATIVE_INFINITY, Double_NEGATIVE_INFINITY);
    this->ZeroCheck();
  }
  /* ********************************************************************************* */
  void ClearZero() {// for empty boxes
    this->Min.Assign(0, 0);
    this->Max.Assign(0, 0);
  }
  /* ********************************************************************************* */
  void Include(const CajaDelimitadora& other) {// for aggregating with all of my child boxes
    this->Min.x = Math::min(this->Min.x, other.Min.x);
    this->Min.y = Math::min(this->Min.y, other.Min.y);
    this->Max.x = Math::max(this->Max.x, other.Max.x);
    this->Max.y = Math::max(this->Max.y, other.Max.y);
    this->ZeroCheck();
  }
  /* ********************************************************************************* */
  void IncludePoint(const Point2D& other) {// for aggregating with vertices
    IncludePoint(other.x, other.y);
  }
  /* ********************************************************************************* */
  void IncludePoint(SoundFloat OtherX, SoundFloat OtherY) {// for aggregating with vertices
    this->Min.x = Math::min(this->Min.x, OtherX);
    this->Min.y = Math::min(this->Min.y, OtherY);
    this->Max.x = Math::max(this->Max.x, OtherX);
    this->Max.y = Math::max(this->Max.y, OtherY);
    this->ZeroCheck();
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {
    return true;
  }
  void Delete_Me() override {// if you can't delete it, at least mess it up so it can't be reused without exploding
    Min.Delete_Me(); Max.Delete_Me();// wreck everything
    this->Limits[0] = nullptr;
    this->Limits[1] = nullptr;
  }
  /* ********************************************************************************* */
  boolean Contains(SoundFloat XLoc, SoundFloat YLoc) {
    if (this->Min.x <= XLoc) {
      if (XLoc <= this->Max.x) {
        if (this->Min.y <= YLoc) {
          if (YLoc <= this->Max.y) {
            return true;
          }
        }
      }
    }
    return false;
  }
  /* ********************************************************************************* */
//  JsonParse::Node* Export(CollisionLibrary& HitTable)=0;// to do: pass a collision table parameter
//  void ShallowLoad(JsonParse::Node& phrase)=0;// just fill in primitive fields that belong to this object, don't follow up pointers.
//  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances)=0;// Fill in all the values of an already-created object, including deep pointers.
#if false
  JsonParse::Node* Export(CollisionLibrary& HitTable) override {// ITextable
    JsonParse.Node phrase = neuvo JsonParse.Node();
    phrase.ChildrenHash = neuvo HashMap<String, JsonParse.Node>();
    phrase.AddSubPhrase("Min", IFactory.Utils.PackField(this->Min.toString()));
    phrase.AddSubPhrase("Max", IFactory.Utils.PackField(this->Max.toString()));
    return phrase;
  }
  void ShallowLoad(JsonParse::Node& phrase) override {// ITextable
    HashMap<String, JsonParse.Node> Fields = phrase.ChildrenHash;
    throw std::runtime_error("Not supported yet.");
  }
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) override {// ITextable - Fill in all the values of an already-created object, including deep pointers.
    throw std::runtime_error("Not supported yet.");
  }
#endif // false
};

#endif
