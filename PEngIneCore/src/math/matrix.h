/************************************************************************/
/* pei::Engine (©)Copyright 2009-12 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <math/math.h>

#include <string.h>
#include <math.h>

namespace pei {

    class Matrix4x4
    {
    public:
        float m[16]; // 13/14/15 -> x/y/z

        inline Matrix4x4()
        {
            LoadIdentity();
        }

        inline Matrix4x4( float ma[16] )
        {
            memcpy( m, ma, sizeof(m) );
        }

        inline Matrix4x4( const Matrix4x4& matrix )
        {
            memcpy( m, matrix.m, sizeof(m) );
        }

        // dangerous - but efficient
        const float* asArray() const { return m; }

        float* asArray() { return m; }

        inline Matrix4x4& LoadIdentity( )
        {
            m[0] = 1; m[4] = 0; m[8]  = 0; m[12] = 0;
            m[1] = 0; m[5] = 1; m[9]  = 0; m[13] = 0;
            m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
            m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
            return *this;
        }

        inline Matrix4x4& Set( float m0,float m4,float m8, float m12,
                               float m1,float m5,float m9, float m13,
                               float m2,float m6,float m10,float m14,
                               float m3,float m7,float m11,float m15)
        {
            m[0] = m0; m[4] = m4; m[8]  = m8;  m[12] = m12;
            m[1] = m1; m[5] = m5; m[9]  = m9;  m[13] = m13;
            m[2] = m2; m[6] = m6; m[10] = m10; m[14] = m14;
            m[3] = m3; m[7] = m7; m[11] = m11; m[15] = m15;
            return *this;
        }

        inline Matrix4x4& Add( const float mat[16] )
        {
            for(int i = 0;i < 4;i++) {
                for(int j = 0;j < 4;j++) {
                    m[i + j * 4] += mat[i + j * 4];
                }
            }
            return *this;
        }

        inline Matrix4x4& Sub( const float mat[16] )
        {
            for(int i = 0;i < 4;i++) {
                for(int j = 0;j < 4;j++) {
                    m[i + j * 4] -= mat[i + j * 4];
                }
            }
            return *this;
        }

        inline Matrix4x4& Mul( const float mat[16] )
        {
            for (int i = 0; i < 4; i++)
            {
                // rotate m1
                float a = m[ i * 4 + 0 ];
                float b = m[ i * 4 + 1 ];
                float c = m[ i * 4 + 2 ];
                float d = m[ i * 4 + 3 ];

                m[i * 4 + 0] = a * mat[0] + b * mat[4] + c * mat[8]  + d*mat[12];
                m[i * 4 + 1] = a * mat[1] + b * mat[5] + c * mat[9]  + d*mat[13];
                m[i * 4 + 2] = a * mat[2] + b * mat[6] + c * mat[10] + d*mat[14];
                m[i * 4 + 3] = a * mat[3] + b * mat[7] + c * mat[11] + d*mat[15];
            }
            return *this;
        }

        inline Matrix4x4& Mul3( const float mat[16] )
        {
            for (int i = 0; i < 3; i++)
            {
                // rotate m1
                float a = m[ i * 4 + 0 ];
                float b = m[ i * 4 + 1 ];
                float c = m[ i * 4 + 2 ];

                m[i * 4 + 0] = a * mat[0] + b * mat[4] + c * mat[8];
                m[i * 4 + 1] = a * mat[1] + b * mat[5] + c * mat[9];
                m[i * 4 + 2] = a * mat[2] + b * mat[6] + c * mat[10];
//                m[i * 4 + 3] = a * mat[3] + b * mat[7] + c * mat[11] + mat[15];
            }
//            m[12] = mat[12];
//            m[13] = mat[13];
//            m[14] = mat[14];
//            m[15] = mat[15];

            return *this;
        }

        inline Matrix4x4& Transpose()
        {
            return Set( m[0],  m[1],  m[2],  m[3],
                        m[4],  m[5],  m[6],  m[7],
                        m[8],  m[9],  m[10], m[11],
                        m[12], m[13], m[14], m[15]);
        }

        inline Matrix4x4& Transpose( const Matrix4x4& mat )
        {
            return Set( mat.m[0],  mat.m[1],  mat.m[2],  mat.m[3],
                        mat.m[4],  mat.m[5],  mat.m[6],  mat.m[7],
                        mat.m[8],  mat.m[9],  mat.m[10], mat.m[11],
                        mat.m[12], mat.m[13], mat.m[14], mat.m[15]);
            return *this;
        }

        inline Matrix4x4& Translate( float vector[4] )
        {
            // OpenGL style translation
            float x = vector[0];
            float y = vector[1];
            float z = vector[2];
            float w = 1.0f; // vector[3]; // supposed to be 1

            m[12] = m[0]*x + m[4]*y + m[8] *z + m[12]*w;
            m[13] = m[1]*x + m[5]*y + m[9] *z + m[13]*w;
            m[14] = m[2]*x + m[6]*y + m[10]*z + m[14]*w;
//            m[15] = m[3]*x + m[7]*y + m[11]*z + m[15]*w; // -> 1??
            return *this;
        }

        inline Matrix4x4& RotateZ( float angle )
        {
            static const double RAD = 0.017453292519943295769236907684886f;

            double alpha = angle * RAD;
            float sina = (float)sin( alpha );
            float cosa = (float)cos( alpha );

            float  rot[] = { cosa,  sina,   0,   0,
                            -sina,  cosa,   0,   0,
                                0,     0,   1,   0,
                                0,     0,   0,   1 };
//            *this = rot.Mul3( *this );
            return Mul3( rot );
        }

        inline Matrix4x4& RotateX( float angle )
        {
            static const double RAD = 0.017453292519943295769236907684886f;

            double alpha = angle * RAD;
            float sina = (float)sin( alpha );
            float cosa = (float)cos( alpha );

            float rot [] = {1,     0,     0,   0,
                            0,  cosa,  sina,   0,
                            0, -sina,  cosa,   0,
                            0,     0,     0,   1 };
//            *this = rot.Mul3( *this );
//            return *this;
            return Mul3( rot );
        }

        inline Matrix4x4& RotateY( float angle )
        {
            static const double RAD = 0.017453292519943295769236907684886f;

            double alpha = angle * RAD;
            float sina = (float)sin( alpha );
            float cosa = (float)cos( alpha );

            float rot[] = { cosa,  0, -sina,   0,
                               0,  1,     0,   0,
                            sina,  0,  cosa,   0,
                            0,     0,     0,   1 };
//            *this = rot.Mul3( *this );
//            return *this;
            return Mul3( rot );

        }

        inline Matrix4x4& Rotate( float v[4] )
        {
            // see: http://www.sjbaker.org/steve/omniv/matrices_can_be_your_friends.html
            /* Rotation around Z axis : c = cos, s = sin
            c -s 0 0
            s c 0 0
            0 0 1 0
            0 0 0 1

            in OpenGL format (column-major order)

            c s 0 0 -s c 0 0 0 0 1 0 0 0 0 1
            */

            static const double RAD = 0.017453292519943295769236907684886f;

            if ( v[0] == 0 ) {
                if ( v[1] == 0 ) {
                    if ( v[2] != 0 ) {
                        return RotateZ( v[2] );
                    }
                } else if ( v[2] == 0 ) {
                    return RotateY( v[1] );
                }
            } else if ( v[1] == 0 ) {
                if ( v[2] == 0 ) {
                    return RotateX( v[0] );
                }
            } else {

                double x_rad = v[0] * RAD;
                double y_rad = v[1] * RAD;
                double z_rad = v[2] * RAD;

                float sx = (float)sin( x_rad );
                float cx = (float)cos( x_rad );
                float sy = (float)sin( y_rad );
                float cy = (float)cos( y_rad );
                float sz = (float)sin( z_rad );
                float cz = (float)cos( z_rad );

                float rot[]={(cy * cz - sy * sx * sz), (cy * sz + sy * sx * cz), (-sy * cx), 0.0f,
                             (cx * -sz),               (cx * cz),                sx,         0.0f,
                             (sy * cz + cy * sx * sz), (sy * sz - cy * sx * cz), (cy * cx),  0.0f,
                              0.0f,                     0.0f,                     0.0f,      1.0f };
                Mul3( rot );
//            *this = rot.Mul3( *this );
//            return *this;
            }
            return *this;
        }

        inline Matrix4x4& Scale( float s[4] )
        {
            m[0] *= s[0]; m[4] *= s[1]; m[8]  *= s[2];
            m[1] *= s[0]; m[5] *= s[1]; m[9]  *= s[2];
            m[2] *= s[0]; m[6] *= s[1]; m[10] *= s[2];
            return *this;
        }

        inline Matrix4x4& Scale( float s )
        {
            m[0] *= s; m[4] *= s; m[8]  *= s;
            m[1] *= s; m[5] *= s; m[9]  *= s;
            m[2] *= s; m[6] *= s; m[10] *= s;
            return *this;
        }

        inline Matrix4x4& operator=( float ma[16] )
        {
            memcpy( m, ma, sizeof(m) );
            return *this;
        }

        inline Matrix4x4& operator=( const Matrix4x4& matrix )
        {
            memcpy( m, matrix.m, sizeof(m) );
            return *this;
        }

        inline Matrix4x4 operator!()
        {
            return Matrix4x4(*this).Transpose();
        }

        inline float operator[](unsigned int i) const
        {
            if ( i < 16 ) {
                return m[i];
            }
            return 0;
        }
    };


    class Matrix3x3 {
    public:
        float m[9];
    };
} // namespace pei

#endif
