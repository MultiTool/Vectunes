#ifndef Line2D_hpp
#define Line2D_hpp

#include "Globals.hpp"
#include "IDeletable.hpp"
#include "Point2D.hpp"

/* ********************************************************************************* */
class Line2D {
private:
  Point2D pnt[2];
  Point2D delta;// delta X, delta Y
public:
  Line2D() {}
  Line2D(Point2D& p0, Point2D& p1) {
    this->Assign(p0, p1);
  }
  void Assign(ldouble X0, ldouble Y0, ldouble X1, ldouble Y1) {
    this->pnt[0].Assign(X0, Y0); this->pnt[1].Assign(X1, Y1);
    this->CompDelta();
  }
  void Assign(const Point2D& p0, const Point2D& p1) {
    this->pnt[0].Assign(p0); this->pnt[1].Assign(p1);
    this->CompDelta();
  }
  void CompDelta() {
    this->delta.Assign(this->pnt[1].x - this->pnt[0].x, this->pnt[1].y - this->pnt[0].y);
  }
  void FractAlong(ldouble Fract, Point2D& results) {
    ldouble dx = this->delta.x * Fract;
    ldouble dy = this->delta.y * Fract;
    results.x = this->pnt[0].x + dx;
    results.y = this->pnt[0].y + dy;
    //System.out.println("Fract:" + Fract + " dx:" + this->delta.x + " dy:" + this->delta.y);
    //System.out.println("Fract:" + Fract + " dx:" + dx + " dy:" + dy);
  }
};

#endif // Line2D_hpp
