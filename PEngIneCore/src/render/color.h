/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_COLOR_H
#define RENDER_COLOR_H

#include <boost/shared_ptr.hpp>

namespace pei
{
	class Color
	{
	public:
		enum enCONSTS
		{
			PIXF_OPAQUE     = 255,
			PIXF_TRANPARENT = 0,
		};
	public:
		/** @short Default constructor, black with a 255 */
		Color( );

		Color( const Color& color );

		/** @short Copy constructor from an integer already holding the color value */
		Color( unsigned int col);

		/** @short Constructor with all four color components - needs to be int to avoid abiguity with double */
		Color( int r, int g, int b, int a = PIXF_OPAQUE);

        /** @short Constructor with all four color components */
        Color(double r, double g, double b, double a = 1.0 );

        Color( const float* c, int n );

        Color( const float c[4] );

        /** Automatic cast to an unsigned long word */
		inline operator unsigned int() const { return m_combined; }

		/** Add two Color together */
		inline Color operator+(const Color &c) const
		{
			return Color((unsigned char)(((int)r + (int)c.r) & 0xff),
                         (unsigned char)(((int)g + (int)c.g) & 0xff),
                         (unsigned char)(((int)b + (int)c.b) & 0xff),
                         (unsigned char)(((int)a + (int)c.a) & 0xff));
		}

		/** Subtract two Color together */
		inline Color operator-( const Color &c) const
		{
			return Color((unsigned char)(r - c.r),
                         (unsigned char)(g - c.g),
                         (unsigned char)(b - c.b),
                         (unsigned char)(a - c.a));
		}

		/** Add two Color together */
		inline Color& operator+=(const Color &c)
		{
			r = ((int)r + (int)c.r) & 0xff;
            g = ((int)g + (int)c.g) & 0xff;
            b = ((int)b + (int)c.b) & 0xff;
            a = ((int)a + (int)c.a) & 0xff;
            return *this;
		}

		/** Subtract two Color together */
		inline Color& operator-=( const Color &c)
		{
			r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this;
		}

        double R() const { return (double)r/255.0; }
        double G() const { return (double)g/255.0; }
        double B() const { return (double)b/255.0; }
        double A() const { return (double)a/255.0; }

        Color& R( double dr ) { r = (unsigned char)(dr * 255.0); return *this; }
        Color& G( double dg ) { g = (unsigned char)(dg * 255.0); return *this; }
        Color& B( double db ) { b = (unsigned char)(db * 255.0); return *this; }
        Color& A( double da ) { a = (unsigned char)(da * 255.0); return *this; }

		public:
            // might switch to double or float for float frame buffer!
			union
			{
				struct // RGBA color - goes straight into RGBA texture!
				{
                    unsigned char r;
					unsigned char g;
                    unsigned char b;
					unsigned char a;
				};
				unsigned char m_components[4];
				unsigned int  m_combined;
			};

		protected:

	};
	typedef boost::shared_ptr<Color> ColorPtr;

    // some pre defined colors
    #define COL_WHITE    Color(255,255,255)
    #define COL_BLACK    Color(0,0,0)
    #define COL_RED      Color(255,0,0)
    #define COL_GREEN    Color(0,255,0)
    #define COL_BLUE     Color(0,0,255)
    #define COL_YELLOW   Color(255,255,0)
    #define COL_CYAN     Color(255,0,255)
    #define COL_MANGENTA Color(0,255,255)

	class Palette
	{
	public:
		Palette( int num_colors = 256 );

		Palette( const Palette& other );

		~Palette();

		int GetNumberOfColors() const;

		Color& GetColor( int i ) const;

		void SetColor( int i, const Color& c );

	protected:
		/** The colors of the palette */
		int    m_num_colors;
		Color *m_colors;
	};
	typedef boost::shared_ptr<Palette> PalettePtr;

} // namespace pei


#endif
