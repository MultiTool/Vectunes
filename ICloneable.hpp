#ifndef ICloneable_hpp
#define ICloneable_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"

/**
 *
 * @author MultiTool
 */
class CollisionLibrary;// forward
class ICloneable {
public:
  virtual ICloneable* Clone_Me() = 0;
  virtual ICloneable* Deep_Clone_Me(CollisionLibrary& HitTable) = 0;
};

#endif
