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
//class IDrawingContext;// forward
class IDrawable: public ICloneable {
public:
  class IMoveable;// forward
  virtual void Draw_Me(IDrawingContext& ParentDC) = 0;
  virtual CajaDelimitadora* GetBoundingBox() = 0;
  virtual void UpdateBoundingBox() = 0;
  virtual void UpdateBoundingBoxLocal() = 0;
  virtual void GoFishing(IGrabber& Scoop) = 0;
  virtual IDrawable* Clone_Me() = 0;
  virtual IDrawable* Deep_Clone_Me(CollisionLibrary& HitTable) = 0;
//  class IMoveable: public IDrawable {// IMoveable is for things that can be selected, dragged, copied, pasted, deleted etc. through the UI.
//    virtual void MoveTo(double XLoc, double YLoc) = 0;
//    virtual boolean HitsMe(double XLoc, double YLoc) = 0;// click detection
//    virtual double GetX() = 0;
//    virtual double GetY() = 0;
//    virtual void SetSelected(boolean Selected) = 0;
//    virtual IMoveable* Clone_Me() = 0;
//    virtual IMoveable* Deep_Clone_Me(CollisionLibrary& HitTable) = 0;
//  };
};

class IMoveable: public IDrawable {// IMoveable is for things that can be selected, dragged, copied, pasted, deleted etc. through the UI.
public:
  virtual void MoveTo(double XLoc, double YLoc) = 0;
  virtual boolean HitsMe(double XLoc, double YLoc) = 0;// click detection
  virtual double GetX() = 0;
  virtual double GetY() = 0;
  virtual void SetSelected(boolean Selected) = 0;
  virtual IMoveable* Clone_Me() = 0;
  virtual IMoveable* Deep_Clone_Me(CollisionLibrary& HitTable) = 0;
};

#endif
