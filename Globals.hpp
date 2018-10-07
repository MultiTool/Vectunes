#ifndef Globals_hpp
#define Globals_hpp

// C++ stuff
#define _USE_MATH_DEFINES
#include <cmath>
#include <climits>
#include <limits>       // std::numeric_limits
#include <algorithm>    // std::min
#include <stdlib.h>     // srand, rand
#include <cstdlib> // RAND_MAX
#include <vector>
#include <map>

#define boolean bool
#define jpublic
#define jprivate
#define implements :
#define interface class
#define extends :
//#define Graphics2D void*
#define Graphics2D (void*)
#define Object void*
#define null nullptr
#define String std::string
#define ArrayList std::vector
//#define HashMap std::unordered_map
#define HashMap std::map

#define Math_min(a, b) (std::min(a, b))
#define Math_max(a, b) (std::max(a, b))
#define Math_ceil(a) (std::ceil(a))
#define Math_abs(a) (std::abs(a))
#define Math_hypot(a, b) (std::hypot(a, b))

#define Double_POSITIVE_INFINITY std::numeric_limits<double>::max()
#define Double_NEGATIVE_INFINITY std::numeric_limits<double>::min()
#define Integer_MIN_VALUE INT_MIN
#define Integer_MAX_VALUE INT_MAX

/**
 *
 * @author MultiTool
 */

class Color {
public:
  Color() {};
  Color(double R, double G, double B) {};
  Color(double R, double G, double B, double A) {};
  double getRed() { return 0.0; };
  double getGreen() { return 0.0; };
  double getBlue() { return 0.0; };
};

class Globals {
public:
  const static int SampleRate = 44100;
  //const int SampleRate = 44100;
  const static int SampleRateTest = 100;
  static constexpr double BaseFreqC0 = 16.3516;// hz
  static constexpr double BaseFreqA0 = 27.5000;// hz
  //static constexpr double BaseFreqC0;// hz
  //const constexpr BaseFreqC0 = 16.3516;// hz
  //static double BaseFreqA0;// = 27.5000;// hz
  //constexpr static double usPerSec = 1000000.0;
  // http://stackoverflow.com/questions/2777541/static-const-double-in-c
  static double BaseFreqA0_G() {
    return 27.5000;  // hz
  }
  static double BaseFreqC0_G() {
    return 16.3516;  // hz
  }

  static constexpr long double Math_PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270L;
  static constexpr long double TwoPi = Math_PI * 2; //M_PI_2; // hz
  static constexpr double Fudge = 0.00000000001;
  class Random {
  public:
    double NextDouble() {
      return ((double)rand()) / (double)RAND_MAX;
    }
  };
  const static Random RandomGenerator();// = new Random();
  //* ********************************************************************************* *
  static Color* ToAlpha(Color& col, int Alpha) {
    //return new Color();
    return new Color(col.getRed(), col.getGreen(), col.getBlue(), Alpha);// rgba
  }
  //* ********************************************************************************* *
  static Color* ToRainbow(double Fraction) {
    if (Fraction < 0.5) {
      Fraction *= 2;
      return new Color((1.0 - Fraction), Fraction, 0);
    } else {
      Fraction = Math_min((Fraction - 0.5) * 2, 1.0);
      return new Color(0, (1.0 - Fraction), Fraction);
    }
  }
};

class Math {
public:
  //static constexpr long double PI = 3.141592653589793238L;
  static constexpr long double PI = Globals::Math_PI;

  static double min(double a, double b) {
    return std::min(a, b);
  }
  static double max(double a, double b) {
    return std::max(a, b);
  }
  static double ceil(double a) {
    return std::ceil(a);
  }
  static double floor(double a) {
    return std::floor(a);
  }
  static double abs(double a) {
    return std::abs(a);
  }
  static double hypot(double a, double b) {
    return std::hypot(a, b);
  }
  static double sqrt(double a) {
    return std::sqrt(a);
  }
  static double sin(double a) {
    return std::sin(a);
  }
  static double cos(double a) {
    return std::cos(a);
  }
  static double log(double a) {
    return std::log(a);
  }
  static double pow(double a, double exp) {
    return std::pow(a, exp);
  }
  //  std::srand(std::time(nullptr)); // use current time as seed for random generator
  static double frand() {
    return ((double)std::rand()) / (double)RAND_MAX;
  }
};

struct Example {
  static double usPerSec() {
    return 1000000.0;
  }
};

#endif
