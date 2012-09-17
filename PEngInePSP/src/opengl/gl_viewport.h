/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef GL_VIEWPORT_H
#define GL_VIEWPORT_H

#include <profile_config.h>

#ifdef _HAS_OPENGL_

#include <render/render_node.h>

namespace pei
{
    namespace opengl {
        class Viewport;
        typedef boost::shared_ptr<Viewport> ViewportPtr;

        /* generic class to backup and restore a viewport - ViewportInfos are only valid within a Draw() call! */
        class ViewportInfo
        {
        public:
            virtual ~ViewportInfo() {}

            virtual void SetViewport() = 0;
        };
        typedef boost::shared_ptr<ViewportInfo> ViewportInfoPtr;

        class Viewport : public pei::RenderNode
        {
        protected:
            ViewportInfoPtr m_ViewportInfo;
        public:
            virtual ~Viewport() {}

            virtual int Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param );

            virtual ViewportInfoPtr GetViewportInfo() { return m_ViewportInfo; }

        protected:
            virtual void OnUpdateState( pei::RenderProfilePtr& profile );
        };

        ViewportInfoPtr BackupParentViewport( pei::RenderNodeInterfacePtr node );

        class Viewport2D : public Viewport
        {
        protected:
            class ViewportInfo2D : public ViewportInfo
            {
            public: // only public within its scope
                // viewport
                double vpx, vpy, vpw, vph;
                // ortho
                double ox0, ox1, oy1, oy0;

                ViewportInfo2D();

                virtual ~ViewportInfo2D() {}

                virtual void SetViewport();
            };
            ViewportInfo2D *m_ViewportInfo2D;
        public:
            Viewport2D();

            ~Viewport2D() {}
        protected:
            virtual void SetViewport( double x, double y, double w, double h );

            virtual void OnDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  );

            virtual pei::SurfacePtr OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  );
        };
        typedef boost::shared_ptr<Viewport2D> Viewport2DPtr;

        ////////////////////////////////////////////////////////////////////////////////////
        // client viewport - 2D
        class ClientViewport2D : public Viewport2D
        {
        protected:
            // keep the aspect of original size
            bool m_ScaleChildren;
        public:
            ClientViewport2D( bool scaleChildren = false );

            ~ClientViewport2D();

        protected:
            virtual pei::SurfacePtr OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  );

            virtual int Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param );

            virtual void SetParent( pei::RenderNodeInterfacePtr parent );
        };

        class Viewport3D : public Viewport
        {
        protected:
            class ViewportInfo3D : public ViewportInfo
            {
            public: // only public within its scope
                // viewport
                double vpx, vpy, vpw, vph;

                double fov;
                double zNear;
                double zFar;
                double aspect;
            public:
                ViewportInfo3D();

                virtual ~ViewportInfo3D() {}

                virtual void SetViewport();
            };
            ViewportInfo3D *m_ViewportInfo3D;

            double m_Fov;
            double m_zNear;
            double m_zFar;
        public:
            Viewport3D( double fov = 35.0, double zNear = 1 /*pei::RenderNode::MIN_Z */, double zFar = pei::RenderNode::MAX_Z );

            virtual ~Viewport3D() {}

            virtual void SetFov( double fov, double zNear, double zFar );
        protected:
            virtual void SetViewport( double x, double y, double w, double h ) {}

            virtual pei::SurfacePtr OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  );

            virtual void OnDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  );
        };

        ////////////////////////////////////////////////////////////////////////////////////
        // client viewport - 3D
        class ClientViewport3D : public Viewport3D
        {
        protected:
            // keep the aspect of original size
            bool m_KeepAspect;
            bool m_ScaleChildren;
        public:
            ClientViewport3D( bool keepAspect = true, bool scaleChildren = false );

            ~ClientViewport3D();
        protected:
            virtual int Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param );

            virtual pei::SurfacePtr OnPreDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  );
        };

        ////////////////////////////////////////////////////////////////////////////////////

        template< class VP >
        class EmbeddedViewport : public VP
        {
        public:
            virtual ~EmbeddedViewport( ) {}

            virtual int Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
            {
                ViewportInfoPtr vpi = BackupParentViewport( ((pei::RenderNodeInterface*)this)->GetParent() );
                int n = VP::Draw(profile, buffer, param);
                if ( vpi ) {
                    vpi->SetViewport();
                }
                return n;
            }
        };

        class ViewportWrapper : pei::RenderNode
        {
            virtual int Draw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
            {
                ViewportInfoPtr vpi = BackupParentViewport( GetParent() );
                int n = pei::RenderNode::Draw(profile, buffer, param);
                if ( vpi ) {
                    vpi->SetViewport();
                }
                return n;
            }
        };
    }
} // namespace pei

#endif // _HAS_OPENGL_
#endif // GL_VIEWPORT_H
