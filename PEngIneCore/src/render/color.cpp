/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <render/color.h>

#include <string.h>

using namespace pei;

static Color DEFAULT_COLOR;

/** @short Default constructor, black with a 255 */
pei::Color::Color( ) :r(0), g(0), b(0), a(PIXF_OPAQUE) { }

pei::Color::Color( const Color& other )
    : r(other.r)
	, g(other.g)
    , b(other.b)
	, a(other.a)
{ }

/** @short Copy constructor from an integer already holding the color value */
pei::Color::Color(unsigned int col) : m_combined(col) { }

/** @short Constructor with all four color components */
pei::Color::Color( int _r, int _g, int _b, int _a)
	: r(_r), g(_g), b(_b), a(_a)
{}

// should clamp to 0.0-1.0
pei::Color::Color( double _r, double _g, double _b, double _a /*= 1.0 */ )
: r((unsigned char)(_r*255.0))
, g((unsigned char)(_g*255.0))
, b((unsigned char)(_b*255.0))
, a((unsigned char)(_a*255.0))
{
}

pei::Color::Color( const float* c, int n )
{
    switch (n)
    {
    case 4:
        a = c[3] * 255;
        // fall through
    case 3:
        b = c[2] * 255;
        g = c[1] * 255;
        r = c[0] * 255;
        break;
    default: break;
    }
}

pei::Color::Color( const float c[4] )
{
    a = c[3] * 255;
    b = c[2] * 255;
    g = c[1] * 255;
    r = c[0] * 255;
}


//////////////////////////////////////////////////////////////////////////
//

pei::Palette::Palette( int num_colors /*= 256*/ )
: m_num_colors( num_colors )
{
	m_colors = new Color[ m_num_colors ];
	Color default_color;
	memset( m_colors, default_color, m_num_colors * sizeof( Color ) );
}

pei::Palette::Palette( const Palette& other )
: m_num_colors( other.m_num_colors )

{
	m_colors = new Color[ m_num_colors ];
	memcpy ( m_colors, other.m_colors , m_num_colors * sizeof( Color ) );
}

pei::Palette::~Palette() {
	if ( m_colors ) {
		delete m_colors;
	}
}

int pei::Palette::GetNumberOfColors() const
{
	return m_num_colors;
}

pei::Color& pei::Palette::GetColor( int i ) const
{
	if ( i < m_num_colors ) {
		return m_colors[ i ];
	}
	return DEFAULT_COLOR;
}

void pei::Palette::SetColor( int i, const Color& c )
{
	if ( i < m_num_colors ) {
		m_colors[ i ] = c;
	}
}

