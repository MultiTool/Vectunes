#ifndef IDrawable_hpp
#define IDrawable_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "CajaDelimitadora.hpp"
#include "ICloneable.hpp"
#include "IDrawingContext.hpp"

/**
 *
 * @author MultiTool
 *
 * This IDrawable approach is dubious. We will have to think it through a lot more.
 */
class IGrabber;// forward
class IDrawable: public ICloneable {
public:
  //class IMoveable;// forward
  virtual void Draw_Me(IDrawingContext& ParentDC) {}
  virtual CajaDelimitadora* GetBoundingBox() { return nullptr; };
  virtual void UpdateBoundingBox() {}
  virtual void UpdateBoundingBoxLocal() {}
  virtual void GoFishing(IGrabber& Scoop) {}// look for mouse clicks on me or my children
  virtual IDrawable* Clone_Me() { return nullptr; };
  virtual IDrawable* Deep_Clone_Me(CollisionLibrary& HitTable) { return nullptr; };
};

class IMoveable: public IDrawable {// IMoveable is for things that can be selected, dragged, copied, pasted, deleted etc. through the UI.
public:
  boolean IsSelected = false;
  virtual void MoveTo(ldouble XLoc, ldouble YLoc) {};
  virtual boolean HitsMe(ldouble XLoc, ldouble YLoc) { return false; };// click detection
  virtual ldouble GetX() { return Double_NEGATIVE_INFINITY; };
  virtual ldouble GetY() { return Double_NEGATIVE_INFINITY; };
  virtual void SetSelected(boolean Selected) { this->IsSelected = Selected; }
  virtual IMoveable* Clone_Me() { return nullptr; };
  virtual IMoveable* Deep_Clone_Me(CollisionLibrary& HitTable) { return nullptr; };
};

#endif
