/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef FONT_ASSET_H
#define FONT_ASSET_H

#include <interfaces/asset_interface.h>

#include <render/rectangle.h>
#include <render/color.h>
#include <render/render_surface.h>

#include <boost/shared_ptr.hpp>

namespace pei
{
    class FontAsset : public Asset
    {
    public:
        FontAsset( const char* name, size_t size );

        virtual ~FontAsset() {}

        virtual SurfacePtr RenderText( const char *text, const pei::Color& color = pei::Color(255,255,255) ) = 0;

        virtual void RenderText( SurfacePtr dest, int& x, int& y, const char* text, const pei::Color& color = pei::Color(255,255,255) ) = 0;

        virtual void RenderTextA( SurfacePtr dest, int x, int y, const char* text, const pei::Color& color = pei::Color(255,255,255) ) const
        {
            const_cast<FontAsset*>(this)->RenderText( dest, x, y, text, color );
        }

        virtual pei::Rectangle GetTextDimensions( const char* text ) const = 0;

    };

    typedef boost::shared_ptr<FontAsset> FontAssetPtr;

};

#endif // FONT_ASSET_H
