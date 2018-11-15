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

#include "ArrayList.hpp"

#if false
 #define ldouble long double
#else
 #define ldouble double
#endif // false

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
//#define ArrayList std::vector
//#define HashMap std::unordered_map
#define HashMap std::map

#define Math_min(a, b) (std::min(a, b))
#define Math_max(a, b) (std::max(a, b))
#define Math_ceil(a) (std::ceil(a))
#define Math_abs(a) (std::abs(a))
#define Math_hypot(a, b) (std::hypot(a, b))

#define Double_POSITIVE_INFINITY std::numeric_limits<ldouble>::max()
#define Double_NEGATIVE_INFINITY std::numeric_limits<ldouble>::min()
#define Integer_MIN_VALUE INT_MIN
#define Integer_MAX_VALUE INT_MAX

#define PIdef 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270L
#define PI2def (PIdef*2.0) //M_PI_2;

/**
 *
 * @author MultiTool
 */

class Color {
public:
  Color() {};
  Color(ldouble R, ldouble G, ldouble B) {};
  Color(ldouble R, ldouble G, ldouble B, ldouble A) {};
  ldouble getRed() { return 0.0; };
  ldouble getGreen() { return 0.0; };
  ldouble getBlue() { return 0.0; };
};

class Double {// more simulated Java
public:
  static constexpr double POSITIVE_INFINITY = std::numeric_limits<double>::max();
  static constexpr double NEGATIVE_INFINITY = std::numeric_limits<double>::min();
};

class Integer {// more simulated Java
public:
  static constexpr int MAX_VALUE = std::numeric_limits<int>::max();
  static constexpr int MIN_VALUE = std::numeric_limits<int>::min();
};

class Globals {
public:
  const static int SampleRate = 44100;
  //const int SampleRate = 44100;
  const static int SampleRateTest = 100;
  static constexpr ldouble BaseFreqC0 = 16.3516;// hz
  static constexpr ldouble BaseFreqA0 = 27.5000;// hz
  //static constexpr ldouble BaseFreqC0;// hz
  //const constexpr BaseFreqC0 = 16.3516;// hz
  //static ldouble BaseFreqA0;// = 27.5000;// hz
  //constexpr static ldouble usPerSec = 1000000.0;
  // http://stackoverflow.com/questions/2777541/static-const-ldouble-in-c
  static ldouble BaseFreqA0_G() {
    return 27.5000;  // hz
  }
  static ldouble BaseFreqC0_G() {
    return 16.3516;  // hz
  }

  static constexpr long double Math_PI = PIdef;
  static constexpr long double TwoPi = PI2def; //M_PI_2; // hz
  static constexpr ldouble Fudge = 0.00000000001;
  class Random {
  public:
    ldouble NextDouble() {
      return ((ldouble)rand()) / (ldouble)RAND_MAX;
    }
  };
  const static Random RandomGenerator();
  //* ********************************************************************************* *
  static Color* ToAlpha(Color& col, int Alpha) {
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
  static constexpr long double PI = PIdef;

  static ldouble min(ldouble a, ldouble b) {
    return std::min(a, b);
  }
  static ldouble max(ldouble a, ldouble b) {
    return std::max(a, b);
  }
  static ldouble ceil(ldouble a) {
    return std::ceil(a);
  }
  static ldouble floor(ldouble a) {
    return std::floor(a);
  }
  static ldouble round(ldouble a) {
    return std::round(a);
  }
  static ldouble abs(ldouble a) {
    return std::abs(a);
  }
  static ldouble hypot(ldouble a, ldouble b) {
    return std::hypot(a, b);
  }
  static ldouble sqrt(ldouble a) {
    return std::sqrt(a);
  }
  static ldouble sin(ldouble a) {
    return std::sin(a);
  }
  static ldouble cos(ldouble a) {
    return std::cos(a);
  }
  static ldouble log(ldouble a) {
    return std::log(a);
  }
  static ldouble pow(ldouble a, ldouble exp) {
    return std::pow(a, exp);
  }
  //  std::srand(std::time(nullptr)); // use current time as seed for random generator
  static ldouble frand() {
    int numer = std::rand();
    int denom = 9973;//117;// prime number
    int remainder = numer % denom;
    return ((ldouble)remainder) / (ldouble)denom;
    //return ((ldouble)std::rand()) / (ldouble)RAND_MAX;
  }
};

struct Example {
  static ldouble usPerSec() {
    return 1000000.0;
  }
};

#endif
