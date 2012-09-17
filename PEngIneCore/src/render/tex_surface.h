/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef GL_SURFACE_H
#define GL_SURFACE_H

#include <profile_config.h>

#include "render_surface.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace pei
{
	class TexSurface: public pei::Surface
	{
	public:
#ifdef _HAS_OPENGL_
        static const int REPEAT;
        static const int CLAMP;
        static const int CLAMP_EDGE;
        static const int CLAMP_BORDER;
		enum enTexType {
			EN_TEX_1D,
			EN_TEX_2D,
			EN_TEX_3D,
			EN_TEX_CUBE
		};
#else
        static const int REPEAT;
        static const int CLAMP_EDGE;
		enum enTexType {
			EN_TEX_2D,
		};
#endif

		struct TexCoords {
			double MinX, MinY;
			double MaxX, MaxY;

			TexCoords( double minx = 0.0, double miny = 0.0, double maxx = 1.0, double maxy = 1.0 )
			: MinX(minx), MinY(miny), MaxX(maxx), MaxY(maxy) {}

			TexCoords( const TexCoords& o )
			: MinX(o.MinX), MinY(o.MinY), MaxX(o.MaxX), MaxY(o.MaxY) {}
		};

	protected:
		enTexType  	    m_TexType;
		size_t 		    m_TexId;
		TexCoords 	    m_TexCoords;
		int				m_WrapMode;
		int				m_Filter;

		bool            m_IsLoaded; //!< flag if we were loaded into texture memory

		mutable boost::recursive_mutex  m_PixelLock;

        TexSurface( enTexType type );
	public:

		virtual ~TexSurface();

        virtual void SetFormat( const Format& format ) { m_Format = format; }

        virtual void Lock() const;

        virtual void Unlock() const;

        virtual void SetWrapMode( int wrapMode ) { m_WrapMode = wrapMode; }

        virtual int GetWrapMode( ) const { return m_WrapMode; }

        virtual void SetFilter( int filter ) { m_Filter = filter; }

        virtual int GetFilter() const { return m_Filter; }

		virtual enTexType GetType() const { return m_TexType; }

		virtual size_t GetId() const { return m_TexId; }

		virtual const TexCoords& GetCoords() const { return m_TexCoords; }

		virtual void Bind();

		virtual void Enable();

		virtual void Disable();

		/*!
		 * Check if the texture is already loaded into vmem
		 * @return
		 */
		virtual bool IsLoaded() const { return m_IsLoaded; }

	};

#ifdef _HAS_OPENGL_
	class Texture1DSurface : public TexSurface
	{
	public:
	    Texture1DSurface( );

		virtual ~Texture1DSurface();

	};
	typedef boost::shared_ptr<Texture1DSurface> Texture1DSurfacePtr;
#endif

	class Texture2DSurface : public TexSurface
	{
	public:
        Texture2DSurface(  );

        virtual void SetFormat( const Format& format );

//		virtual void Swap( const pei::Format& format );

		virtual ~Texture2DSurface();

        virtual void Bind();

		virtual void Enable();

		virtual void Disable();

		virtual void Lock() const;

		virtual void Unlock() const;
	};
	typedef boost::shared_ptr<Texture2DSurface> Texture2DSurfacePtr;

} /* namespace pei */
#endif /* GL_SURFACE_H_ */
