/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/


#include "bm_font_asset.h"
#include "font_manager.h"

#include <utils/utils.h>

#include <video/blitter.h>
#include <video/render_tools.h>

#include <utils/utils.h>

#include <string>
#include <sstream>

#include <string.h>

#include <boost/thread/mutex.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;

namespace pei
{
    //////////////////////////////////////////////////////////////////////////
    //

    static boost::mutex sMutex;
    static bool isInitialized(false);

    BmFontAsset::BmFontAsset( const char* name, size_t size )
        : FontAsset( name, size )
        , m_FontName(name)
        , m_FontSize(size)
        , m_IsOpen(false)
    {
        bfs::path path_name( name );
        if ( bfs::exists( path_name ) && !bfs::is_directory( path_name ) ) {
            m_FontDirectory = path_name.parent_path().string();
            m_FontName = path_name.filename();
        }

        boost::mutex::scoped_lock lock(sMutex);
        if ( !isInitialized ) {
            pei::FontFactory().Init();
            isInitialized = true;
        }
    }

    bool BmFontAsset::CanHandle()
    {
        return true;
    }

    bool BmFontAsset::Open()
    {
    	if (!m_IsOpen) {
			// always returns a font - if one needs to be created, it will be empty and data must be filled
			m_Font = pei::FontFactory().CreateFont( m_FontName.c_str(), m_FontSize );
			if ( m_Font->IsEmpty() ) {
				if (!GenerateFontStrip( m_Font )) {
					pei::FontFactory().CloseFont( m_Font );
					m_Font = FontPtr();
					return false;
				} else {
					m_IsOpen = true;
					return true;
				}
			}
    	}
        return (m_Font == NULL) ? false : !m_Font->IsEmpty();
    }

    void BmFontAsset::Close()
    {
    	m_IsOpen = false;
    }

    int BmFontAsset::GetMemoryUsage()
    {
        return 0;
    }

    std::string BmFontAsset::GetFormatString() const
    {
        return BM_FONT_EXT;
    }

    SurfacePtr BmFontAsset::RenderText( const char *text, const pei::Color& color /*= pei::Color(255,255,255)*/ )
    {
        if ( strlen( text ) > 0 ) {
            pei::Rectangle dims = GetTextDimensions(text);
            if ( dims.w() > 0 ) {
                // Create a buffer to render into
            	pei::Format fmt(dims.w(),dims.h());
                SurfacePtr s( new pei::Surface( fmt ));
                // clear buffer incl. alpha
                pei::Fill32( s, pei::Color(0,0,0,0) );
                // now render into buffer
                int x(0);
                int y(0);
                RenderText( s, x, y, text, color );
                return s;
            }
        }
        return SurfacePtr();
    }

    void BmFontAsset::RenderText( SurfacePtr dest, int& x, int& y, const char* text, const pei::Color& color /*= pei::XColor(255,255,255)*/ )
    {
        if ( m_Font && !m_Font->IsEmpty() && (strlen( text ) > 0) ) {
            // src fmt is font strip format
            const pei::SurfacePtr fontStrip = m_Font->GetFontStrip();
            if ( fontStrip ) {
                y += fontStrip->GetHeight();
                pei::Blitter blitter( pei::PixOpPtr(new PixOpColorBlend(fontStrip->GetFormat(),dest->GetFormat(), color ) ) );
                const char *c = text;
                pei::Rectangle dr;
                int h = 0;
                while ( *c != '\0') {
                    const Glyph* glyph = m_Font->GetGlyph( (int)*c );
                    if ( glyph ) {
                        dr.x() = x + glyph->left;
                        dr.y() = y - glyph->top;
                        dr.w() = glyph->dimension.w();
                        dr.h() = glyph->dimension.h();
                        blitter.Blit( glyph->surface, glyph->dimension, dest, dr );
                        x += glyph->GetAdvance();
                        if ( h < (int)glyph->dimension.h() ) {
                            h = (int)glyph->dimension.h();
                        }
                    } else {
                        x += m_Font->GetDefaultAdvance();
                    }
                    c++;
                }
            }
        }
    }

    pei::Rectangle BmFontAsset::GetTextDimensions( const char* text ) const
    {
        // collect all glyphs from font asset
        Rectangle rc;
        if ( m_Font && !m_Font->IsEmpty() ) {
            const char *c = text;
            while ( *c != '\0') {
                // TODO: add ascent and descent
                const Glyph* glyph = m_Font->GetGlyph( (int)*c );
                if ( glyph ) {
                    int h = /* glyph->dimension.y() + */ glyph->dimension.h();
                    if ( h > rc.h() ) { rc.h() = h; }
                    rc.w() += glyph->advance;
                } else {
                    rc.w() += m_Font->GetDefaultAdvance();
                }
                c++;
            }
        }
        return rc;
    }

    bool BmFontAsset::GenerateFontStrip( FontPtr bmfont )
    {
        // Font imp has to:
        //  - read font strip image,
        //  - set font strip surface in bmfont
        //  - read charmap (file - or decode from image),
        //  - for all chars in map:
        //      - find char in strip
        //      - create char dimensions
        //      - add char to bmfont

        // Font Layout of .bfn fonts
        // - Fontname is folder
        // - for each size: subfolder within font folder
        // - font has font_strip.png
        // -          font_textuture.png (optional)
        // -          charset.txt

        // the folder can be zip compressed with extension .bfn

        return false;
    }

};
