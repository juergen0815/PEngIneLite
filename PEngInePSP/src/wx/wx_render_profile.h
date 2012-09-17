/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef WX_RENDER_PROFILE_H
#define WX_RENDER_PROFILE_H

#include <profile_config.h>

#include <render/render_profile.h>

#include <opengl/gl_profile.h>

#include <render/render_surface.h>
#include <render/render_param.h>

#include <wx/wx.h>
#include <wx/glcanvas.h>

namespace pei {
    namespace wx {

        class RenderProfile;
        typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

        class RenderProfile : public pei::opengl::RenderProfile
        {
        public:
            wxGLCanvas      *m_Canvas;
            wxGLContext     *m_GLContext;
        	pei::SurfacePtr  m_BackBuffer;

            RenderProfile( wxGLCanvas* canvas );
        public:
            static pei::RenderProfilePtr Create( wxGLCanvas* canvas ) {
                pei::RenderProfilePtr rp( new wx::RenderProfile(canvas) );
                rp->LinkSmartPtr( rp );
                return rp;
            }

            virtual ~RenderProfile();

            virtual bool Init();

            virtual bool OpenScreen( size_t width, size_t height, size_t flags, const char* title );

            virtual void MakeCurrent();

            virtual void SwapBuffers();

            virtual void RenderFrame( pei::RenderProfilePtr& self, double ticks );

            virtual double GetTicks();

            virtual void Delay( double t );
        };

    } /* namespace wx */
} /* namespace pei */
#endif /* WX_RENDER_PROFILE_H */
