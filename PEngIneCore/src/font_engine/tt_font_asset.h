/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef TT_FONT_ASSET_H
#define TT_FONT_ASSET_H

#include "font_asset.h"

#include <render/color.h>
#include <render/rectangle.h>

#include <boost/shared_ptr.hpp>

namespace pei
{
    //////////////////////////////////////////////////////////////////////////
    // TTF asset
    class TtFont;
    typedef boost::shared_ptr<TtFont> TtFontPtr;

    class TtFontAsset : public FontAsset
    {
        std::string     m_FontName;
        size_t          m_FontSize;
        TtFontPtr       m_TtFont;

        TtFontAsset( const TtFontAsset& fa );
    public:
        TtFontAsset( const char* name, size_t size );

        virtual ~TtFontAsset();

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
        virtual const char* GetResourceDirectory() const;

        virtual SurfacePtr RenderText( const char *text, const pei::Color& color = pei::Color(255,255,255) );

        virtual void RenderText( SurfacePtr dest, int& x, int& y, const char* text, const pei::Color& color = pei::Color(255,255,255) );

        virtual void RenderTextA( SurfacePtr dest, int x, int y, const char* text, const pei::Color& color = pei::Color(255,255,255) );

        virtual pei::Rectangle GetTextDimensions( const char* text ) const;

    };
};

#endif // TT_FONT_ASSET_H

