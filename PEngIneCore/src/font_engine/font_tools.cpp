/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "font_tools.h"
#include "font_manager.h"

namespace pei
{
    pei::FontAssetPtr CreateFontAsset( const char* name, size_t size, char *type /*= "ttf"*/ )
    {
        return FontManager().CreateFontAsset( name, size, type );
    }
};
