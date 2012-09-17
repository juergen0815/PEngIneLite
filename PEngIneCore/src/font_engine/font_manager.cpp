/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "font_manager.h"

#include "font_tools.h"
#include "font_asset.h"
#include "tt_font_asset.h"
#include "FT_font_asset.h"
#include "sfont_asset.h"

#include <boost/thread/mutex.hpp>

#include <string.h>

#if defined (WIN32) || defined (__CYGWIN__)
#include <windows.h>
#endif

namespace pei
{
    static boost::mutex s_AssetMapLock;

    typedef std::map< std::string, boost::shared_ptr<FontAssetCreatorInterface> > FontTypeMap_t;
    static FontTypeMap_t s_FontAssetCreators;

    static FontManager::FontPath_t m_FontSearchPath;

    FontManager::FontManager()
    {
    }

    bool FontManager::RegisterFontAssetForType( const char* type, FontAssetCreatorInterfacePtr creator )
    {
        boost::mutex::scoped_lock lock(s_AssetMapLock);

        FontTypeMap_t::iterator fit = s_FontAssetCreators.find( std::string(type) );
        if ( fit == s_FontAssetCreators.end() ) {
            s_FontAssetCreators[ type ] = creator;
            return true;
        }
        return false;
    }

    pei::FontAssetPtr FontManager::CreateFontAsset( const char* name, size_t size, const char* type /*= "ttf" */ )
    {
        boost::mutex::scoped_lock lock(s_AssetMapLock);

        bfs::path font_name( name );
        std::string ext = font_name.extension();
        if ( ext.size() >= 3 ) {
            ext = ext.substr( ext.size()-3, ext.size() );
            type = ext.c_str();
        }
        // register default factory for ttf and SFonts
        FontTypeMap_t::iterator fit = s_FontAssetCreators.find( "ttf" );
        if ( fit == s_FontAssetCreators.end() ) {
//            s_FontAssetCreators[ "ttf" ] = FontAssetCreatorInterfacePtr(new pei::FontAssetCreator<pei::TtFontAsset>);
            s_FontAssetCreators[ "ttf" ] = FontAssetCreatorInterfacePtr(new pei::FontAssetCreator<pei::FTFontAsset>);
        }
        fit = s_FontAssetCreators.find( "png" );
        if ( fit == s_FontAssetCreators.end() ) {
            s_FontAssetCreators[ "png" ] = FontAssetCreatorInterfacePtr(new pei::FontAssetCreator<pei::SFontAsset>);
        }
        fit = s_FontAssetCreators.find( type );
        if ( fit != s_FontAssetCreators.end() && fit->second ) {
            pei::FontAssetPtr fnt = fit->second->CreateFontAsset( font_name.string().c_str(), size );
            if ( fnt ) fnt->Open();
			return fnt;
        } else {
            if ( strcmp( type, "ttf" ) ) {
            }
            else if ( strcmp( type, "png" ) ) {

            }
        }
        return pei::FontAssetPtr();
    }

    void FontManager::CreateInitialSearchPaths()
    {
#if defined (WIN32) || defined (__CYGWIN__)
            char buffer[256];
            ::GetWindowsDirectoryA( buffer, sizeof(buffer) );

            bfs::path font_path(buffer);
            font_path /= "fonts";

            AddFontPath( font_path.string().c_str(), 5 );
#else
  #if defined ( __MACOSX__ )
            AddFontPath( "/Library/Fonts", 5 );
  #else
   #if defined (__linux__)
            AddFontPath( "/usr/share/fonts/truetype/freefont", 5 );
   #endif
  #endif
#endif
#ifdef __QNXNTO__
        AddFontPath( "/usr/fonts/font_repository/adobe" );
        AddFontPath( "/usr/fonts/font_repository/monotype/" );
#endif
    }


    void FontManager::AddFontPath( const char* font_path, int priority /*= 0 */ )
    {
        m_FontSearchPath.insert( std::pair< int, bfs::path > ( -priority , bfs::system_complete( font_path ) ) );
    }

    FontManager::FontPathIterator FontManager::GetFontPathBegin()
    {
        return m_FontSearchPath.begin();
    }

    FontManager::FontPathIterator FontManager::GetFontPathEnd()
    {
        return m_FontSearchPath.end();
    }
};
