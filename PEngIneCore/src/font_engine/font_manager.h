/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "font_asset.h"

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;

namespace pei
{
    class FontAsset;

    class FontAssetCreatorInterface;
    typedef boost::shared_ptr<class FontAssetCreatorInterface> FontAssetCreatorInterfacePtr;

    class FontAssetCreatorInterface
    {
    public:
        FontAssetCreatorInterface() {}

        virtual ~FontAssetCreatorInterface() {}

        virtual pei::FontAssetPtr CreateFontAsset( const char* name, size_t size ) = 0;
    };

    template < class FONT_ASSET_TYPE >
    class FontAssetCreator : public FontAssetCreatorInterface
    {
    public:
        virtual pei::FontAssetPtr CreateFontAsset( const char* name, size_t size )
        { return pei::FontAssetPtr(new FONT_ASSET_TYPE( name, size )); }
    };

    class FontManager
    {
    public:
        typedef std::multimap< int, bfs::path >   FontPath_t;
        typedef FontPath_t::iterator              FontPathIterator;

        FontManager();

        ~FontManager() {}

        void CreateInitialSearchPaths();

        bool RegisterFontAssetForType( const char* type, FontAssetCreatorInterfacePtr creator );

        pei::FontAssetPtr CreateFontAsset( const char* name, size_t size, const char* type = "ttf" );

        void AddFontPath( const char* font_path, int priority = 0 );

        FontManager::FontPathIterator GetFontPathBegin();

        FontManager::FontPathIterator GetFontPathEnd();
    };

    #define DECLARE_FNT_ASSET_CREATOR( KEY, CLASS ) \
    struct Create_##KEY   \
    {                   \
        Create_##KEY() {  \
        FontManager().RegisterFontAssetForType( #KEY, new pei::FontAssetCreator<CLASS>() ); \
        }               \
    };                  \
    static Create_##KEY __Creator_##KEY;

};

#endif // FONT_MANAGER_H
