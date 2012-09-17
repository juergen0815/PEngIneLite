/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "FT_font_asset.h"

#include <decoder/dec_video.h>

#include <font_engine/font_manager.h>

#include <decoder/dec_video_png_image.h>

#include <video/render_tools.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;

namespace pei
{
    extern FT_Library s_FtLibrary;

    FTFontAsset::FTFontAsset( const char* name, size_t size )
        : BmFontAsset(name, size)
    {
        bfs::path fontName( name );
        if ( fontName.extension().size() < 3 ) {
            m_FontName += GetFormatString();
        }
    }

    pei::Rectangle FTFontAsset::GetTextDimensions( const char* text ) const
    {
        // collect all glyphs from font asset
        Rectangle rc;
        if ( m_Font && !m_Font->IsEmpty() ) {
            const char *c = text;
            while ( *c != '\0') {
                // TODO: add ascent and descent
                const Glyph* glyph = m_Font->GetGlyph( (int)*c );
                if ( glyph ) {
                    int h = glyph->dimension.y() + glyph->dimension.h();
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

    bool FTFontAsset::GenerateFontStrip( pei::FontPtr font )
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
                bool noGlobalLibrary(s_FtLibrary == NULL);
                // not thread safe! Might have been initialized by FtFontFactory
                if ( noGlobalLibrary ) {
                    FT_Error error = FT_Init_FreeType( &s_FtLibrary );
                    if ( error ) {
                        std::cerr << "Error loading Freetype library\n" << std::endl;
                        return false;
                    }
                }
                FT_Face face;
                if (FT_New_Face(s_FtLibrary, full_file_name.string().c_str(),0,&face)) {
                    std::cerr << "Error loading font " << full_file_name.string().c_str()  << std::endl;
                    return false;
                }

                if(FT_Set_Char_Size ( face, 0, m_FontSize * 64, 0, 0)) {
                    std::cerr << "Error initializing character parameters" << std::endl;
                    return false;
                }

//                if(FT_Set_Pixel_Sizes ( face, 0, m_FontSize )) {
//                    std::cerr << "Error initializing character parameters" << std::endl;
//                    return false;
//                }

                // TODO: Rewrite this


                int c;
                int i, j;
                FT_GlyphSlot slot;
                FT_Bitmap    bmp;

                int bufferWidth(0);
                int bufferHeight(0);
                //First calculate the max width and height of a character in a passed font
                for(c = 0; c < 128; c++) {
                    if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                        // ignore error, skip char
                        continue;
                    }

                    slot = face->glyph;
                    bmp  = slot->bitmap;

                    bufferWidth += bmp.width;
                    if ( bufferHeight < bmp.rows ) bufferHeight = bmp.rows;
                }

                // TODO: Change to Luma+Alpha surface (requires Blitter/PixOp Update!!)
                pei::Format fmt(bufferWidth, bufferHeight);
                pei::SurfacePtr fontStrip( new pei::Surface(fmt) );
                unsigned int* fontPixels = (unsigned int*)fontStrip->GetPixels();

                pei::Rectangle glyphDims;
                int x(0), y(0);
                // Fill font texture bitmap with individual bmp data and record appropriate size, texture coordinates and offsets for every glyph
                for(c = 0; c < 128; c++) {
                    if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                        // ignore error, skip char
                        continue;
                    }
                    slot = face->glyph;
                    bmp  = slot->bitmap;

                    y = bufferHeight - bmp.rows;
                    glyphDims.x() = x;
                    glyphDims.w() = bmp.width;
                    glyphDims.y() = y;
                    glyphDims.h() = bmp.rows;

                    for (j = 0; j < bmp.rows; j++) {
                        for (i = 0; i < bmp.width; i++) {
                            // TODO: convert to Luma+Alpha surface
                            // extract pixel into RGBA value
                            unsigned char level = bmp.buffer[i + bmp.width * j];
                            unsigned char pix = level == 0 ? 0 : 0xff;
                            fontPixels[ (x + i) + (y + j) * bufferWidth ] = pei::Color( pix, pix, pix, level );
                        }
                    }
                    x += bmp.width;
                    // TODO: Glyph needs more info for advance FT font setup (kerning, etc)
                    // add a glyph for this character
                    Glyph glyph( c, glyphDims, fontStrip );
                    glyph.advance = (slot->advance.x >> 6);
                    glyph.left    = slot->bitmap_left;
                    glyph.top     = slot->bitmap_top;
                    // glyph.top  = ((slot->metrics.horiBearingY-face->glyph->metrics.height) >> 6);
                    font->AddGlyph( glyph );
                }
                // attach the font strip
                font->SetFontStrip( fontStrip );
                // TODO: use a space instead
                const Glyph* glyph = font->GetGlyph( (int)'!' );
                if ( !glyph ) {
                    // TODO: In SFont this is calculated in CharPos[2] - CharPos[1]
                    font->SetDefaultAdvance( 4 );
                } else {
                    font->SetDefaultAdvance( glyph->GetAdvance() );
                }

                FT_Done_Face(face);
                if ( noGlobalLibrary ) {
                    FT_Done_FreeType(s_FtLibrary);
                    s_FtLibrary = NULL;
                }
                font_ok = true;
            }
        }
        return font_ok;
    }
};

