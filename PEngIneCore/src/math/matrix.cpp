/************************************************************************/
/* pei::Engine (©)Copyright 2009-12 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "matrix.h"

// Syntax check only

#if 0


#include <math.h>

#include "matrix.h"
#include "defs.h"

//#include "xmmintrin.h"

Matrix4v& Matrix4v::Zero( )
{
	return Set(0,0,0,0,
			   0,0,0,0,
		       0,0,0,0,
		       0,0,0,0);
}

Matrix4v& Matrix4v::Identity()
{
	return Set(1,0,0,0,
		       0,1,0,0,
		       0,0,1,0,
		       0,0,0,1);
}

Matrix4v& Matrix4v::Transpose( )
{
	// see _MM_TRANSPOSE4_PS(row0, row1, row2, row3) - needs alligned data!
	return Set(data.m00,data.m10,data.m20,data.m30,
		       data.m01,data.m11,data.m21,data.m31,
		       data.m02,data.m12,data.m22,data.m32,
		       data.m03,data.m13,data.m23,data.m33);

}
Matrix4v& Matrix4v::Add( const Matrix4v &m2)
{
#if _MSC_VER > 1000
	const Matrix4v *p = &m2;
	__asm {
		mov  eax, dword ptr [this]
		mov  ebx, dword ptr [p]

		movups xmm0, [ eax ]
		movups xmm1, [ ebx ]
		addps  xmm0, xmm1
		movups [ eax ], xmm0

		movups xmm2, [ eax + 0x10 ]
		movups xmm3, [ ebx + 0x10 ]
		addps  xmm2, xmm3
		movups [ eax + 0x10 ], xmm2

		movups xmm4, [ eax + 0x20 ]
		movups xmm5, [ ebx + 0x20 ]
		addps  xmm4, xmm5
		movups [ eax + 0x20 ], xmm4

		movups xmm6, [ eax + 0x30 ]
		movups xmm7, [ ebx + 0x30 ]
		addps  xmm6, xmm7
		movups [ eax + 0x30 ], xmm6
	}
#else
	int i,j;

	for(i = 0;i < 4;i++) {
  #ifdef __SSE__
		__m128 _xmm0;

	    _xmm0 = _mm_add_ps( _mm_loadu_ps( data.m[i] ), _mm_loadu_ps( m2.data.m[i] ) );
	    _mm_storeu_ps( data.m[i], _xmm0 );
  #else
		for(j = 0;j < 4;j++) {
			data.m[i][j] = data.m[i][j] + m2.data.m[i][j];
		}
  #endif // __SSE__
	}
#endif // _MSC_VER
	return *this;
}

Matrix4v& Matrix4v::Sub( const Matrix4v &m2)
{
#if _MSC_VER > 1000
	const Matrix4v *p = &m2;
	__asm {
		mov  eax, dword ptr [this]
		mov  ebx, dword ptr [p]

		movups xmm0, [ eax ]
		movups xmm1, [ ebx ]
		subps  xmm0, xmm1
		movups [ eax ], xmm0

		movups xmm2, [ eax + 0x10 ]
		movups xmm3, [ ebx + 0x10 ]
		subps  xmm2, xmm3
		movups [ eax + 0x10 ], xmm2

		movups xmm4, [ eax + 0x20 ]
		movups xmm5, [ ebx + 0x20 ]
		subps  xmm4, xmm5
		movups [ eax + 0x20 ], xmm4

		movups xmm6, [ eax + 0x30 ]
		movups xmm7, [ ebx + 0x30 ]
		subps  xmm6, xmm7
		movups [ eax + 0x30 ], xmm6
	}
#else

	int i,j;

	for(i = 0;i < 4;i++) {
#ifdef __SSE__
		__m128 _xmm0;

	    _xmm0 = _mm_sub_ps( _mm_loadu_ps( data.m[i] ), _mm_loadu_ps( m2.data.m[i] ) );
	    _mm_storeu_ps( data.m[i], _xmm0 );
#else
		for(j = 0;j < 4;j++)
			data.m[i][j] = data.m[i][j] - m2.data.m[i][j];
#endif
	}
#endif
	return *this;
}

Matrix4v& Matrix4v::Mul( const Matrix4v &m2 )
{
#if _MSC_VER > 1000
	const Matrix4v::MatrixData4x4 *m = &m2.data;
	__asm {

		mov	eax, dword ptr [ this ]
		mov ebx, dword ptr [ m ]

		mov   edx, 4
	_loop0:
		dec  edx

		movss  xmm0, [ eax ]
		shufps xmm0, xmm0, 0
		movups xmm1, [ ebx ];
		mulps  xmm1, xmm0

		movss  xmm0, [ eax + 4 ]
		shufps xmm0, xmm0, 0
		movups xmm2, [ ebx + 0x10 ];
		mulps  xmm2, xmm0

		movss  xmm0, [ eax + 8 ]
		shufps xmm0, xmm0, 0
		movups xmm3, [ ebx + 0x20];
		mulps  xmm3, xmm0

		movss  xmm0, [ eax + 12 ]
		shufps xmm0, xmm0, 0
		movups xmm4, [ ebx + 0x30];
		mulps  xmm0, xmm4

		addps  xmm0, xmm3
		addps  xmm0, xmm2
		addps  xmm0, xmm1

		movups [ eax ], xmm0

		add  eax, 0x10
		cmp  edx, 0
		jne _loop0
	}
#else
	int	i;

	const Matrix4v::MatrixData4x4 &mat = m2.data;
	for(i = 0;i < 4;i++)
	{
#ifdef __SSE__
		__m128 _xmm0 ATTRIBUTE((__aligned(16)));
		__m128 _xmm1 ATTRIBUTE((__aligned(16)));
		__m128 _xmm2 ATTRIBUTE((__aligned(16)));
		__m128 _xmm3 ATTRIBUTE((__aligned(16)));
		__m128 _xmm4 ATTRIBUTE((__aligned(16)));

		// load one element from vector
	    _xmm0 = _mm_load1_ps( &data.m[i][0] );
	    // load first line of matrix
	    _xmm1 = _mm_loadu_ps( mat.m[0] );
	    // multiple element with line
	    _xmm1 = _mm_mul_ps( _xmm0, _xmm1 );
	    // again, line2
	    _xmm0 = _mm_load1_ps( &data.m[i][1] );
	    _xmm2 = _mm_loadu_ps( mat.m[1] );
	    _xmm2 = _mm_mul_ps( _xmm0, _xmm2 );
	    // again, line 3
	    _xmm0 = _mm_load1_ps( &data.m[i][2] );
	    _xmm3 = _mm_loadu_ps( mat.m[2] );
	    _xmm3 = _mm_mul_ps( _xmm0, _xmm3 );
	    // line 4. plain add, no mult
	    _xmm0 = _mm_load1_ps( &data.m[i][3] );
	    _xmm4 = _mm_loadu_ps( mat.m[3] );
	    _xmm4 = _mm_mul_ps( _xmm0, _xmm4 );
	    // add the 4 components
	    _xmm0 = _mm_add_ps( _xmm1, _xmm2 );
	    _xmm0 = _mm_add_ps( _xmm0, _xmm3 );
	    _xmm0 = _mm_add_ps( _xmm0, _xmm4 );

	    _mm_storeu_ps( data.m[i], _xmm0 );
#else
		// rotate m1
		float e1 = data.m[i][0];
		float e2 = data.m[i][1];
		float e3 = data.m[i][2];
		float e4 = data.m[i][3];

		data.m[i][0] = e1 * mat.m00 + e2 * mat.m10 + e3 * mat.m20 + e4 * mat.m30;
		data.m[i][1] = e1 * mat.m01 + e2 * mat.m11 + e3 * mat.m21 + e4 * mat.m31;
		data.m[i][2] = e1 * mat.m02 + e2 * mat.m12 + e3 * mat.m22 + e4 * mat.m32;
		data.m[i][3] = e1 * mat.m03 + e2 * mat.m13 + e3 * mat.m23 + e4 * mat.m33;
#endif
	}
#endif
	return *this;
}

Matrix4v& Matrix4v::Mul3( const Matrix4v &m2 )
{
#if _MSC_VER > 1000
	const Matrix4v::MatrixData4x4 *m = &m2.data;
	__asm {

		mov	eax, dword ptr [ this ]
		mov ebx, dword ptr [ m ]

		mov   edx, 3
    _loop0:
		dec  edx

		movss  xmm0, [ eax ]
		shufps xmm0, xmm0, 0
		movups xmm1, [ ebx ];
		mulps  xmm1, xmm0

		movss  xmm0, [ eax + 4 ]
		shufps xmm0, xmm0, 0
		movups xmm2, [ ebx + 0x10 ];
		mulps  xmm2, xmm0

		movss  xmm0, [ eax + 8 ]
		shufps xmm0, xmm0, 0
		movups xmm3, [ ebx + 0x20];
		mulps  xmm3, xmm0

		// read mat.m[3][3] and shift right
		movss xmm0, [ ebx + 0x3c ];
		shufps xmm0, xmm0, 0x3f

		addps  xmm0, xmm3
		addps  xmm0, xmm2
		addps  xmm0, xmm1

		movups [ eax ], xmm0

		add  eax, 0x10
		cmp  edx, 0
		jne _loop0

		// copy last row to dest straight
		movups xmm0, [ ebx + 0x30]
		movups [ eax ], xmm0
	}
#else
	int	i;

	const Matrix4v::MatrixData4x4 &mat = m2.data;
	for(i = 0;i < 3;i++)
	{
#ifdef __SSE__
		__m128 _xmm0 ATTRIBUTE((__aligned(16)));
		__m128 _xmm1 ATTRIBUTE((__aligned(16)));
		__m128 _xmm2 ATTRIBUTE((__aligned(16)));
		__m128 _xmm3 ATTRIBUTE((__aligned(16)));
		__m128 _xmm4 ATTRIBUTE((__aligned(16)));

		// load one element from vector
	    _xmm0 = _mm_load1_ps( &data.m[i][0] );
	    // load first line of matrix
	    _xmm1 = _mm_loadu_ps( mat.m[0] );
	    // multiple element with line
	    _xmm1 = _mm_mul_ps( _xmm0, _xmm1 );
	    // again, line2
	    _xmm0 = _mm_load1_ps( &data.m[i][1] );
	    _xmm2 = _mm_loadu_ps( mat.m[1] );
	    _xmm2 = _mm_mul_ps( _xmm0, _xmm2 );
	    // again, line 3
	    _xmm0 = _mm_load1_ps( &data.m[i][2] );
	    _xmm3 = _mm_loadu_ps( mat.m[2] );
	    _xmm3 = _mm_mul_ps( _xmm0, _xmm3 );
	    // mat.m33
	    _xmm4 = _mm_setr_ps( 0,0,0,mat.m[3][3] );

	    // add the 4 components
	    _xmm0 = _mm_add_ps( _xmm1, _xmm2 );
	    _xmm0 = _mm_add_ps( _xmm0, _xmm3 );
	    _xmm0 = _mm_add_ps( _xmm0, _xmm4 );

	    _mm_storeu_ps( data.m[i], _xmm0 );
#else
		// rotate m1
		float x = data.m[i][0];
		float y = data.m[i][1];
		float z = data.m[i][2];

		data.m[i][0] = x * mat.m00 + y * mat.m10 + z * mat.m20;
		data.m[i][1] = x * mat.m01 + y * mat.m11 + z * mat.m21;
		data.m[i][2] = x * mat.m02 + y * mat.m12 + z * mat.m22;
		data.m[i][3] = x * mat.m03 + y * mat.m13 + z * mat.m23 + mat.m33;
#endif
	}
#ifdef __SSE__
	_mm_storeu_ps( data.m[3], _mm_loadu_ps( mat.m[3] ) );
#else
	data.m[3][0] = mat.m[3][0];
	data.m[3][1] = mat.m[3][1];
	data.m[3][2] = mat.m[3][2];
	data.m[3][3] = mat.m[3][3];
#endif
#endif
	return *this;
}

Matrix4v &Matrix4v::Scale( float sx,float sy,float sz )
{
#if _MSC_VER > 1000
	__asm
	{
		mov eax, dword ptr [ this ]

		movss  xmm0, sx
		shufps xmm0, xmm0, 0
		movups xmm1, [ eax ]
		mulps  xmm1, xmm0
		movups [ eax ], xmm1

		movss  xmm2, sy
		shufps xmm2, xmm2, 0
		movups xmm3, [ eax + 0x10 ]
		mulps  xmm3, xmm2
		movups [ eax + 0x10 ], xmm3

		movss  xmm4, sy
		shufps xmm4, xmm4, 0
		movups xmm5, [ eax + 0x20 ]
		mulps  xmm5, xmm4
		movups [ eax + 0x20 ], xmm5
	}
#else
#ifdef __SSE__
	__m128 _xmm0 ATTRIBUTE((__aligned(16)));
	__m128 _xmm1 ATTRIBUTE((__aligned(16)));

	_xmm0 = _mm_set1_ps( sx );
	_xmm1 = _mm_loadu_ps( data.m[0] );
	_mm_storeu_ps( data.m[0], _mm_mul_ps( _xmm0, _xmm1 ) );

	_xmm0 = _mm_set1_ps( sy );
	_xmm1 = _mm_loadu_ps( data.m[1] );
	_mm_storeu_ps( data.m[1], _mm_mul_ps( _xmm0, _xmm1 ) );

	_xmm0 = _mm_set1_ps( sz );
	_xmm1 = _mm_loadu_ps( data.m[2] );
	_mm_storeu_ps( data.m[2], _mm_mul_ps( _xmm0, _xmm1 ) );

#else
	data.e[0] *= sx;   data.e[4] *= sy;   data.e[8]  *= sz;
	data.e[1] *= sx;   data.e[5] *= sy;   data.e[9]  *= sz;
	data.e[2] *= sx;   data.e[6] *= sy;   data.e[10] *= sz;
	data.e[3] *= sx;   data.e[7] *= sy;   data.e[11] *= sz;
#endif
#endif
	return *this;
}

Matrix4v & Matrix4v::Translate( float tx,float ty,float tz )
{
#ifdef __SSE__
	__m128 _xmm0 ATTRIBUTE((__aligned(16)));
	__m128 _xmm1 ATTRIBUTE((__aligned(16)));
	__m128 _xmm2;
	__m128 _xmm3;

	_xmm2 = _mm_loadu_ps( data.m[3] );

	_xmm0 = _mm_set1_ps( tx );
	_xmm1 = _mm_loadu_ps( data.m[0] );
	_xmm2 = _mm_add_ps( _xmm2, _mm_mul_ps( _xmm0, _xmm1 ) );

	_xmm0 = _mm_set1_ps( ty );
	_xmm1 = _mm_loadu_ps( data.m[1] );
	_xmm2 = _mm_add_ps( _xmm2, _mm_mul_ps( _xmm0, _xmm1 ) );

	_xmm0 = _mm_set1_ps( tz );
	_xmm1 = _mm_loadu_ps( data.m[2] );
	_xmm2 = _mm_add_ps( _xmm2, _mm_mul_ps( _xmm0, _xmm1 ) );

	_mm_storeu_ps( data.m[3], _xmm2 );
#else
#if _MSC_VER > 1000
	__asm
	{
		mov      eax, dword ptr [this + 0]

		movups   xmm3, [ eax + 0x30 ]

		movss	 xmm0, tx
	    shufps   xmm0, xmm0, 0x0
		movups   xmm1, [ eax ]
		mulps    xmm1, xmm0
		addps    xmm3, xmm1

		movss	 xmm0, ty
		shufps   xmm0, xmm0, 0x0
		movups   xmm1, [ eax + 0x10 ]
		mulps	 xmm1, xmm0
		addps    xmm3, xmm1

		movss	 xmm0, tz
		shufps   xmm0, xmm0, 0x0
		movups   xmm1, [ eax + 0x20 ]
		mulps	 xmm1, xmm0
		addps    xmm3, xmm1

		movups   [ eax + 0x30 ], xmm3
	}
#else
	data.e[12] = data.e[0] * tx + data.e[4] * ty + data.e[8]  * tz + data.e[12];
	data.e[13] = data.e[1] * tx + data.e[5] * ty + data.e[9]  * tz + data.e[13];
	data.e[14] = data.e[2] * tx + data.e[6] * ty + data.e[10] * tz + data.e[14];
	data.e[15] = data.e[3] * tx + data.e[7] * ty + data.e[11] * tz + data.e[15];
#endif

#endif
	return *this;
}

Matrix4v &Matrix4v::Rotate( float rx, float ry, float rz )
{
	if ( rx == 0 ) {
		if ( ry == 0 ) {
			if ( rz != 0 ) {
				return RotateZ( rz );
			}
		} else if ( rz == 0 ) {
			return RotateY( ry );
		}
	} else if ( ry == 0 ) {
		if ( rz == 0 ) {
			return RotateX( rx );
		}
	}

	double x_rad = rx * RAD;
	double y_rad = ry * RAD;
	double z_rad = rz * RAD;

	float sx = (float)sin( x_rad );
	float cx = (float)cos( x_rad );
	float sy = (float)sin( y_rad );
	float cy = (float)cos( y_rad );
	float sz = (float)sin( z_rad );
	float cz = (float)cos( z_rad );

	Matrix4v rot ((cy * cz - sy * sx * sz), (cy * sz + sy * sx * cz), (-sy * cx),0.0f,
				   (cx * -sz),               (cx * cz),                sx,        0.0f,
				   (sy * cz + cy * sx * sz), (sy * sz - cy * sx * cz), (cy * cx), 0.0f,
				   0.0f,                      0.0f,                     0.0f,     1.0f);

	*this = rot.Mul3( *this );
	return *this;
}

Matrix4v &Matrix4v::RotateZ( float angle )
{
	double alpha = angle * RAD;
	float sina = (float)sin( alpha );
	float cosa = (float)cos( alpha );

	Matrix4v rot (  cosa,  sina,   0,   0,
				    -sina,  cosa,   0,   0,
				        0,     0,   1,   0,
				        0,     0,   0,   1);
	*this = rot.Mul3( *this );

	return *this;
}

Matrix4v &Matrix4v::RotateX( float angle )
{
	double alpha = angle * RAD;
	float sina = (float)sin( alpha );
	float cosa = (float)cos( alpha );

	Matrix4v rot ( 1,     0,     0,   0,
			        0,  cosa,  sina,   0,
				    0, -sina,  cosa,   0,
				    0,     0,     0,   1 );
	*this = rot.Mul3( *this );

	return *this;
}

Matrix4v &Matrix4v::RotateY( float angle )
{
	double alpha = angle * RAD;
	float sina = (float)sin( alpha );
	float cosa = (float)cos( alpha );

	Matrix4v rot ( cosa,  0, -sina,   0,
			           0,  1,     0,   0,
				    sina,  0,  cosa,   0,
				    0,     0,     0,   1 );
	*this = rot.Mul3( *this );

	return *this;
}

#endif
