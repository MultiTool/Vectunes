#ifndef DrawingContext_hpp
#define DrawingContext_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "IDeletable.hpp"
#include "CajaDelimitadora.hpp"
#include "OffsetBoxBase.hpp"
/**
 *
 * @author MultiTool
 */
// Every IDrawable has a bounding box, and every IDrawingContext also has a bounding box for clipping.
// Drawing will always be called from the top, and the bounding box will define what to draw.
class OffsetBoxBase;// forward
/* ********************************************************************************* */
class IDrawingContext: public IDeletable {
private:
  ldouble Excitement;// to highlight animation, range 0 to 1.
public:
  //public Graphics2D gr;
  CajaDelimitadora ClipBounds;
  OffsetBoxBase *Offset, *GlobalOffset;// Global Offset is transformation to and from pixels
  int RecurseDepth;
  /* ********************************************************************************* */
  // IDrawingContext() = 0;
  /* ********************************************************************************* */
  // IDrawingContext(IDrawingContext& Fresh_Parent, OffsetBox& Fresh_Transform) = 0;
  /* ********************************************************************************* */
  virtual Point2D* To_Screen(ldouble XLoc, ldouble YLoc) = 0;
  /* ********************************************************************************* */
  virtual void Compound(OffsetBoxBase& other) = 0;
  /* ********************************************************************************* */
  virtual boolean Create_Me() = 0;
  virtual void Delete_Me() = 0;
};

#endif
