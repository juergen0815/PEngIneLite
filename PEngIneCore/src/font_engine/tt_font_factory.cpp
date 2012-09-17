/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "tt_font_factory.h"
#include "font_manager.h"

#include <utils/utils.h>

#include <sstream>

#include <map>
#include <string>

#include <stdlib.h>

#ifdef _HAS_SDL_TTF_ // TODO: Change inverse logic
#undef _HAS_SDL_TTF_
//#include <SDL_ttf.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

namespace pei
{
#ifdef _HAS_SDL_TTF_ // TODO: Change inverse logic
    class SDLTtFont : public TtFont
    {
    public:
        TTF_Font *m_Font;

        SDLTtFont( TTF_Font *fnt ) : m_Font(fnt) {}

        virtual ~SDLTtFont() {
            if ( m_Font ) {
                TTF_CloseFont( m_Font );
            }
        }

        virtual void* GetFontHandle() { return m_Font; }
    };
#endif

    typedef std::map<std::string, TtFontPtr> FontList;

    static FontList s_TtfList;

    TtFontFactory::TtFontFactory()
    {
    }

    TtFontFactory::~TtFontFactory()
    {
    }

    void TtFontFactory::DestroyFontList()
    {
        s_TtfList.clear();
#ifdef _HAS_SDL_TTF_
        TTF_Quit();
#endif
    }

    int TtFontFactory::Init()
    {
#ifdef _HAS_SDL_TTF_
        if (TTF_Init() == -1)
        {
            fprintf(stderr,
                "Couldn't initialize SDL_ttf: %s\n", TTF_GetError());
            return -1;
        }
#endif

//#if defined (WIN32) || defined (__CYGWIN__)
//        char buffer[256];
//        ::GetWindowsDirectoryA( buffer, sizeof(buffer) );
//
//        std::string font_path = buffer;  font_path += "\\fonts";
//        FontManager().AddFontPath( font_path.c_str(), 5 );
//#else
//  #if defined ( __MACOSX__ )
//        FontManager().AddFontPath( "/Library/Fonts", 5 );
//  #else
//   #if defined (__LINUX__)
//        FontManager().AddFontPath( "/usr/share/fonts/truetype/freefont", 5 );
//   #endif
//  #endif
//#endif
//#ifdef __QNXNTO__
//        pei::FontManager().AddFontPath( "/usr/fonts/font_repository/adobe" );
//        pei::FontManager().AddFontPath( "/usr/fonts/font_repository/monotype/" );
//#endif

        atexit( TtFontFactory::DestroyFontList );
        return 0;
    }

    void TtFontFactory::CloseTtFont( TtFontPtr font )
    {
        // TODO: how about find???
        FontList::iterator it = s_TtfList.begin();
        if ( it != s_TtfList.end() )
        {
            if ( it->second == font )
            {
                s_TtfList.erase( it );
            }
        }
    }

    TtFontPtr TtFontFactory::FindTtFont(const char* font, int size)
    {
        std::string cfont(font);
        if ( cfont.rfind( ".ttf") != std::string::npos )  {
            cfont = cfont.substr(0, cfont.size()-4 );
        }
        // need size in the name
        std::stringstream fnt_key;
        fnt_key << cfont.c_str() << "_x_" << size << std::ends;

        FontList::iterator it = s_TtfList.find( fnt_key.str() );
        if ( it != s_TtfList.end() )
        {
            return (*it).second;
        }
        return TtFontPtr();
    }

    TtFontPtr TtFontFactory::OpenTtFont(const char* font, int size)
    {
#ifdef _HAS_SDL_TTF_
        TTF_Font *pFont(NULL);

        FontManager::FontPathIterator fit = FontManager().GetFontPathBegin();
        FontManager::FontPathIterator fit_end = FontManager().GetFontPathEnd();
        while (!pFont && fit != fit_end ) {
            bfs::path font_path = fit++->second / font;

            pFont = TTF_OpenFont( font_path.string().c_str(), size );
        };
        // cache font if found
        if ( pFont ) {
            return TtFontPtr( new SDLTtFont(pFont) );
        }
#endif
        return TtFontPtr();
    }

    TtFontPtr TtFontFactory::CreateTtFont( const char* font, int size )
    {
        TtFontPtr fnt = FindTtFont(font, size);
        if ( fnt == NULL ) {
            fnt = OpenTtFont( font, size );
            if ( fnt ) {
                std::string cfont(font);
                if ( cfont.rfind( ".ttf") != std::string::npos )  {
                    cfont = cfont.substr(0, cfont.size()-4 );
                }
                // need size in the name
                std::stringstream fnt_key;
                fnt_key << cfont.c_str() << "_x_" << size << std::ends;
                s_TtfList[ fnt_key.str() ] = fnt;
            }
        }
        return fnt;
    }
}; // namespace pei

