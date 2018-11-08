#ifndef Splines_hpp
#define Splines_hpp

#include <vector>
#include "Globals.hpp"
#include "CajaDelimitadora.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"

/**
 *
 * @author MultiTool
 */
class Splines {
public:
//  static constexpr ldouble Sin90 = Math::sin(Math::PI / 2.0);// 90 degrees
//  static ldouble Cos90 = Math::cos(Math::PI / 2.0);// 90 degrees
//  static ldouble Sin270 = Math::sin(Math::PI * 3.0 / 2.0);// 270 degrees
//  static ldouble Cos270 = Math::cos(Math::PI * 3.0 / 2.0);// 270 degrees
  /* ********************************************************************************* */
  static void Cubic_Spline_Chunk(const Point2D& p0, const Point2D& p1, const Point2D& p2, const Point2D& p3, ldouble FractAlong, Point2D& result) {
    // B(t) = (1-t)^3*P0  +  3*(1-t)^2*t*P1  +  3*(1-t)*t^2*P2  +  t^3*P3
    ldouble MinusFract = 1.0 - FractAlong;
    ldouble Wgt0 = Math::pow(MinusFract, 3);// * P0;
    ldouble Wgt1 = 3.0 * FractAlong * Math::pow(MinusFract, 2);// * P1;
    ldouble Wgt2 = 3.0 * MinusFract * Math::pow(FractAlong, 2);// * P2;
    ldouble Wgt3 = Math::pow(FractAlong, 3);// * P3;

    ldouble XLoc = (p0.x * Wgt0) + (p1.x * Wgt1) + (p2.x * Wgt2) + (p3.x * Wgt3);
    ldouble YLoc = (p0.y * Wgt0) + (p1.y * Wgt1) + (p2.y * Wgt2) + (p3.y * Wgt3);
    result.Assign(XLoc, YLoc);
  }
  /* ********************************************************************************* */
  static void Cubic_Spline_Boxes(ArrayList<OffsetBoxBase*>& raw, int NumSubLines, ArrayList<Point2D>& SplinePoints) {// Splines that only go forward in time
    ldouble CtrlPntLength = 1.0 / 2.0;
    Point2D Prev(0, 0), Next(0, 0);
    Point2D CtrlPrev, CtrlNext;
    Point2D result;
    int len = raw.size();
    ldouble FractAlong;
    ldouble XDist;
    OffsetBoxBase *NowBox;

    int pcnt = 0;
    int rescnt = 0;
    while (pcnt < len) {
      Prev.CopyFrom(Next);// rollover
      NowBox = raw.at(pcnt);
      NowBox->CopyTo_Point2D(Next);
      XDist = Next.x - Prev.x;

      CtrlPrev.CopyFrom(Prev);
      CtrlPrev.x += (XDist * CtrlPntLength);

      CtrlNext.CopyFrom(Next);
      CtrlNext.x -= (XDist * CtrlPntLength);

      for (int subdex = 0; subdex < NumSubLines; subdex++) {
        FractAlong = ((ldouble) subdex) / (ldouble) NumSubLines;
        Cubic_Spline_Chunk(Prev, CtrlPrev, CtrlNext, Next, FractAlong, result);
        SplinePoints[rescnt].CopyFrom(result);
        rescnt++;
      }
      pcnt++;
    }
    SplinePoints[rescnt].CopyFrom(Next);// pin last segment to final point
  }
};
/*
 real cubic spline:
 B(t) = (1-t)^3*P0  +  3*(1-t)^2*P1  +  3*(1-t)*t^2*P2  +  t^3*P3

 quadratic:
 B(t) = (1-t)^2*P0  +  2*(1-t)*t*P1  +  t^2*P2

 */


#endif
