/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef TT_FONT_FACTORY_H
#define TT_FONT_FACTORY_H

#include <boost/shared_ptr.hpp>

namespace pei
{
    class TtFont
    {
    public:
        virtual ~TtFont() {}

        virtual void* GetFontHandle() = 0;
    };
    typedef boost::shared_ptr<TtFont> TtFontPtr;

    class TtFontFactory
    {
    protected:
        static void DestroyFontList();

        virtual void CloseTtFont( TtFontPtr font );
    public:
        TtFontFactory();

        virtual ~TtFontFactory();

        virtual int Init();

        virtual TtFontPtr CreateTtFont( const char* font, int size );
    protected:
        virtual TtFontPtr FindTtFont(const char* font, int size);

        virtual TtFontPtr OpenTtFont(const char* font, int size);
};
};

#endif // TT_FONT_FACTORY_H
