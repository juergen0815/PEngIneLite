/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef BM_FONT_ASSET_H
#define BM_FONT_ASSET_H

#include "font_asset.h"
#include "bm_font_factory.h"

namespace pei
{
    //////////////////////////////////////////////////////////////////////////
    // Bitmap font asset

    #define BM_FONT_EXT ".bfn"

    class BmFontAsset : public FontAsset
    {
    protected:
        std::string m_FontDirectory;
        std::string m_FontName;
        size_t      m_FontSize;

        FontPtr     m_Font;
        bool        m_IsOpen;
    public:
        BmFontAsset( const char* name, size_t size );

        virtual ~BmFontAsset() {}

        /** check if the assert can handle the data associated */
        virtual bool CanHandle();

        /** open the resource */
        virtual bool Open( );

        /** close the resource handle */
        virtual void Close( );

        /** memory need to load the asset */
        virtual int GetMemoryUsage();

        /** abstract format description */
        virtual std::string GetFormatString() const;

        /** get the stored resource name */
        virtual const char* GetResourceName() const {
            return m_FontName.c_str();
        };

        /** get the stored resource name */
        virtual const char* GetResourceDirectory() const {
            return m_FontDirectory.c_str();
        }

        SurfacePtr RenderText( const char *text, const pei::Color& color = pei::Color(255,255,255) );

        virtual void RenderText( SurfacePtr dest, int& x, int& y, const char* text, const pei::Color& color = pei::Color(255,255,255) );

        virtual pei::Rectangle GetTextDimensions( const char* text ) const;

    protected:

        // overload this if you want to use you own customize 
        // font specs. The BitmapFontPtr will be an empty BitmapFont class for you
        // to be filled. E.g no font strip, no charset mapping.
        // The font strip in the BitmapFont is a linear 2D surface containing all
        // characters. These are mapped with position and dimension to the appropriate
        // int character id (depending on encoding, simple encoding would be ASCII)
        virtual bool GenerateFontStrip( FontPtr bmfont );

    };

};

#endif // BM_FONT_ASSET_H
