#ifndef IDeletable_hpp
#define IDeletable_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>

/**
 *
 * @author MultiTool
 *
 * IDeletable exists to help porting to a non-garbage-collecting environment such as C++, It helps do accounting for memory leaks.
 * We will probably switch this out for a reference counting scheme. Ref counting will only be useful for objects with multiple references such as Songlets.
 *
 */
class IDeletable {
  virtual boolean Create_Me() = 0;
  virtual void Delete_Me() = 0;
//  int Ref_Me();// increment ref counter and return new value just for kicks
//  int UnRef_Me();// decrement ref counter and return new value just for kicks
};
//  Possible pattern:
//  (MyPointer = DeletableObject).Ref_Me();// ref pattern
//  MyPointer = MyPointer.UnRef_Me();// unref pattern, unref returns null?

#endif
