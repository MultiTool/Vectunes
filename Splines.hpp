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
//  static constexpr double Sin90 = Math::sin(Math::PI / 2.0);// 90 degrees
//  static double Cos90 = Math::cos(Math::PI / 2.0);// 90 degrees
//  static double Sin270 = Math::sin(Math::PI * 3.0 / 2.0);// 270 degrees
//  static double Cos270 = Math::cos(Math::PI * 3.0 / 2.0);// 270 degrees
  /* ********************************************************************************* */
  static void Cubic_Spline_Chunk(Point2D& p0, Point2D& p1, Point2D& p2, Point2D& p3, double FractAlong, Point2D& result) {
    // B(t) = (1-t)^3*P0  +  3*(1-t)^2*t*P1  +  3*(1-t)*t^2*P2  +  t^3*P3
    double MinusFract = 1.0 - FractAlong;
    double Wgt0 = Math::pow(MinusFract, 3);// * P0;
    double Wgt1 = 3.0 * FractAlong * Math::pow(MinusFract, 2);// * P1;
    double Wgt2 = 3.0 * MinusFract * Math::pow(FractAlong, 2);// * P2;
    double Wgt3 = Math::pow(FractAlong, 3);// * P3;

    double XLoc = (p0.x * Wgt0) + (p1.x * Wgt1) + (p2.x * Wgt2) + (p3.x * Wgt3);
    double YLoc = (p0.y * Wgt0) + (p1.y * Wgt1) + (p2.y * Wgt2) + (p3.y * Wgt3);
    result.Assign(XLoc, YLoc);
  }
  /* ********************************************************************************* */
  static void Cubic_Spline_Boxes(ArrayList<IOffsetBox*>& raw, int NumSubLines, ArrayList<Point2D>& SplinePoints) {// Splines that only go forward in time
    double CtrlPntLength = 1.0 / 2.0;
    Point2D Prev(0, 0), Next(0, 0);
    Point2D CtrlPrev, CtrlNext;
    Point2D result;
    int len = raw.size();
    double FractAlong;
    double XDist;
    IOffsetBox *NowBox;

    int pcnt = 0;
    int rescnt = 0;
    while (pcnt < len) {
      Prev.CopyFrom(Next);// rollover
      NowBox = raw.at(pcnt);
      NowBox->CopyTo_PointX(Next);
      XDist = Next.x - Prev.x;

      CtrlPrev.CopyFrom(Prev);
      CtrlPrev.x += (XDist * CtrlPntLength);

      CtrlNext.CopyFrom(Next);
      CtrlNext.x -= (XDist * CtrlPntLength);

      for (int subdex = 0; subdex < NumSubLines; subdex++) {
        FractAlong = ((double) subdex) / (double) NumSubLines;
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