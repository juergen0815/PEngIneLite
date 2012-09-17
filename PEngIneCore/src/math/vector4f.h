/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef VECTOR4F_H
#define VECTOR4F_H

#include <string>
#include <vector>

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace pei
{
    class Vector4f
    {
    public:
        enum { X = 0, Y = 1, Z = 2, W = 3 };
        enum { A = 0, B = 1, C = 2, D = 3 };

    protected:
        float vector[4];
    public:
        inline std::string asString()
        {
            float x = vector[0] < 0.01f ? 0 : vector[0];
            float y = vector[1] < 0.01f ? 0 : vector[1];
            float z = vector[2] < 0.01f ? 0 : vector[2];
            float w = vector[3] < 0.01f ? 0 : vector[3];
            char buffer[256];
            sprintf( buffer, "%.6f, %.6f, %.6f, %.6f", x,y,z,w);
            return buffer;
        }

        inline std::vector<float> asFloatVector() const { return std::vector<float>(&vector[0],&vector[3]); }

        inline float* asFloat() { return &vector[0]; }

        inline Vector4f() { vector[A] = vector[B] = vector[C] = vector[D] = 0; }

        inline Vector4f( const Vector4f& vec )
        {
            vector[X] = vec.x();
            vector[Y] = vec.y();
            vector[Z] = vec.z();
            vector[W] = vec.w();
        }

        inline Vector4f( const float vec[4] ) {
            Set4f( vec );
        }

        inline Vector4f( float x, float y, float z = 0.0 ) {
            vector[X] = x;
            vector[Y] = y;
            vector[Z] = z;
            vector[W] = 1.0;
        }

        inline Vector4f( float a, float b, float c, float d ) {
            vector[A] = a;
            vector[B] = b;
            vector[C] = c;
            vector[D] = d;
        }

        inline float x() const { return vector[X]; }
        inline float y() const { return vector[Y]; }
        inline float z() const { return vector[Z]; }
        inline float w() const { return vector[W]; }

        inline void x( float v ) { vector[X] = v; }
        inline void y( float v ) { vector[Y] = v; }
        inline void z( float v ) { vector[Z] = v; }
        inline void w( float v ) { vector[W] = v; }

        // dangerous - but efficient
        const float* asArray() const { return vector; }

        inline bool equals( const Vector4f& vec ) const {
            return (fabs(vector[X] - vec.x()) < 0.01) &&
                   (fabs(vector[Y] - vec.y()) < 0.01) &&
                   (fabs(vector[Z] - vec.z()) < 0.01) &&
                   (fabs(vector[W] - vec.w()) < 0.01);
        }

        inline Vector4f& Set4f( const float vec[4] ) {
            memcpy( vector, vec, sizeof(float[4]));
            return *this;
        }

        inline Vector4f& Set3f( const float vec[3] ) {
            memcpy( vector, vec, sizeof(float[3]));
            vector[W] = 1.0;
            return *this;
        }

        inline Vector4f& Add(const Vector4f& v2) {
            vector[A] += v2.vector[A];
            vector[B] += v2.vector[B];
            vector[C] += v2.vector[C];
            vector[D] += v2.vector[D];
            return *this;
        }

        inline Vector4f& Add( float v ) {
            vector[A] += v;
            vector[B] += v;
            vector[C] += v;
            vector[D] += v;
            return *this;
        }

        static inline Vector4f Add( const Vector4f& v1, const Vector4f& v2 ) {
            Vector4f a(v1);
            return a.Add( v2 );
        }

        inline Vector4f& Sub( const Vector4f& v2) {
            vector[A] -= v2.vector[A];
            vector[B] -= v2.vector[B];
            vector[C] -= v2.vector[C];
            vector[D] -= v2.vector[D];
            return *this;
        }

        inline Vector4f& Sub( float v ) {
            vector[A] -= v;
            vector[B] -= v;
            vector[C] -= v;
            vector[D] -= v;
            return *this;
        }

        static inline Vector4f Sub( const Vector4f& v1, const Vector4f& v2 ) {
            Vector4f a(v1);
            return a.Sub( v2 );
        }

        inline Vector4f& Div( const Vector4f& v2 ) {
            vector[A] /= v2.vector[A] != 0.0f ? v2.vector[A] : 1.0f;
            vector[B] /= v2.vector[B] != 0.0f ? v2.vector[B] : 1.0f;
            vector[C] /= v2.vector[C] != 0.0f ? v2.vector[C] : 1.0f;
            vector[D] /= v2.vector[D] != 0.0f ? v2.vector[D] : 1.0f;
            return *this;
        }

        static inline Vector4f Div( const Vector4f& v1, const Vector4f& v2 ) {
            Vector4f a(v1);
            return a.Div( v2 );
        }

        inline Vector4f& Div( float s ) {
            if ( s != 0 ) {
                vector[A] /= s;
                vector[B] /= s;
                vector[C] /= s;
                vector[D] /= s;
            } else {
                vector[A] = vector[B] = vector[C] = vector[D] = 0;
            }
            return *this;
        }

        static inline Vector4f Div( const Vector4f& v , float s ) {
            float d = s == 0 ? 1.0f : s;
            return Vector4f( v.x() / d, v.y() / d, v.z() / d, v.w() / d );
        }

        inline Vector4f& Mul( const Vector4f& v2) {
            vector[A] *= v2.vector[A];
            vector[B] *= v2.vector[B];
            vector[C] *= v2.vector[C];
            vector[D] *= v2.vector[D];

            return *this;
        }

        inline Vector4f& Mul( float s ) {
            vector[A] *= s;
            vector[B] *= s;
            vector[C] *= s;
            vector[D] *= s;
            return *this;
        }

        static inline Vector4f Mul( const Vector4f& a, const Vector4f& b ) {
            return Vector4f( a.x() * b.x(), a.y() * b.y(), a.z() * b.z(), a.w() * b.w() );
        }

        static inline Vector4f Mul( const Vector4f& v, float s ) {
            return Vector4f( v.x() * s, v.y() * s, v.z() * s, v.w() * s );
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Multiply with matrix (OpenGL layout!)

        inline pei::Vector4f& Mul( float m[16] )
        {
            float a = vector[A];
            float b = vector[B];
            float c = vector[C];
            float d = vector[D];

            vector[A] = m[0]*a + m[4]*b + m[8] *c + m[12]*d;
            vector[B] = m[1]*a + m[5]*b + m[9] *c + m[13]*d;
            vector[C] = m[2]*a + m[6]*b + m[10]*c + m[14]*d;
            vector[D] = m[3]*a + m[7]*b + m[11]*c + m[15]*d;

            return *this;
        }

        static inline pei::Vector4f Mul( const pei::Vector4f& v, float m[16] )
        {
            float a = v[A];
            float b = v[B];
            float c = v[C];
            float d = v[D];

            pei::Vector4f vo( m[0]*a + m[4]*b + m[8] *c + m[12]*d,
                              m[1]*a + m[5]*b + m[9] *c + m[13]*d,
                              m[2]*a + m[6]*b + m[10]*c + m[14]*d,
                              m[3]*a + m[7]*b + m[11]*c + m[15]*d );

            return vo;
        }

        inline float Magnitude( ) {
            float x = vector[X];
            float y = vector[Y];
            float z = vector[Z];
            float w = vector[W];
            return (float)sqrt( (double)(x*x + y*y + z*z + w*w) );
        }

        inline Vector4f& Normalize( ) {
            return Div(Magnitude());
        }

        inline Vector4f Normalized( ) const
        {
            return Vector4f(*this).Normalize();
        }

        inline float Dot( const Vector4f& v ) const {
            return ( vector[A] * v.vector[A] +
                     vector[B] * v.vector[B] +
                     vector[C] * v.vector[C] +
                     vector[D] * v.vector[D] );
        }

        inline float Dot( const Vector4f& v1, const Vector4f& v2 ) {
            return Vector4f(v1).Dot( v2 );
        }

        inline Vector4f& Cross( const Vector4f& v) {
            float x = vector[Y] * v.vector[Z] - vector[Z] * v.vector[Y];
            float y = vector[Z] * v.vector[X] - vector[X] * v.vector[Z];
            float z = vector[X] * v.vector[Y] - vector[Y] * v.vector[X];

            vector[X] = x;
            vector[Y] = y;
            vector[Z] = z;
            vector[D] = 0;
            return *this;
        }

        static inline Vector4f Cross( const Vector4f& v1, const Vector4f& v2 ) {
            return Vector4f(v1).Cross(v2);
        }

        inline bool IsNull() {
            // maybe memcmp makes more sense??
            return vector[0] == 0.0f && vector[1] == 0.0f && vector[2] == 0.0f && vector[3] == 0.0f;
        }

        inline float operator[]( unsigned int n ) const {
            if (n < 4 ) return vector[n];
            return 0;
        }

        inline float& operator[]( unsigned int n ) {
            if (n < 4 ) return vector[n];
            throw std::exception();
        }

        inline bool operator==( const pei::Vector4f& vec ) const {
            return equals( vec );
        }

        inline Vector4f& operator+=( float v ) { return Add(v); }

        inline Vector4f& operator+=(const Vector4f& v2 ) { return Add(v2); }

        inline Vector4f& operator-=( float v ) { return Sub(v); }

        inline Vector4f& operator-=( const Vector4f& v2 ) { return Sub(v2); }

        inline Vector4f& operator*=( const Vector4f& v2 ) { return Mul(v2); }

        inline Vector4f& operator*=( float s ) { return Mul(s); }

        inline Vector4f& operator/=( const Vector4f& v2 ) { return Div(v2); }

        inline Vector4f& operator/=( float s ) { return Div(s); }

        inline Vector4f& operator=( float v[4] ) { return Set4f(v); }
    };

    inline pei::Vector4f operator+( const pei::Vector4f& v1, const pei::Vector4f& v2 ) {
        Vector4f a(v1);
        return a.Add( v2 );
    }

    inline pei::Vector4f operator-( const pei::Vector4f& v1, const pei::Vector4f& v2 ) {
        Vector4f a(v1);
        return a.Sub( v2 );
    }

} // namespace pei

#endif /* VECTOR4F_H */
