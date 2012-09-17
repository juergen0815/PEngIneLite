/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "wx_render_profile.h"

#include <render/rectangle.h>
#include <render/render_target.h>

#include <opengl/gl_target.h>

#include <video/blitter.h>
#include <video/render_tools.h>

#undef _HAS_SDL_
#undef _HAS_SDL_IMAGE_

// TODO: This needs to go
#ifdef _HAS_SDL_
#include <SDL.h>
#endif

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <boost/thread.hpp>

namespace pei {
  namespace wx {

    //////////////////////////////////////////////////////////////////////////
    // wxWidget OpenGL Profile

    RenderProfile::RenderProfile( wxGLCanvas* canvas )
    	: m_Canvas(canvas)
        , m_GLContext( NULL )
    {
        // does not run in render thread!!!
        m_Flags &= (~pei::RenderProfile::EN_RENDER_ASYNC);

		// over write default render target
		RenderTargetInterfacePtr rti( new pei::opengl::RenderTarget() );
		// set a default render target - will be over written by implementation
		SetDefaultRenderTarget( rti );
		// do not clone default. We can use this directly
		AddRenderTarget( DEFAULT_TARGET, rti );

		// we can use this here directly, too
		if ( m_Screen ) {
            // main viewport does not render - set NULL device
			m_Screen->SetRenderTarget( RenderTargetInterfacePtr( new pei::RenderTarget));
			m_Screen->SetName("wx::GL::MainViewport");
		}
        m_GLContext = new wxGLContext( m_Canvas );
    }

    RenderProfile::~RenderProfile()
    {
        if ( m_GLContext ) {
            delete m_GLContext;
            m_GLContext = NULL;
        }
    }

    bool RenderProfile::Init()
    {
        // TODO: This needs to go!
#ifdef _HAS_SDL_IMAGE_
        // I still need an SDL subsystem (SDL_ttf, SDL_image)
        if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
        {
            std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }
#endif
        return true;
    }

    bool RenderProfile::OpenScreen( size_t width, size_t height, size_t flags, const char* title )
    {
        // TODO: read capabilities and render accordingly
        if ( m_Canvas )
        {
            pei::RenderNodeInterfacePtr screen = ResizeScreen( width, height );
            if ( screen != NULL ) {
                m_GLContext->SetCurrent(*m_Canvas);

                // TODO: Drag-and-Drop support
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glEnable(GL_POINT_SMOOTH);
                glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);

                glClearColor(0.0f,0.0f,0.0f,0.0f);
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                return true;
            }
        }
        return false;
    }

    void RenderProfile::MakeCurrent()
    {
        if ( m_Canvas && m_GLContext ) {
            m_GLContext->SetCurrent( *m_Canvas );
        }
    }

    void RenderProfile::RenderFrame( pei::RenderProfilePtr& self, double ticks )
    {
        if ( m_Canvas && m_GLContext ) {
            m_Screen->Update( self, ticks, 0 );
            m_Screen->Draw( self, m_BackBuffer, LoadIdentity() );
        }
    }

    void RenderProfile::SwapBuffers()
    {
        if ( m_Canvas ) {
            m_Canvas->SwapBuffers();
        }
    }

    double RenderProfile::GetTicks()
    {
#ifdef WIN32
    	return (double)::GetTickCount();
#else
		struct timeval current;
		gettimeofday( &current, NULL );
		double s_ms = 1000.0 * (double)current.tv_sec;
		double u_ms = (double)current.tv_usec / 1000.0;
		return (s_ms + u_ms);
#endif
    }

    void RenderProfile::Delay( double t )
    {
        boost::this_thread::sleep(boost::posix_time::millisec((boost::int64_t)t));
    }



  }
}
