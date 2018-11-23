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

#if true
 #define ldouble long double
#else
 #define ldouble double
#endif // false

#if false
 #define SoundFloat long double
#else
 #define SoundFloat double
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

#define Double_POSITIVE_INFINITY std::numeric_limits<SoundFloat>::max()
#define Double_NEGATIVE_INFINITY std::numeric_limits<SoundFloat>::min()
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
  Color(SoundFloat R, SoundFloat G, SoundFloat B) {};
  Color(SoundFloat R, SoundFloat G, SoundFloat B, SoundFloat A) {};
  SoundFloat getRed() { return 0.0; };
  SoundFloat getGreen() { return 0.0; };
  SoundFloat getBlue() { return 0.0; };
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
  static constexpr SoundFloat BaseFreqC0 = 16.3516;// hz
  static constexpr SoundFloat BaseFreqA0 = 27.5000;// hz
  //static constexpr SoundFloat BaseFreqC0;// hz
  //const constexpr BaseFreqC0 = 16.3516;// hz
  //static SoundFloat BaseFreqA0;// = 27.5000;// hz
  //constexpr static SoundFloat usPerSec = 1000000.0;
  // http://stackoverflow.com/questions/2777541/static-const-SoundFloat-in-c
  static SoundFloat BaseFreqA0_G() {
    return 27.5000;  // hz
  }
  static SoundFloat BaseFreqC0_G() {
    return 16.3516;  // hz
  }

  static constexpr long double Math_PI = PIdef;
  static constexpr long double TwoPi = PI2def; //M_PI_2; // hz
  static constexpr SoundFloat Fudge = 0.00000000001;
  class Random {
  public:
    SoundFloat NextDouble() {
      return ((SoundFloat)rand()) / (SoundFloat)RAND_MAX;
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

  static SoundFloat min(SoundFloat a, SoundFloat b) {
    return std::min(a, b);
  }
  static SoundFloat max(SoundFloat a, SoundFloat b) {
    return std::max(a, b);
  }
  static SoundFloat ceil(SoundFloat a) {
    return std::ceil(a);
  }
  static SoundFloat floor(SoundFloat a) {
    return std::floor(a);
  }
  static SoundFloat round(SoundFloat a) {
    return std::round(a);
  }
  static SoundFloat abs(SoundFloat a) {
    return std::abs(a);
  }
  static SoundFloat hypot(SoundFloat a, SoundFloat b) {
    return std::hypot(a, b);
  }
  static SoundFloat sqrt(SoundFloat a) {
    return std::sqrt(a);
  }
  static SoundFloat sin(SoundFloat a) {
    return std::sin(a);
  }
  static SoundFloat cos(SoundFloat a) {
    return std::cos(a);
  }
  static SoundFloat log(SoundFloat a) {
    return std::log(a);
  }
  static SoundFloat pow(SoundFloat a, SoundFloat exp) {
    return std::pow(a, exp);
  }
  //  std::srand(std::time(nullptr)); // use current time as seed for random generator
  static SoundFloat frand() {
    int numer = std::rand();
    int denom = 9973;//117;// prime number
    int remainder = numer % denom;
    return ((SoundFloat)remainder) / (SoundFloat)denom;
    //return ((SoundFloat)std::rand()) / (SoundFloat)RAND_MAX;
  }
};

struct Example {
  static SoundFloat usPerSec() {
    return 1000000.0;
  }
};

#endif
