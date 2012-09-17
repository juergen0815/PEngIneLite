/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/


#include "bm_font_factory.h"
#include "bm_font_asset.h"
#include "font_manager.h"

#include <utils/utils.h>

#include <map>
#include <string>
#include <sstream>

#include <stdlib.h>

namespace pei
{
    typedef std::map<std::string, FontPtr > FontMap;

    static FontMap s_FontList;

    FontFactory::FontFactory()
    {
    }

    FontFactory::~FontFactory()
    {
    }

    void FontFactory::DestroyFontList()
    {
        s_FontList.clear();
    }

    int FontFactory::Init()
    {
        atexit( FontFactory::DestroyFontList );
        return 0;
    }

    struct FindFont
    {
        FontPtr font;

        FindFont( FontPtr f ) : font(f) {}

        bool operator()( const std::pair< std::string, FontPtr > other ) const
        {
            return other.second == font;
        }
    };

    void FontFactory::CloseFont( FontPtr font )
    {
        FontMap::iterator it = std::find_if( s_FontList.begin(), s_FontList.end(), FindFont( font ) );
        if ( it!= s_FontList.end() ) {
            s_FontList.erase( it );
        }
    }

    FontPtr FontFactory::CreateFont( const char* font, int size )
    {
        std::string cfont(font);
        size_t p;
        if ( (p = cfont.rfind( "." )) != std::string::npos )  {
            cfont = cfont.substr(0, p );
        }
        // need size in the name
        std::stringstream fnt_key;
        fnt_key << cfont.c_str() << "_x_" << size << std::ends;

        FontMap::iterator it = s_FontList.find( fnt_key.str() );
        if ( it != s_FontList.end() )
        {
            return (*it).second;
        }
        FontPtr bmfont( new Font );
        s_FontList[ fnt_key.str() ] = bmfont;
        return bmfont;
    }

    //////////////////////////////////////////////////////////////////////////
    //

    Font::Font()
        : m_DefaultAdvance(4)
    {
    }

    Font::~Font()
    {
    }

    const Glyph* Font::GetGlyph( int character ) const
    {
        CharMap::const_iterator fit = m_CharMap.find( character );
        if ( fit != m_CharMap.end() ) {
            return &fit->second;
        }
        return NULL;
    }

    void Font::SetFontStrip( SurfacePtr font_surface )
    {
        m_FontStrip = font_surface;
    }

    void Font::AddGlyph( int _char, const Rectangle& rc, const SurfacePtr& surface )
    {
        m_CharMap[ _char ] = Glyph( _char, rc, surface );
    }

    void Font::AddGlyph( const Glyph& glyph  )
    {
        m_CharMap[ glyph.character ] = glyph;
    }

    const SurfacePtr Font::GetFontStrip() const
    {
        return m_FontStrip;
    }

    int Font::GetDefaultAdvance() const
    {
        return m_DefaultAdvance;
    }

    void Font::SetDefaultAdvance( int adv )
    {
        m_DefaultAdvance = adv;
    }


}; // namespace pei
