/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "FT_font_factory.h"
#include "font_manager.h"

#include <utils/utils.h>

#include <sstream>

#include <map>
#include <string>
#include <iostream>

#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _WIN32 // font search path
#include <windows.h>
#endif

namespace pei
{
    FT_Library s_FtLibrary = NULL;

    class FtFontFactoryPrivate
    {
    public:
        FtFontFactoryPrivate() { }

        ~FtFontFactoryPrivate() { }
    };

    class FtFont : public TtFont
    {
    public:
        FT_Face *m_FontFace;

        FtFont( )  {}

        virtual ~FtFont() {
        }

        virtual void* GetFontHandle() { return m_FontFace; }
    };
    typedef std::map<std::string, TtFontPtr> FontList;

    FtFontFactory::FtFontFactory()
        : m_Private( new FtFontFactoryPrivate() )
    {
    }

    FtFontFactory::~FtFontFactory()
    {
        if (s_FtLibrary) FT_Done_FreeType(s_FtLibrary);
    }

    int FtFontFactory::Init()
    {
        FT_Error error = FT_Init_FreeType( &s_FtLibrary );
        if ( error ) {
            std::cerr << "Error loading Freetype library\n" << std::endl;
            return false;
        }
        return TtFontFactory::Init();
    }

    TtFontPtr FtFontFactory::OpenTtFont(const char* font, int size)
    {
//        FontManager::FontPathIterator fit = FontManager().GetFontPathBegin();
//        FontManager::FontPathIterator fit_end = FontManager().GetFontPathEnd();
//        while (!pFont && fit != fit_end ) {
//            bfs::path font_path = fit++->second / font;
//#ifdef _HAS_SDL_TTF_
//            pFont = TTF_OpenFont( font_path.string().c_str(), size );
//#endif
//        };
//        // cache font if found
//        if ( pFont ) {
//#ifdef _HAS_SDL_TTF_
//            return TtFontPtr( new SDLTtFont(pFont) );
//#endif
//        }
        return TtFontPtr();
    }
}; // namespace pei

