/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef SFONT_ASSET_H
#define SFONT_ASSET_H

#include <interfaces/asset_interface.h>

#include <font_engine/font_manager.h>
#include <font_engine/bm_font_asset.h>

namespace pei
{
    class SFontAsset : public pei::BmFontAsset
    {
    public:
        SFontAsset( const char* name, size_t size );

        virtual ~SFontAsset() {}

        /** abstract format description */
        virtual std::string GetFormatString() const { return ".png"; }

    protected:

        // overload this if you want to use you own customize 
        // font specs. The BitmapFontPtr will be an empty BitmapFont class for you
        // to be filled. E.g no font strip, no charset mapping.
        // The font strip in the BitmapFont is a linear 2D surface containing all
        // characters. These are mapped with position and dimension to the appropriate
        // int character id (depending on encoding, simple encoding would be ASCII)
        virtual bool GenerateFontStrip( pei::FontPtr bmfont );

    };
};


#endif // SFONT_ASSET_H
