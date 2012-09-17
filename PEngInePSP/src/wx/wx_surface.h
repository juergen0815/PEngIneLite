/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef WX_SURFACE_H
#define WX_SURFACE_H

#include <render/render_surface.h>

#include <wx/image.h>
#include <wx/bitmap.h>

namespace pei {
    namespace wx
    {

        class Surface : public pei::Surface
        {
        protected:
            struct NullDeleter {
                void operator()( void const* ) const { }
            };

            Surface() {}
        public:

            Surface( const wxImage& image );

            Surface( const wxBitmap& bitmap );

            Surface( const pei::SurfacePtr& other );

            virtual wxImage asImage() const;

            static wxImage ConveToImage( const pei::SurfacePtr& surface );

        protected:
            void SetPixels( const wxImage& image );
        };

        // Same as above but does not copy surface
        class SurfaceRef : public wx::Surface
        {
            pei::SurfacePtr m_SurfaceRef;

            // no body = hidden c'tors!
            SurfaceRef( const wxImage& image );

            SurfaceRef( const wxBitmap& bitmap );
        public:
            SurfaceRef( const pei::SurfacePtr& ref );

            virtual wxImage asImage() const;
        };
    }

}

#endif /* WX_SURFACE_H */
