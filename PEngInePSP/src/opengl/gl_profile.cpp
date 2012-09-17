/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#ifdef _HAS_OPENGL_

#include <interfaces/render_node_interface.h>

#include <opengl/gl_profile.h>

#include <opengl/gl_target.h>
#include <render/tex_surface.h>

namespace pei
{
    namespace opengl {
        RenderProfile::RenderProfile()
        {
            // does not run in render thread!!!
            m_Flags &= (~pei::RenderProfile::EN_RENDER_ASYNC);

            // over write default render target
            pei::RenderTargetInterfacePtr rti( new pei::opengl::RenderTarget() );
            // set a default render target - will be over written by implementation
            SetDefaultRenderTarget( rti );
            // do not clone default. We can use this directly
            AddRenderTarget( DEFAULT_TARGET, rti );

            // we can use this here directly, too
            if ( m_Screen ) {
                // viewport does not render
                m_Screen->SetRenderTarget( RenderTargetInterfacePtr( new pei::RenderTarget));
                m_Screen->SetName("SDL::GL::MainViewport");
            }
            // default matrix mode is model view matrix
            SetMatrixMode( GL_MODELVIEW_MATRIX );
        }

        void RenderProfile::ClearBuffers()
        {
            glClearColor( m_ClearColor.R(),
                          m_ClearColor.G(),
                          m_ClearColor.B(),
                          m_ClearColor.A() );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
        }

        void RenderProfile::Blit( pei::SurfacePtr source, pei::SurfacePtr dest, const pei::RenderParam& param )
        {

    #if 0
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();

            glOrtho( dest->GetXOffset(), dest->GetWidth(),
                     dest->GetHeight(), dest->GetYOffset(),
                     1.0f, -1.0f );

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
    #endif
            glPushAttrib( GL_LIGHTING_BIT | GL_DEPTH_BITS | GL_STENCIL_BITS );
            {
                glPushMatrix();
                glDisable( GL_LIGHTING);
                glDisable( GL_STENCIL_TEST);

                glClear( GL_DEPTH_BUFFER_BIT);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                const pei::Color& c = param.GetColor();
                glColor4f( c.R(), c.G(), c.B(), c.A() );

                glBegin( GL_QUADS );
                    glVertex2f( param.GetX(), param.GetY());
                    glVertex2f( param.GetX() + param.GetWidth(), param.GetY() );
                    glVertex2f( param.GetX() + param.GetWidth(), param.GetY() + param.GetHeight() );
                    glVertex2f( param.GetX(), param.GetY() + param.GetHeight() );
                glEnd();
                glPopMatrix();
            }
            glPopAttrib();

        }

        void RenderProfile::Fill( pei::SurfacePtr surface, const pei::RenderParam& param )
        {
            // must have valid surface attached
            if ( surface.get() == NULL ) {
                return;
            }
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();

            glOrtho( surface->GetXOffset(), surface->GetWidth(), surface->GetHeight(), surface->GetYOffset(), 1.0f, -1.0f );

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glPushAttrib( GL_LIGHTING_BIT | GL_DEPTH_BITS | GL_STENCIL_BITS );
            {
                glDisable( GL_LIGHTING );
                glDisable( GL_STENCIL_TEST );
                glDisable( GL_DEPTH_TEST );
                glDisable( GL_TEXTURE_2D );

                glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BITS );
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glPushMatrix();
                const pei::Color& c = param.GetColor();
                glColor4f( c.R(), c.G(), c.B(), c.A() );
                glBegin( GL_QUADS );
                    glVertex2f( param.GetX(), param.GetY());
                    glVertex2f( param.GetX() + param.GetWidth(), param.GetY() );
                    glVertex2f( param.GetX() + param.GetWidth(), param.GetY() + param.GetHeight() );
                    glVertex2f( param.GetX(), param.GetY() + param.GetHeight() );
                glEnd();
                glPopMatrix();
            }
            glPopAttrib();

        }

        pei::SurfacePtr RenderProfile::CreateSurface( const pei::Format& fmt )
        {
            // -- has no data !!
            return pei::SurfacePtr( new Texture2DSurface( ) );
        }

        bool RenderProfile::Enable( int attr )
        {
            GLint on;
            glGetIntegerv( (GLenum)attr, &on );
            if ( !on ) {
                glEnable( (GLenum)attr );
            }
            return on != 0;
        }

        bool RenderProfile::Disable( int attr )
        {
            GLint on;
            glGetIntegerv( (GLenum)attr, &on );
            if ( on ) {
                glDisable( (GLenum)attr );
            }
            return on != 0;
        }

        pei::RenderParam RenderProfile::LoadIdentity( const pei::RenderParam& identity /* = pei::RenderParam */ )
        {
            glLoadIdentity(); // needs to load the new matrix from RenderParam - sometimes
            return pei::RenderProfile::LoadIdentity( identity );
        }

        void RenderProfile::SetMatrixMode( unsigned int mode )
        {
            glMatrixMode( mode );
            pei::RenderProfile::SetMatrixMode( mode );
        }

        const pei::RenderParam& RenderProfile::PushMatrix( const pei::RenderParam& p )
        {
            glPushMatrix();
            return pei::RenderProfile::PushMatrix( p );
        }

        pei::RenderParam RenderProfile::Translate( const pei::RenderParam& p )
        {
            // translate to origin to scale/rotate
//            glTranslatef( p.GetX()-p.GetXOrigin(), p.GetY()-p.GetYOrigin(), p.GetZ()-p.GetZOrigin() );
            glTranslatef( p.GetX(), p.GetY(), p.GetZ() );
            glRotatef( p.GetXRotation(), 1.0, 0.0, 0.0 );
            glRotatef( p.GetYRotation(), 0.0, 1.0, 0.0 );
            glRotatef( p.GetZRotation(), 0.0, 0.0, 1.0 );
            glScalef( p.GetXScale(), p.GetYScale(), p.GetZScale() );
            // translate back to real position
//            glTranslatef( p.GetXOrigin(), p.GetYOrigin(), p.GetZOrigin() );

            // we need to return the last on the stack - do not apply current

            return pei::RenderProfile::Translate( p );
        }

        const Matrix4x4& RenderProfile::GetMatrix() const
        {
            GLenum matrixType;
            switch (GetMatrixMode())
            {
                case GL_MODELVIEW:  matrixType = GL_MODELVIEW_MATRIX; break;
                case GL_PROJECTION: matrixType = GL_PROJECTION_MATRIX; break;
                case GL_TEXTURE:    matrixType = GL_TEXTURE_MATRIX; break;
                case GL_COLOR:      matrixType = GL_COLOR_MATRIX; break;
                default: return pei::RenderProfile::GetMatrix();
            }
            glGetFloatv( matrixType, (float*)m_Matrix.m );
            return pei::RenderProfile::GetMatrix();
        }

        Matrix4x4& RenderProfile::GetMatrix()
        {
            GLenum matrixType;
            switch (GetMatrixMode())
            {
                case GL_MODELVIEW:  matrixType = GL_MODELVIEW_MATRIX; break;
                case GL_PROJECTION: matrixType = GL_PROJECTION_MATRIX; break;
                case GL_TEXTURE:    matrixType = GL_TEXTURE_MATRIX; break;
                case GL_COLOR:      matrixType = GL_COLOR_MATRIX; break;
                default: return pei::RenderProfile::GetMatrix();
            }
            glGetFloatv( matrixType, (float*)m_Matrix.m );
            return pei::RenderProfile::GetMatrix();
        }

        void RenderProfile::LoadMatrix( const Matrix4x4& matrix )
        {
            glLoadMatrixf( (float*)matrix.asArray() );
            return pei::RenderProfile::LoadMatrix(matrix);
        }

        const pei::RenderParam& RenderProfile::PopMatrix()
        {
            glPopMatrix();
            return pei::RenderProfile::PopMatrix();
        }

        void RenderProfile::SetClipRegion( const pei::Rectangle& bounds )
        {
            pei::RenderProfile::SetClipRegion( bounds );
        }

        void RenderProfile::ClearClipRegion()
        {
            pei::RenderProfile::ClearClipRegion();
        }

        void RenderProfile::EnableClipping()
        {
            // render reflection surface into stencil buffer
            int depth_test_enabled;
            glGetIntegerv(GL_DEPTH_TEST, &depth_test_enabled);
            if (depth_test_enabled) {
                glDisable(GL_DEPTH_TEST);
            }

            /* Draw 1 into the stencil buffer. */
            int stencil_enabled;
            glGetIntegerv(GL_STENCIL_TEST, &stencil_enabled);
            if (!stencil_enabled) {
                glEnable(GL_STENCIL_TEST);
            }
            glColorMask(false,false,false,false);
            glStencilFunc( GL_ALWAYS, 1, 1);
            glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
            glColor4f(1,1,1,1);
            // we are already transformed
            GLfloat vertices[] =
            {
                m_ClipRegion.minX(), m_ClipRegion.minY(),
                m_ClipRegion.maxX(), m_ClipRegion.minY(),
                m_ClipRegion.minX(), m_ClipRegion.maxY(),
                m_ClipRegion.maxX(), m_ClipRegion.maxY(),
            };
            int vertex_array_enabled;
            glGetIntegerv(GL_VERTEX_ARRAY, &vertex_array_enabled);
            if (!vertex_array_enabled) {
                glEnableClientState(GL_VERTEX_ARRAY);
            }
            glVertexPointer(2, GL_FLOAT, 0, vertices);

            GLshort idx[] = { 0, 1, 2, 3 };
            glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, idx );

            if (!vertex_array_enabled) {
                glDisableClientState(GL_VERTEX_ARRAY);
            }

            /* Now, only render where stencil is set to 1. */
            glStencilFunc( GL_EQUAL, 1, 1);  /* draw if ==1 */
            glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP);

            glColorMask(true,true,true,true);
            if (depth_test_enabled) {
                glEnable(GL_DEPTH_TEST);
            }
        }

        void RenderProfile::DisableClipping()
        {
            glDisable(GL_STENCIL_TEST);
            pei::RenderProfile::DisableClipping();
        }
    }

} /* namespace pei */

#endif
