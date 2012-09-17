/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <interfaces/render_target_interface.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace pei
{
	class Surface;
	typedef boost::shared_ptr<Surface> SurfacePtr;

	class RenderProfile;
	typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

	class RenderParam;

	class RenderTarget : public RenderTargetInterface
	{
	    int    m_RenderMode;
	    double m_LineSize;
	public:
		RenderTarget();
		virtual ~RenderTarget();

		virtual RenderTargetInterfacePtr Create() const {
			return pei::RenderTargetInterfacePtr( new RenderTarget() );
		}

        virtual void SetRenderMode( int renderMode) { m_RenderMode = renderMode; }

        virtual int GetRenderMode() const { return m_RenderMode; }

        virtual SurfacePtr SetSurface( const SurfacePtr& s, int idx = 0 );

		virtual SurfacePtr GetSurface( int idx = 0 ) const;

		/*!
		 * Render into a buffer - if available, otherwise render to screen
		 * @param buffer - ref to buffer. Must be fast, no ref counter!
		 * @param param - render param to specify how to render
		 */
		virtual void Render( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );

		// remove this!
        virtual void SetLineSize( double size = 1.0 ) { m_LineSize = size; }

        virtual double GetLineSize( ) const { return m_LineSize; }

	protected:
        virtual void PreRender( RenderProfilePtr& profile, const RenderParam& param );

        virtual void PostRender( RenderProfilePtr& profile, const RenderParam& param );
	};

	class BufferedRenderTarget : public RenderTarget
	{
	protected:
		std::vector<SurfacePtr> m_Surface;
	public:
		BufferedRenderTarget();
		virtual ~BufferedRenderTarget() {}

		virtual RenderTargetInterfacePtr Create() const {
			return pei::RenderTargetInterfacePtr( new BufferedRenderTarget() );
		}

		virtual SurfacePtr SetSurface( const SurfacePtr& s, int idx = 0 );

		virtual SurfacePtr GetSurface( int idx = 0 ) const;

        virtual void Render( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );
    };

} /* namespace pei */
#endif /* RENDER_TARGET_H */
