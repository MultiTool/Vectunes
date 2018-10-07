#ifndef IGrabber_hpp
#define IGrabber_hpp

#include "Globals.hpp"
#include "IDrawable.hpp"

/**
 *
 * @author MultiTool

 IGrabber is basically a spatial query which also carries back all the results data.
 *
 */
class IGrabber {
public:
  /* ********************************************************************************* */
  virtual void ConsiderLeaf(IMoveable& CandidateLeaf) = 0;
  /* ********************************************************************************* */
  virtual boolean KeepDigging(IMoveable& Candidate) = 0;
  /* ********************************************************************************* */
  virtual void AddBoxToStack(IOffsetBox& child) = 0;
  virtual void DecrementStack() = 0;
  /* ********************************************************************************* */
  virtual bool Intersects(CajaDelimitadora& OtherBounds) = 0;
};

#endif // IGrabber_hpp
