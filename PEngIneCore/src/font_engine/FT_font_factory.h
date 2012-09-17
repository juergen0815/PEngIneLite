/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef FT_FONT_FACTORY_H
#define FT_FONT_FACTORY_H

#include <boost/shared_ptr.hpp>

#include <font_engine/tt_font_factory.h>

namespace pei
{
    class FtFontFactoryPrivate;

    class FtFontFactory : public TtFontFactory
    {
    protected:
        FtFontFactoryPrivate *m_Private;
    public:
        FtFontFactory();

        virtual ~FtFontFactory();

        virtual int Init();

    protected:
        virtual TtFontPtr OpenTtFont(const char* font, int size);
    };
};

#endif // FT_FONT_FACTORY_H
