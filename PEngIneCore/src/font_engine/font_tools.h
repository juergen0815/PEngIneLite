/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef FONT_TOOLS_H
#define FONT_TOOLS_H

// #include <utils/xcolor.h>
// 
// #include <SDL_video.h>

#include "font_asset.h"

#include <boost/shared_ptr.hpp>

namespace pei
{
    class FontAsset;

#if 0
    SDL_Surface *TtfDrawTextRef( SDL_Surface* dest, const TTF_Font *pFont, const char* text,
                                     int& x, int& y, const XColor &color = XColor(255,255,255) );

    SDL_Surface *TtfDrawText( SDL_Surface* dest, const TTF_Font *pFont, const char* text,
                             int x, int y, const XColor &color = XColor(255,255,255) )
    {
        return DrawTextRef( dest, pFont, text, x, y, color );
    }

    SDL_Surface *TtfDrawText( const TTF_Font *pFont, const char* text, const XColor &color = XColor(255,255,255) );
#endif

//     SDL_Surface *RenderTextRef( SDL_Surface* dest, FontAsset *pFont, const char* text,
//         int& x, int& y, const XColor &color = XColor(255,255,255) );
// 
//     SDL_Surface *RenderText( SDL_Surface* dest, FontAsset *pFont, const char* text,
//                            int x, int y, const XColor &color = XColor(255,255,255) )
//     {
//         return RenderTextRef( dest, pFont, text, x, y, color );
//     }
// 
//     SDL_Surface *RenderText( FontAsset *pFont, const char* text, const XColor &color = XColor(255,255,255) );
// 
//     void GetTextDimensions( FontAsset *pFont, const char* text, int &w, int& h );

    ///
//     SDL_Surface *RenderTextRef( SDL_Surface* dest, FontAssetPtr::shared_ptr<FontAsset> font, const char* text,
//         int& x, int& y, const XColor &color = XColor(255,255,255) )
//     {
//         return RenderTextRef( dest, font.get(), text, x, y, color );
//     }
// 
//     SDL_Surface *RenderText( SDL_Surface* dest, FontAssetPtr::shared_ptr<FontAsset> font, const char* text,
//         int x, int y, const XColor &color = XColor(255,255,255) )
//     {
//         return RenderTextRef( dest, font.get(), text, x, y, color );
//     }
// 
//     SDL_Surface *RenderText( FontAssetPtr::shared_ptr<FontAsset> font, const char* text, const XColor &color = XColor(255,255,255) )
//     {
//         return RenderText( font.get(), text, color );
//     }
// 
//     void GetTextDimensions( FontAssetPtr::shared_ptr<FontAsset> font, const char* text, int &w, int& h ) {
//         return GetTextDimensions( font.get(), text, w, h );
//     }

    pei::FontAssetPtr CreateFontAsset( const char* name, size_t size, char* type = "ttf" );
};

#endif // FONT_TOOLS_H
