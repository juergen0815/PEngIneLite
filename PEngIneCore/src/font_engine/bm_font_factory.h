/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef BM_FONT_FACTORY_H
#define BM_FONT_FACTORY_H

#include <render/color.h>
#include <render/rectangle.h>
#include <render/render_surface.h>

#include <map>

#include <boost/shared_ptr.hpp>

namespace pei
{
    class Glyph
    {
    public:
        int          character;     // character itself
        Rectangle    dimension;     // character dimensions (pos & size) within font strip
        SurfacePtr   surface;       // look up to surface within font strip
        int          advance;
        int          left;          // left offset within glyph (added to dim.x())
        int          top;           // top offset

        Glyph( int c = 0, const pei::Rectangle& d = pei::Rectangle(), const SurfacePtr& s = SurfacePtr() )
            : character(c), dimension(d), surface(s)
            , advance(d.w())
            , left(0)
            , top(d.h())
        { }

        Glyph( const Glyph& g )
        : character( g.character )
        , dimension( g.dimension )
        , surface( g.surface )
        , advance( g.advance )
        , left( g.left )
        , top( g.top )
        {}

        virtual ~Glyph() {}

        // advance in x direction
        virtual int GetAdvance() const { return advance; }

        virtual void SetAdvance( int adv ) { advance = adv; }
    };

    class Font
    {
    protected:
        typedef std::map< int, Glyph > CharMap;

        CharMap        m_CharMap;
        SurfacePtr     m_FontStrip;
        int            m_DefaultAdvance; // for space or missing characters
    public:
        Font();

        virtual ~Font();

        bool IsEmpty() { return m_CharMap.size() == 0; }

        /** read a glyph from font map - this contains the fontstrip itself and the
         *  position and character size within that strip */
        const Glyph* GetGlyph( int character ) const;

        /** attach an existing font strip */
        virtual void SetFontStrip( SurfacePtr font_surface );

        /** add a glyph into an existing font strip */
        virtual void AddGlyph( const Glyph& glyph );

        virtual void AddGlyph( int _char, const pei::Rectangle& _dim, const SurfacePtr& surface );

        virtual int GetDefaultAdvance() const;

        virtual void SetDefaultAdvance( int adv );

        virtual const SurfacePtr GetFontStrip() const;
    };

    typedef boost::shared_ptr<Font> FontPtr;

    class FontFactory
    {
    public:
        FontFactory();

        virtual ~FontFactory();

        int Init();

        FontPtr CreateFont( const char* font, int size );

        void CloseFont( FontPtr font );
    protected:
        static void DestroyFontList();
    };
};

#endif // BM_FONT_FACTORY_H
