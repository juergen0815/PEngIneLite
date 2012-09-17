/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef FONT_TOOLS_H
#define FONT_TOOLS_H

#include "font_asset.h"

#include <boost/shared_ptr.hpp>

namespace pei
{
    class FontAsset;

    pei::FontAssetPtr CreateFontAsset( const char* name, size_t size, char* type = "ttf" );
};

#endif // FONT_TOOLS_H
