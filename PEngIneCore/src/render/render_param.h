/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_PARAM_H
#define RENDER_PARAM_H

#include <interfaces/render_target_interface.h>

#include <render/color.h>
#include <render/rectangle.h>

#include <math/vector4f.h>
#include <math/matrix.h>

#include <boost/shared_ptr.hpp>

namespace pei
{

// TODO: Use matrix instead!!!

class RenderParam;
typedef boost::shared_ptr<RenderParam> RenderParamPtr;

class RenderParam
{
public:
    enum enRenderPass
    {
        DEFAULT  = 0,
        SHADOW   = 1,
        LIGHTING = 2
    };
protected:
    union {
        // will never be a matrix - but will be use to mul into matrix
        struct {
            float      m_XOrigin,
                       m_YOrigin,
                       m_ZOrigin;  //!< Origin, normally 0,0,0 - use to e.g. change rotation center, etc
            float      pad0;

            float      m_XScale,    //!< size to render
                       m_YScale,
                       m_ZScale;
            float      pad1;

            float      m_XRotation,
                       m_YRotation,
                       m_ZRotation;
            float      pad2;
            // Rearrange to fit matrix layout
            float      m_X,         //!< x position to draw
                       m_Y,         //!< y position to draw
                       m_Z;         //!< z order
            float      pad3;
        };
        //
        struct {
            float m_Origin[4];
            float m_Scale[4];
            float m_Rotation[4];
            float m_Position[4];
        };
    };

    // 3D Volume
    float m_Width,     //!< cached size
          m_Height,    //!<
          m_Depth;     //!< depth (z width) of object - 0 for 2D

    Color m_Color;    //!< transparency in range 0..1
    float m_Volume;   //!< sound specifics - we also render sound, not just vertices

    int   m_RenderPass; //!< render pass index. Default 0

    Matrix4x4 m_MatrixRef; //!< current matrix for this render param - can be NULL! Ref to current modelview matrix
public:
	RenderParam();
	RenderParam( const RenderParam& other );
	RenderParam( const Rectangle& rect, const Color& col = Color(1.0,1.0,1.0,1.0) );
	virtual ~RenderParam() {}

	// Warning! This is not a const! You CAN(!) poke around this matrix
	inline void SetModelView( const Matrix4x4& m ) { m_MatrixRef = m;    }
    inline Matrix4x4& GetModelView( )              { return m_MatrixRef; }
    inline const Matrix4x4& GetModelView( ) const  { return m_MatrixRef; }

	inline RenderParam& SetX( float x ) { m_X = x; return *this; }
	inline RenderParam& SetY( float y ) { m_Y = y; return *this; }
	inline RenderParam& SetZ( float z ) { m_Z = z; return *this; }
	inline RenderParam& SetPosition( const Vector4f& pos ) { memcpy(m_Position,pos.asArray(),4); return *this; }

	inline float GetX() const { return m_X; }
	inline float GetY() const { return m_Y; }
	inline float GetZ() const { return m_Z; }
	inline Vector4f GetPosition() const { return m_Position; }

	inline RenderParam& SetWidth ( float w ) { m_Width  = w; return *this; }
	inline RenderParam& SetHeight( float h ) { m_Height = h; return *this; }
	inline RenderParam& SetDepth( float d ) { m_Depth = d; return *this; }

	inline float GetWidth ( ) const { return m_Width; }
	inline float GetHeight( ) const { return m_Height; }
    inline float GetDepth( ) const { return m_Depth; }

	inline RenderParam& SetXScale( float xs ) { m_XScale = xs; return *this; }
	inline RenderParam& SetYScale( float ys ) { m_YScale = ys; return *this; }
	inline RenderParam& SetZScale( float zs ) { m_ZScale = zs; return *this; }
    inline RenderParam& SetScale( const Vector4f& scale ) { memcpy(m_Scale,scale.asArray(),4); return *this; }

	inline float GetXScale( ) const { return m_XScale; }
	inline float GetYScale( ) const { return m_YScale; }
	inline float GetZScale( ) const { return m_ZScale; }
    inline Vector4f GetScale() const { return m_Scale; }

    inline RenderParam& SetXRotation( float xr ) { m_XRotation = xr; return *this; }
    inline RenderParam& SetYRotation( float yr ) { m_YRotation = yr; return *this; }
    inline RenderParam& SetZRotation( float zr ) { m_ZRotation = zr; return *this; }
    inline RenderParam& SetRotation( const Vector4f& rot ) { memcpy(m_Rotation,rot.asArray(),4); return *this; }

    inline float GetXRotation( ) const { return m_XRotation; }
    inline float GetYRotation( ) const { return m_YRotation; }
    inline float GetZRotation( ) const { return m_ZRotation; }
    inline Vector4f GetRotation() const { return m_Rotation; }

    inline RenderParam& SetXOrigin( float x ) { m_XOrigin = x; return *this; }
    inline RenderParam& SetYOrigin( float y ) { m_YOrigin = y; return *this; }
    inline RenderParam& SetZOrigin( float z ) { m_ZOrigin = z; return *this; }
    inline RenderParam& SetOrigin( const Vector4f& origin) { memcpy(m_Origin,origin.asArray(),4); return *this; }

    inline float GetXOrigin() const { return m_XOrigin; }
    inline float GetYOrigin() const { return m_YOrigin; }
    inline float GetZOrigin() const { return m_ZOrigin; }
    inline Vector4f GetOrigin() const { return m_Origin; }

    inline RenderParam& SetAlpha( float a ) { m_Color.a = (unsigned char)(a*255.0); return *this; }
	inline float GetAlpha() const { return ((float)m_Color.a)/255.0; }

	inline RenderParam& SetColor( const Color& col ) { m_Color = col; return *this; }
	inline const Color& GetColor() const { return m_Color; }

    inline RenderParam& SetVolume( float volume ) { m_Volume = volume; return *this; }
    inline float GetVolume() const { return m_Volume; }

	inline RenderParam& SetRectangle( const Rectangle &rect ) { m_X = rect.x(); m_Y = rect.y(); m_Width = rect.w(); m_Height = rect.h(); return *this; }
	inline Rectangle GetRectangle() const { return Rectangle( m_X, m_Y, m_Width, m_Height); }

	inline void SetRenderPass( int renderPass ) { m_RenderPass = renderPass; }
	inline int GetRenderPass( ) const { return m_RenderPass; }

	virtual RenderParam& Add( const RenderParam& o ) {
		m_X += o.m_X; m_Y += o.m_Y; m_Z += o.m_Z;
		m_XRotation += o.m_XRotation; m_YRotation += o.m_YRotation; m_ZRotation += o.m_ZRotation;
		return *this;
	}
	virtual RenderParam  Add( const RenderParam& o ) const { RenderParam p(*this); return p.Add(o); }

	virtual RenderParam& Sub( const RenderParam& o ) {
		m_X -= o.m_X; m_Y -= o.m_Y; m_Z -= o.m_Z;
		m_XRotation -= o.m_XRotation; m_YRotation -= o.m_YRotation; m_ZRotation -= o.m_ZRotation;
		return *this;
	}
	virtual RenderParam  Sub( const RenderParam& o ) const { RenderParam p(*this); return p.Sub(o); }

	virtual RenderParam& Mul( const RenderParam& o ) {
		m_XScale *= o.m_XScale; m_YScale *= o.m_YScale; m_ZScale = o.m_ZScale;
		return *this;
	}
	virtual RenderParam  Mul( const RenderParam& o ) const { RenderParam p(*this); return p.Mul(o); }

	RenderParam operator+( const RenderParam& p ) const  { return Add(p); }
	RenderParam& operator+=( const RenderParam& p ) { return Add(p); }

	RenderParam operator-( const RenderParam& p ) const  { return Sub(p); }
	RenderParam& operator-=( const RenderParam& p ) { return Sub(p); }

	RenderParam operator*( const RenderParam& p ) const { return Mul(p); }
	RenderParam& operator*=( const RenderParam& p ) { return Mul(p); }

	bool operator==(const RenderParam& o) const {
		// do integer comparison
		return (int)(m_X - o.m_X) == 0 &&
			   (int)(m_Y - o.m_Y) == 0 &&
			   (int)(m_Z - o.m_Z) == 0 &&
			   (int)(m_XScale - o.m_XScale) == 0 &&
			   (int)(m_YScale - o.m_YScale) == 0 &&
			   (int)(m_ZScale - o.m_ZScale) == 0 &&
			   (int)(m_Width  - o.m_Width)  == 0 &&
			   (int)(m_Height - o.m_Height) == 0 &&
			        (m_Color == o.m_Color) ;
	}

	bool operator!=(const RenderParam& o) const {
		return !(operator==(o));
	}
};

//RenderParam operator+( const RenderParam& a, const RenderParam& b ) {
//	return a.Add( b );
//}
//
//RenderParam operator*( const RenderParam& a, const RenderParam& b ) {
//	return a.Mul( b );
//}

} /* namespace pei */
#endif /* RENDER_PARAM_H_ */
