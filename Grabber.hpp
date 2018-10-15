#ifndef Grabber_hpp
#define Grabber_hpp

#include <vector>
#include "Globals.hpp"
#include "Wave.hpp"
#include "CajaDelimitadora.hpp"
#include "IGrabber.hpp"
#include "ITextable.hpp"

/**

 @author MultiTool
 Grabber is basically a spatial query which also carries back all the results data.

*/

class GroupBox;// forward

/* ********************************************************************************* */
class StackItem: public IDeletable {
public:
  CajaDelimitadora SearchBounds;
  OffsetBoxBase* OBox;
  int HitDex;
  Point2D Loc;
 /* so do we pass HitDex to all songlets, or to all oboxes? only songlets can use it directly.
  and when do we use it? we use it when we move the thing we grabbed, in MapThroughStack
  we would have to pass it through every mapto.  ack.
  why not just give
  */
  /* ********************************************************************************* */
  StackItem(){}
  ~StackItem(){this->Delete_Me();}
  /* ********************************************************************************* */
  void Copy_From(StackItem& donor) {
    this->SearchBounds.Copy_From(donor.SearchBounds);
    this->OBox = donor.OBox;
    this->HitDex = donor.HitDex;
    this->Loc.setLocation(donor.Loc);
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {// IDeletable
    return true;
  }
  void Delete_Me() override {// IDeletable
    this->SearchBounds.Delete_Me();// redundant, should be automatic
    this->OBox = null;
    this->Loc.Assign(Double_NEGATIVE_INFINITY, Double_NEGATIVE_INFINITY);
    this->HitDex = Integer_MAX_VALUE;
  }
};

/* ********************************************************************************* */
class Grabber: public IGrabber {
public:
  double XHit, YHit;// exact mouse click point
  StackItem *CurrentContext = null;
  int Stack_Depth = 0, Stack_Depth_Best = 0;
  ArrayList<StackItem> Explore_Stack;
  ArrayList<StackItem> Best_Stack;
  IMoveable *Leaf;// thing we hit and are going to move or copy or whatever
  double Radius = 5;
  /* ********************************************************************************* */
  virtual void ConsiderLeaf(IMoveable& CandidateLeaf) {
    if (CandidateLeaf.HitsMe(this->CurrentContext->Loc.x, this->CurrentContext->Loc.y)) {
      //System.out.print(" Was Hit, ");
      if (this->Stack_Depth_Best <= this->Stack_Depth) {// prefer the most distal
        this->Stack_Depth_Best = this->Stack_Depth;// or if equal, prefer the last drawn (most recent hit)
        this->Leaf = &CandidateLeaf;
        Copy_Stack(this->Explore_Stack, this->Best_Stack);
      }
      // this->Compare(this->Leaf, leaf);
    }
  }
  /* ********************************************************************************* */
  virtual boolean KeepDigging(IMoveable& Candidate) { // Determine whether we should look into the children of Candidate to select a songlet
    return (this->CurrentContext->SearchBounds.Intersects(*Candidate.GetBoundingBox()));
  }
  /* ********************************************************************************* */
  void Init(OffsetBoxBase& starter, double XLoc, double YLoc) {// add first space map at start of search
//    OffsetBoxBase child = neuvo OffsetBoxBase();// first layer place holder.  not a great solution.
//    child.MyBounds.Assign(Double_NEGATIVE_INFINITY, Double_NEGATIVE_INFINITY, Double_POSITIVE_INFINITY, Double_POSITIVE_INFINITY);
//    StackItem next = neuvo StackItem();
//    next.OBox = child;
//    next.Loc.x = XLoc;// Next's location values exist in the space above it. At the top they are mouse coordinates.
//    next.Loc.y = YLoc;
//    next.SearchBounds.Assign(XLoc - Radius, YLoc - Radius, XLoc + Radius, YLoc + Radius);
//    this->Explore_Stack.add(next);
//    this->CurrentContext = next;
//    Stack_Depth = 1;// Now we have one element, whee!
  }
  /* ********************************************************************************* */
  virtual void AddFirstBox(OffsetBoxBase& starter, double XLoc, double YLoc) {// add first space map at start of search
//    this->Leaf = null;
//    this->Stack_Depth_Best = 0;
//    this->Stack_Depth = 0;
//    TruncateStack(this->Explore_Stack, 0);
//    StackItem next = neuvo StackItem();
//    next.OBox = starter;
////    next.Loc.x = XLoc;// Next's location values exist in the space above it. At the top they are mouse coordinates.
////    next.Loc.y = YLoc;
////    next.SearchBounds.Assign(XLoc - Radius, YLoc - Radius, XLoc + Radius, YLoc + Radius);
//
//    // map to child space
//    CajaDelimitadora SearchBoundsTemp = neuvo CajaDelimitadora();
//    SearchBoundsTemp.Assign(XLoc - Radius, YLoc - Radius, XLoc + Radius, YLoc + Radius);
//    starter.MapTo(SearchBoundsTemp, next.SearchBounds);// prev.SearchBounds.Map(child, next.SearchBounds);
//    SearchBoundsTemp.Delete_Me();
//    starter.MapTo(neuvo Point2D(XLoc, YLoc), next.Loc);
//
//    this->Explore_Stack.add(next);
//    this->CurrentContext = next;
//    Stack_Depth = 1;// Now we have one element, whee!
  }
  /* ********************************************************************************* */
  void AddBoxToStack(OffsetBoxBase& child) {}
  void DecrementStack() {}
  void TruncateStack(ArrayList<StackItem>& Stack, int resize) {}
  /* ********************************************************************************* */
  int Compare(IMoveable& thing0, IMoveable& thing1) { return 0; }
  /* ********************************************************************************* */
  void Copy_Stack(ArrayList<StackItem>& StackPrev, ArrayList<StackItem>& StackNext) {}
  /* ********************************************************************************* */
  void UnMapThroughStack(double XLoc, double YLoc, Point2D& results) {}
  /* ********************************************************************************* */
  void MapThroughStack(double XLoc, double YLoc, Point2D& results) {}
  /* ********************************************************************************* */
  void MapThroughStack(double XLoc, double YLoc, MonkeyBox& startplace, Point2D& results) {}
  /* ********************************************************************************* */
  void CompoundStack(MonkeyBox& startplace, MonkeyBox& results) {}
  /* ********************************************************************************* */
  void UpdateBoundingBoxes() {}
  /* ********************************************************************************* */
  boolean Intersects(CajaDelimitadora& OtherBounds) override {
    return this->CurrentContext->SearchBounds.Intersects(OtherBounds);
  }
};

/* ********************************************************************************* */
class DestinationGrabber: public Grabber {// this class searches for containers in which to drop a floating, copied songlet
public:
  OffsetBoxBase* Floater = null;
  GroupBox* PossibleDestination = null;
  double ClosestDistance = Double_POSITIVE_INFINITY;
  void AddFirstBox(OffsetBoxBase& starter, double XLoc, double YLoc) override {}
  /* ********************************************************************************* */
  void ConsiderLeaf(IMoveable& CandidateLeaf) override {}
  /* ********************************************************************************* */
  boolean KeepDigging(IMoveable& Candidate) override { return 0; }
};

#endif
