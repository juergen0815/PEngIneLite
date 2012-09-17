/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "sfont_asset.h"

#include <decoder/dec_video.h>

#include <font_engine/font_manager.h>

#include <decoder/dec_video_png_image.h>

#include <video/render_tools.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;

namespace pei
{
    // Full font encoded charset for SFONT fonts - need full ASCII ?? encoding
    static const char charset[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\0";

    SFontAsset::SFontAsset( const char* name, size_t size )
        : BmFontAsset(name, size)
    {
        bfs::path fontName( name );
        if ( fontName.extension().size() < 3 ) {
            m_FontName += GetFormatString();
        }
    }

    bool SFontAsset::GenerateFontStrip( pei::FontPtr font )
    {
    	bool font_ok(false);

        pei::FontManager fnt_manager;
        pei::FontManager::FontPathIterator fit     = fnt_manager.GetFontPathBegin();
        pei::FontManager::FontPathIterator fit_end = fnt_manager.GetFontPathEnd();
        while (!font_ok && fit != fit_end ) {
            bfs::path full_file_name = fit++->second / GetResourceName();
            bool fontExists = bfs::exists( full_file_name );
            bool hasExtension = full_file_name.extension() == GetFormatString();
            if ( hasExtension && fontExists )
            {
                // SFont decoder
                pei::VideoAssetPtr img = pei::PngImageAsset::Create( full_file_name.string().c_str() );
                if ( img->Open() ) {
                    // default format is RGBA8888
                    pei::Format fmt;
                    const pei::SurfacePtr fontStrip = img->Decode( fmt, 0 );
                    font->SetFontStrip( fontStrip );

                    pei::Rectangle bounds = pei::DetectSurfaceEdge( fontStrip, 0, 1 );
                    unsigned int* pline = (unsigned int*)fontStrip->GetPixels();
                    unsigned int w = fontStrip->GetWidth();
                    const char *character = charset;
                    unsigned int i = 0;
                    const unsigned int colkey(0xffff00ff); // cyan color key - 0xff49 - gray + alpha

                    // scan this area only
                    pei::Rectangle char_dim( bounds.x(), bounds.y(), bounds.w() - bounds.x(), bounds.h() );
                    i = char_dim.x(); pline = &pline[i];

                    // find first char
                    while ( *pline != colkey && i < w ) {
                    	i++; pline++;
                    }
                    while ( *pline == colkey && i < w ) {
                    	i++; pline++;
                    }
                    char_dim.x() = i;
                    char_dim.w() = 0;
                    int char_idx(0);
                    // find remaining chars
                    while ( i < w && *character != '\0' ) {
                        while ( *pline != colkey && i < w ) {
                        	i++; pline++;
                        }
                        // glyph must be at least 1 pix wide
                        if ( i > char_dim.x() ) {
                            char_dim.w() = i - char_dim.x();
                            // note to self: possibly remove surface param. I might have fonts with surfaces for each glyph
                            Glyph glyph((int)*character++, char_dim, fontStrip);
                            glyph.top = fontStrip->GetHeight();
                            font->AddGlyph( glyph );
                            font_ok = true;
                            while ( *pline == colkey && i < w ) {
                            	i++; pline++;
                            }
                            char_dim.x() = i;
                            char_idx++;
                        } else {
                            // something went wrong. abort
                            break;
                        }
                    }
                    const Glyph* glyph = font->GetGlyph( (int)'!' );
                    if ( !glyph ) {
                        // TODO: In SFont this is calculated in CharPos[2] - CharPos[1]
                        font->SetDefaultAdvance( 4 );
                    } else {
                        font->SetDefaultAdvance( glyph->GetAdvance() );
                    }
                }
            }
        }
        return font_ok;
    }
};

