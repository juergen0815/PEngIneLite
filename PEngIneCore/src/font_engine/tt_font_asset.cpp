/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "tt_font_asset.h"
#include "tt_font_factory.h"
#include "font_manager.h"

#include <render/render_surface.h>
#include <video/blitter.h>

// TODO: This has to go
#ifdef _HAS_SDL_TTF_
#include <SDL_ttf.h>
#include <SDL/sdl_surface.h>
#endif

#include <utils/utils.h>

#include <boost/thread/mutex.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


namespace bfs = boost::filesystem;

namespace pei
{
    static boost::mutex sMutex;
    static bool isInitialized(false);

    //////////////////////////////////////////////////////////////////////////
    //

    TtFontAsset::TtFontAsset( const char* name, size_t size )
        : FontAsset( name, size )
        , m_FontName(name)
        , m_FontSize(size)
    {
        sMutex.lock();
        if ( !isInitialized ) {
            pei::TtFontFactory().Init();
            isInitialized = true;
        }
        sMutex.unlock();
    }

    TtFontAsset::TtFontAsset( const TtFontAsset& fa )
        : FontAsset(fa.m_FontName.c_str(), fa.m_FontSize)
        , m_FontName(fa.m_FontName)
        , m_FontSize(fa.m_FontSize)
        , m_TtFont(fa.m_TtFont)
    {
    }

    TtFontAsset::~TtFontAsset()
    {
    }

    bool TtFontAsset::CanHandle()
    {
        m_TtFont = pei::TtFontFactory().CreateTtFont( m_FontName.c_str(), m_FontSize );
        return m_TtFont != NULL;
    }

    bool TtFontAsset::Open()
    {
        if ( m_TtFont == NULL ) {
            m_TtFont = pei::TtFontFactory().CreateTtFont( m_FontName.c_str(), m_FontSize );
        }
        return m_TtFont != NULL;
    }

    void TtFontAsset::Close()
    {
        m_TtFont = TtFontPtr();
    }

    int TtFontAsset::GetMemoryUsage()
    {
        return 0;
    }

    std::string TtFontAsset::GetFormatString() const
    {
        return ".ttf";
    }

    const char* TtFontAsset::GetResourceDirectory() const
    {
        return "";
    }

    SurfacePtr TtFontAsset::RenderText( const char *text, const pei::Color& color /*= pei::Color(255,255,255)*/ )
    {
#ifdef _HAS_SDL_TTF_
        if ( m_TtFont ) {
            // this is a hack! We swap r & g and correct the color format manually
            // SDL_ttf renders ARGB, but we need ABGR (or LE RGBA)
            SDL_Color c = { color.b, color.g, color.r, color.a };
            SDL_Surface * s = TTF_RenderText_Blended( (TTF_Font*)m_TtFont->GetFontHandle(), text, c );
            // swap r&g in the format description
            s->format->Rmask = 0x000000ff; s->format->Rshift = 0;
            s->format->Bmask = 0x00ff0000; s->format->Bshift = 16;
            return SurfacePtr( new pei::SDL::SurfaceWrapper(s) );
        }
#endif
        return SurfacePtr();
    }

    void TtFontAsset::RenderText( SurfacePtr dest, int& x, int& y, const char* text, const pei::Color& color /*= pei::XColor(255,255,255)*/ )
    {
        //     assert ( pFont );
        if (!m_TtFont || dest.get() == NULL ) {
            return;
        }
#ifdef _HAS_SDL_TTF_
        SDL_Surface *text_surface = NULL;
        // this is a hack! We swap r & g and correct the color format manually
        // SDL_ttf renders ARGB, but we need ABGR (or LE RGBA) to match RGBA texture format - performace reasons!
        SDL_Color c = { color.b, color.g, color.r, color.a };
        if ( (text_surface = TTF_RenderText_Blended( (TTF_Font*)m_TtFont->GetFontHandle(), text, c )) != NULL )
        {
            // swap r&g in the format description
            text_surface->format->Rmask = 0x000000ff; text_surface->format->Rshift = 0;
            text_surface->format->Bmask = 0x00ff0000; text_surface->format->Bshift = 16;

        	pei::SurfacePtr txt( new pei::SDL::SurfaceWrapper(text_surface));
            pei::Blitter blitter(pei::PixOpPtr(new PixOpAlphaBlitSrcAlpha(txt->GetFormat(),dest->GetFormat()) ) );
            blitter.Blit( txt, dest, x, y, txt->GetWidth(), txt->GetHeight(), 0, 0 );

            int tw, th;
            TTF_SizeText( (TTF_Font*)m_TtFont->GetFontHandle(), text, &tw, &th);

            Uint32 font_height = TTF_FontHeight((TTF_Font*)m_TtFont->GetFontHandle()); // - a - 2*d;
            y += font_height;
            x += tw;
        }
#endif
    }

    void TtFontAsset::RenderTextA( SurfacePtr dest, int x, int y, const char* text, const pei::Color& color /*= pei::XColor(255,255,255)*/ )
    {
        //     assert ( pFont );
        if (!m_TtFont || dest.get() == NULL ) {
            return;
        }
#ifdef _HAS_SDL_TTF_
        SDL_Surface *text_surface = NULL;
        // this is a hack! We swap r & g and correct the color format manually
        // SDL_ttf renders ARGB, but we need ABGR (or LE RGBA) to match RGBA texture format - performace reasons!
        SDL_Color c = { color.b, color.g, color.r, color.a };
        if ( (text_surface = TTF_RenderText_Blended( (TTF_Font*)m_TtFont->GetFontHandle(), text, c )) != NULL )
        {
            // swap r&g in the format description
            text_surface->format->Rmask = 0x000000ff; text_surface->format->Rshift = 0;
            text_surface->format->Bmask = 0x00ff0000; text_surface->format->Bshift = 16;

            pei::SurfacePtr txt( new pei::SDL::SurfaceWrapper(text_surface));
            pei::Blitter blitter(pei::PixOpPtr(new PixOpAlphaBlitSrcAlpha(txt->GetFormat(),dest->GetFormat()) ) );
            blitter.Blit( txt, dest, x, y, txt->GetWidth(), txt->GetHeight(), 0, 0 );
        }
#endif
    }

    pei::Rectangle TtFontAsset::GetTextDimensions( const char* text ) const
    {
        Rectangle rc;
        if ( m_TtFont ) {
            int w(0),h(0);
#ifdef _HAS_SDL_TTF_
            TTF_SizeText( static_cast<TTF_Font*>(m_TtFont->GetFontHandle()), text, &w, &h);
#endif
            rc.w() = w;
            rc.h() = h;
        }
        return rc;
    }
};

