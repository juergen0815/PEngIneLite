/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef MATH_H
#define MATH_H

namespace pei {

    static const double PI = 3.1415926535897932384626433832795f;

    static const double PI2 = 6.2831853071795864769252867665590f;

    // 0.017453292519943295769236907684886 - convert degree to radiant
    static const double RAD = 0.017453292519943295769236907684886f;

    // 57.295779513082320876798154814105 - convert radiant to degree
    static const double DEG = 57.295779513082320876798154814105f;

    static const double RAD360 = PI2;
    static const double RAD180 = PI;

    inline double toDeg( double rad ) { return rad * DEG; }

    inline double toRad( double deg ) { return deg * RAD; }

} // namespace pei

#endif /* MATH_H */
