/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#ifdef _HAS_OPENGL_

#include <opengl/gl_viewport.h>

namespace pei
{
    namespace opengl {
        ViewportInfoPtr BackupParentViewport( pei::RenderNodeInterfacePtr node )
        {
            ViewportInfoPtr vpi;
            while ( node ) {
                ViewportPtr vp = node->as<Viewport>();
                if ( vp ) {
                    vpi = vp->GetViewportInfo();
                    if ( vpi ) {
                        break;
                    }
                }
                node = node->GetParent();
            }
            return vpi;
        }

        void Viewport::OnUpdateState( pei::RenderProfilePtr& profile )
        {
            if ( MustRefreshState( ) ) {
                RequestRedraw();
            }
        }

        // draw the raw data to destination
        int Viewport::Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            int n(0);
            if ( (m_Flags & EN_DRAWABLE) == EN_DRAWABLE ||
                 (m_Flags & EN_REDRAW) != 0 ) // force redraw
            {
                ++n;
                // this translate the viewport in 2D space
                profile->PushMatrix( param );
                pei::RenderParam p = profile->Translate( m_RenderParam );
                p.SetWidth( GetWidth() ).SetHeight( GetHeight());
                OnPreDraw( profile, buffer, p );
                //////////////////////////////////////////////////////////////////////////
                // default identity for the 3D viewport
                pei::RenderParam identity = profile->LoadIdentity();
                if ( (m_Flags & EN_SKIP_CHILDREN) == 0 && (m_Children.size() > 0) ) {
                    RenderNodeList::iterator it = m_Children.begin();
                    while ( it != m_Children.end() ) {
                        pei::RenderNodeInterfacePtr ro = *it++;
                        n += ro->Draw( profile, buffer, identity );
                    }
                }
                OnDraw( profile, buffer, p );
                profile->PopMatrix();
                m_Flags &= ~EN_REDRAW;
            }
            return n;
        }

        //////////////////////////////////////////////////////////////////////////
        //

        Viewport2D::ViewportInfo2D::ViewportInfo2D()
            : vpx(0), vpy(0), vpw(0), vph(0)
            , ox0(0), ox1(0), oy1(0), oy0(0)
        {
        }

        void Viewport2D::ViewportInfo2D::SetViewport()
        {
            glViewport( vpx, vpy, vpw, vph );
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            glOrtho( ox0, ox1, oy0, oy1, -1000.0f, 1000.0f );
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glDisable( GL_LIGHTING);        // no lighting
            int depth_test;
            glGetIntegerv(GL_DEPTH_TEST, &depth_test);
            if ( depth_test ) {
                glDisable( GL_DEPTH_TEST);      // no depth test in 2D - render in order. Still Z ordered in tree!
//                glClear(GL_DEPTH_BUFFER_BIT);   // clear depth buffer to overlay
            }
        }

        Viewport2D::Viewport2D()
            : m_ViewportInfo2D( new ViewportInfo2D )
        {
            m_ViewportInfo = ViewportInfoPtr( m_ViewportInfo2D );
        }

        void Viewport2D::SetViewport( double x, double y, double w, double h )
        {
            m_ViewportInfo2D->vpx = x;
            m_ViewportInfo2D->vpy = y;
            m_ViewportInfo2D->vpw = w;
            m_ViewportInfo2D->vph = h;

            double x0 = GetXPosition();
            double x1 = x0 + GetWidth() - 1;
            double y1 = GetYPosition();
            double y0 = y1 + GetHeight() - 1;
            m_ViewportInfo2D->ox0 = x0;
            m_ViewportInfo2D->ox1 = x1;
            m_ViewportInfo2D->oy0 = y0;
            m_ViewportInfo2D->oy1 = y1;
            m_ViewportInfo2D->SetViewport();
        }

        pei::SurfacePtr Viewport2D::OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            // A 2D vieport is always in 2D space.
            // We use screen coords top down to position the viewport to make it fit into a standard 2D layout - otherwise mixing 2D & 3D is a pain
            double sh = profile->GetScreen()->GetHeight();
            SetViewport( param.GetX(), sh - (param.GetY()+param.GetHeight()), param.GetWidth(), param.GetHeight() );
            return buffer;
        }

        void Viewport2D::OnDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //

        ClientViewport2D::ClientViewport2D( bool scaleChildren /* = true */ )
            : m_ScaleChildren(scaleChildren)
        {
        }

        ClientViewport2D::~ClientViewport2D()
        {
        }

        void ClientViewport2D::SetParent( pei::RenderNodeInterfacePtr parent )
        {
            //client viewport inherits parents size
            if ( parent && !m_ScaleChildren ) {
                SetSize( parent->GetWidth(), parent->GetHeight() );
            }
            Viewport2D::SetParent(parent);
        }

        int ClientViewport2D::Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            int n(0);
            // this must react to MustRefreshState() to adjust size
            if ( ((m_Flags & EN_DRAWABLE) == EN_DRAWABLE) ||
                 ((m_Flags & EN_REDRAW) == EN_REDRAW)  ) // force redraw
            {
                ++n;
                // param is the current "matrix"

                // only scale if the viewport has a "virtual" size. Otherwise parent will override, with scale = 1.0
                if (m_ScaleChildren && GetWidth() > 0.01 && GetHeight() > 0.01 ) {
                    SetScale( param.GetWidth()/GetWidth(), param.GetHeight()/GetHeight());
                }
                // this translate the viewport in 2D space
                profile->PushMatrix( param );
                pei::RenderParam p = profile->Translate( m_RenderParam );
                // translate does not know what to do with size - so overwrite manually here
                p.SetWidth(param.GetWidth());
                p.SetHeight(param.GetHeight());
                OnPreDraw( profile, buffer, p );
                //////////////////////////////////////////////////////////////////////////
                // all children render with a new identity
                if ( (m_Flags & EN_SKIP_CHILDREN) == 0 && (m_Children.size() > 0) ) {
                    RenderNodeList::iterator it = m_Children.begin();
                    while ( it != m_Children.end() ) {
                        pei::RenderNodeInterfacePtr ro = *it++;
                        n += ro->Draw( profile, buffer, p );
                    }
                }
                OnDraw( profile, buffer, p );
                profile->PopMatrix();
                m_Flags &= ~EN_REDRAW;
            }
            return n;
        }

        SurfacePtr ClientViewport2D::OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            double sh = profile->GetScreen()->GetHeight();

            // TODO: If rotated, VP must adjust its size to incorporate whole (now rotated) rect

            // use translated param!
            double vpw = param.GetWidth()*param.GetXScale();
            double vph = param.GetHeight()*param.GetYScale();
            double vpx = param.GetX()/* *p.GetXScale() */;
            double vpy = param.GetY()/* *p.GetXScale() */; // mul with scale??
            m_ViewportInfo2D->vpx = vpx;
            m_ViewportInfo2D->vpy = sh-(vpy+vph);
            m_ViewportInfo2D->vpw = vpw;
            m_ViewportInfo2D->vph = vph;

            double x0(vpx);
            double x1(vpx+vpw-1);
            double y0(vpy+vph-1);
            double y1(vpy);
            m_ViewportInfo2D->ox0 = x0;
            m_ViewportInfo2D->ox1 = x1;
            m_ViewportInfo2D->oy0 = y0;
            m_ViewportInfo2D->oy1 = y1;
            m_ViewportInfo2D->SetViewport();
            return buffer;
        }

        //////////////////////////////////////////////////////////////////////////
        //

        Viewport3D::ViewportInfo3D::ViewportInfo3D()
            : vpx(0), vpy(0), vpw(0), vph(0)
            , fov(0), zNear(0), zFar(0), aspect(0)
        {
        }

        void Viewport3D::ViewportInfo3D::SetViewport()
        {
            glViewport( vpx, vpy, vpw, vph );

            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();

            const double pi = 3.1415926535897932384626433832795;
            double fH = tan( fov / 360 * pi ) * zNear;
            double fW = fH * aspect;
            glFrustum( -fW, fW, -fH, fH, zNear, zFar );

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            int depth_test;
            glGetIntegerv(GL_DEPTH_TEST, &depth_test);
            if ( !depth_test ) {
                glEnable( GL_DEPTH_TEST);
            }
            // Not sure if I want to do this by default...
            glClear(GL_DEPTH_BUFFER_BIT);   // clear depth buffer to overlay

        }

        Viewport3D::Viewport3D( double fov /*= 35.0*/, double zNear /*= 1*/, double zFar /*= pei::RenderNode::MAX_Z*/ )
            : m_ViewportInfo3D( new ViewportInfo3D )
            , m_Fov(fov), m_zNear(zNear), m_zFar(zFar)
        {
            m_ViewportInfo = ViewportInfoPtr( m_ViewportInfo3D );
        }

        void Viewport3D::SetFov( double fov, double zNear, double zFar )
        {
            m_Fov   = fov;
            m_zNear = zNear;
            m_zFar  = zFar;
        }

        SurfacePtr Viewport3D::OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            // A 3D vieport is always in 2D space.
            // We use screen coords top down to position the viewport to make it fit into a standard 2D layout - otherwise mixing 2D & 3D is a pain
            double sh = profile->GetScreen()->GetHeight();

            m_ViewportInfo3D->fov   = m_Fov;
            m_ViewportInfo3D->zNear = m_zNear;
            m_ViewportInfo3D->zFar  = m_zFar;

            m_ViewportInfo3D->vpx = param.GetX();
            m_ViewportInfo3D->vpy = sh - (param.GetY()+param.GetHeight());
            m_ViewportInfo3D->vpw = param.GetWidth();
            m_ViewportInfo3D->vph = param.GetHeight();
            m_ViewportInfo3D->aspect = param.GetWidth() / param.GetHeight();
            m_ViewportInfo3D->SetViewport();
            return buffer;
        }

        void Viewport3D::OnDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            // intentionally empty
        }

        //////////////////////////////////////////////////////////////////////////
        //

        ClientViewport3D::ClientViewport3D( bool keepAspect /* = true */, bool scaleChildren /*= false*/ )
            : m_KeepAspect(keepAspect), m_ScaleChildren(scaleChildren)
        {

        }

        ClientViewport3D::~ClientViewport3D()
        {
        }

        int ClientViewport3D::Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            int n(0);
            // this must react to MustRefreshState() to adjust size
            if ( ((m_Flags & EN_DRAWABLE) == EN_DRAWABLE) ||
                 ((m_Flags & EN_REDRAW) == EN_REDRAW)  ) // force redraw
            {
                ++n;
                // param is the current "matrix"

                // only scale if the viewport has a "virtual" size. Otherwise parent will override, with scale = 1.0
                if (m_ScaleChildren && GetWidth() > 0.01 && GetHeight() > 0.01 ) {
                    SetScale( param.GetWidth()/GetWidth(), param.GetHeight()/GetHeight());
                }
                // this translate the viewport in 2D space
                profile->PushMatrix( param );
                pei::RenderParam p = profile->Translate( m_RenderParam );
                // translate does not know what to do with size - so overwrite manually here
                p.SetWidth(param.GetWidth());
                p.SetHeight(param.GetHeight());
                OnPreDraw( profile, buffer, p );
                //////////////////////////////////////////////////////////////////////////
                // all children render with a new identity
                pei::RenderParam identity = profile->LoadIdentity();
                if ( (m_Flags & EN_SKIP_CHILDREN) == 0 && (m_Children.size() > 0) ) {
                    RenderNodeList::iterator it = m_Children.begin();
                    while ( it != m_Children.end() ) {
                        RenderNodeInterfacePtr ro = *it++;
                        n += ro->Draw( profile, buffer, identity );
                    }
                }
                OnDraw( profile, buffer, p );
                profile->PopMatrix();
                m_Flags &= ~EN_REDRAW;
            }
            return n;
        }

        pei::SurfacePtr ClientViewport3D::OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            double sh = profile->GetScreen()->GetHeight();

            // TODO: If rotated, VP must adjust its size to incorporate whole (now rotated) rect

            double vpx = param.GetX()/* *p.GetXScale() */;
            double vpy = param.GetY()/* *p.GetXScale() */; // mul with scale??
            double vpw = param.GetWidth()*param.GetXScale();
            double vph = param.GetHeight()*param.GetYScale();

            m_ViewportInfo3D->fov   = m_Fov;
            m_ViewportInfo3D->zNear = m_zNear;
            m_ViewportInfo3D->zFar  = m_zFar;

            m_ViewportInfo3D->vpx = vpx;
            m_ViewportInfo3D->vpy = sh-(vpy+vph);
            m_ViewportInfo3D->vpw = vpw;
            m_ViewportInfo3D->vph = vph;
            if ( m_KeepAspect ) {
                m_ViewportInfo3D->aspect = vpw / vph;
            } else {
                m_ViewportInfo3D->aspect = GetWidth()/GetHeight();
            }
            m_ViewportInfo3D->SetViewport();
            return buffer;
        }
    }
} // namespace pei

#endif // _HAS_OPENGL_
