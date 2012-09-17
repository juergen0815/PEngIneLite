/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef GL_RENDER_PROFILE_H
#define GL_RENDER_PROFILE_H

#include <profile_config.h>

#ifdef _HAS_OPENGL_

#include <render/render_profile.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>

#include <string>
#include <map>

namespace pei
{
    namespace opengl {
        class RenderProfile;
        typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

        class RenderProfile : public pei::RenderProfile
        {
        protected:
            RenderProfile();
        public:
            /** install a clip regin in window coordinates */
            virtual void SetClipRegion( const pei::Rectangle& bound );

            virtual void ClearClipRegion();

            virtual void EnableClipping();

            virtual void DisableClipping();

            virtual void ClearBuffers();

            virtual void SwapBuffers() = 0;

            virtual void Blit( pei::SurfacePtr source, pei::SurfacePtr dest, const pei::RenderParam& param );

            virtual void Fill( pei::SurfacePtr surface, const pei::RenderParam& param );

            virtual pei::SurfacePtr CreateSurface( const pei::Format& fmt);

            virtual bool Enable( int attr );

            virtual bool Disable( int attr );

            virtual pei::RenderParam LoadIdentity( const pei::RenderParam& identity = pei::RenderParam() );

            // need matrix enums
            virtual void SetMatrixMode( unsigned int mode );

            virtual const Matrix4x4& GetMatrix() const;

            virtual Matrix4x4& GetMatrix();

            virtual void LoadMatrix( const Matrix4x4& matrix );

            virtual const pei::RenderParam& PushMatrix( const pei::RenderParam& );

            virtual pei::RenderParam Translate( const RenderParam& param );

            virtual const pei::RenderParam& PopMatrix();
        };
    }
} /* namespace pei */
#endif // _HAS_OPENGL_

#endif /* GL_RENDER_PROFILE_H */

