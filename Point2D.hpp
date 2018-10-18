#ifndef Point2D_hpp
#define Point2D_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "Globals.hpp"
#include "IDeletable.hpp"

/**
 *
 * @author MultiTool
 */
/* ********************************************************************** */
class Point2D;// forward
typedef Point2D *Point2DPtr;
typedef std::vector<Point2DPtr> Point2DVec;
class Point2D : public IDeletable {
public:
  double x, y;
  //Point2D* Zero;
  Point2D() {}
  Point2D(const Point2D& donor) : Point2D(donor.x, donor.y) {
  }
  Point2D(double XLoc, double YLoc) {
    this->Assign(XLoc, YLoc);
    //this->Zero = neuvo Point2D(0, 0);
  }
  ~Point2D() {
    this->Delete_Me();
  }
  void setLocation(double XLoc, double YLoc) {
    this->x = XLoc; this->y = YLoc;
  }
  void setLocation(const Point2D& pd) {
    this->x = pd.x; this->y = pd.y;
  }
  void Assign(double XLoc, double YLoc) {
    this->x = XLoc; this->y = YLoc;
  }
  void Assign(const Point2D& other) {
    this->x = other.x; this->y = other.y;
  }
  void CopyFrom(const Point2D& donor) {
    this->x = donor.x; this->y = donor.y;
  }
  void Add(const Point2D& other) {
    this->x += other.x; this->y += other.y;
  }
  void Subtract(const Point2D& other) {
    this->x -= other.x; this->y -= other.y;
  }
  void Multiply(double factor) {
    this->x *= factor; this->y *= factor;
  }
  void Normalize() {
    if (this->x != 0 || this->y != 0) {
      double magnitude = Math::sqrt((this->x * this->x) + (this->y * this->y));
      this->x /= magnitude; this->y /= magnitude;
    }
  }
  double GetMagnitude() {
    if (this->x == 0 && this->y == 0) { return 0; }
    return Math::sqrt((this->x * this->x) + (this->y * this->y));
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {
    return 0;
  }
  void Delete_Me() override {
    this->x = Double_NEGATIVE_INFINITY;
    this->y = Double_NEGATIVE_INFINITY;
    //delete this->Zero;
  }
  /* ********************************************************************************* */
};

#endif
