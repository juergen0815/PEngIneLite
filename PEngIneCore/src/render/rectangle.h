/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

namespace pei
{

#define MIN_X( rc ) ( (int)rc.x() )
#define MIN_Y( rc ) ( (int)rc.y() )
#define MAX_X( rc ) ( (int)(rc.x() + rc.w() - 1))
#define MAX_Y( rc ) ( (int)(rc.y() + rc.h() - 1))

class Rectangle
{
protected:
	double m_X,     m_Y;
	double m_Width, m_Height;
public:
	Rectangle() : m_X(0), m_Y(0), m_Width(0), m_Height(0) {}
    Rectangle( double w, double h ) : m_X(0), m_Y(0), m_Width(w), m_Height(h) {}
	Rectangle( double x, double y, double w, double h ) : m_X(x), m_Y(y), m_Width(w), m_Height(h) {}
	Rectangle( const Rectangle& o ) : m_X(o.m_X), m_Y(o.m_Y), m_Width(o.m_Width), m_Height(o.m_Height) {}

	virtual ~Rectangle() {}

	inline double& x() { return m_X; }
	inline double& y() { return m_Y; }
	inline double& w() { return m_Width; }
	inline double& h() { return m_Height; }

	inline double x() const { return m_X; }
	inline double y() const { return m_Y; }
	inline double w() const { return m_Width; }
	inline double h() const { return m_Height; }

    inline double minX() const { return m_X; }
    inline double minY() const { return m_Y; }

    inline double maxX() const { return m_X + m_Width - 1; }
    inline double maxY() const { return m_Y + m_Height - 1; }

	// node compatible interface
    inline double GetXPosition() const { return m_X; }
    inline double GetYPosition() const { return m_Y; }

	inline double GetWidth() const { return m_Width; }
    inline double GetHeight() const { return m_Height; }

	inline Rectangle operator+( const Rectangle& r ) const {
        return Rectangle( *this ) += r;
	}

    inline Rectangle& operator+=( const Rectangle& r ) {
        double x1 = maxX() > r.maxX() ? maxX() : r.maxX();
        double y1 = maxY() > r.maxY() ? maxY() : r.maxY();
        double x0 = m_X < r.m_X ? m_X : r.m_X;
        double y0 = m_Y < r.m_Y ? m_Y : r.m_Y;

        m_X = x0; m_Y = y0; m_Width = (x1-x0-1); m_Height = (y1-y0-1);
        return *this;
    }

    inline Rectangle& operator+=( double d ) {
        m_X -= d;
        m_Y -= d;
        m_Width  += 2*d;
        m_Height += 2*d;
        return *this;
    }

    inline Rectangle operator+=( double d ) const {
        return Rectangle(*this) += d;
    }

    inline Rectangle& operator-=( double d ) {
        return Rectangle::operator+=(-d);
    }

    inline Rectangle& operator-=( double d ) const {
        return Rectangle(*this) += -d;
    }

    inline Rectangle& operator--( ) {
        return Rectangle::operator-=(1);
    }

    inline Rectangle& operator++( ) {
        return Rectangle::operator+=(1);
    }

    bool operator<( const Rectangle& r ) const {
        return GetWidth() < r.GetWidth() && GetHeight() < r.GetHeight();
    }

	inline bool IsInside( double x, double y ) {
		return (x >= minX()) && (x <= maxX())
		    && (y >= minY()) && (y <= maxY());
	}
};

bool intersectRect( const Rectangle& bounds, const Rectangle& rect, Rectangle& trc );

inline bool isOffBounds( const Rectangle& bounds, const Rectangle& rect )
{
    return ( rect.maxX() < bounds.minX()   ||
             rect.maxY() < bounds.minY()   ||
             rect.minX() > bounds.maxX() ||
             rect.minY() > bounds.maxY() );
}

inline bool ClipBounds( const Rectangle& render_bounds, Rectangle &src_bounds, Rectangle& dst_bounds )
{
    // render bounds is withing the destination bitmap
    // adjust dest bounds - can never be bigger than src bounds
    dst_bounds.w() = src_bounds.w();
    dst_bounds.h() = src_bounds.h();
    // remember dest position - will be overridden by clipper
    int px = (int)dst_bounds.x();
    int py = (int)dst_bounds.y();

    // clip render bounds
    if ( pei::intersectRect( render_bounds, dst_bounds, dst_bounds ) )
    {
        if ( dst_bounds.w() == 0 || dst_bounds.h() == 0 )
        {
            // off screen!
            return true;
        }
        // adjust src rectangle if clipped
        if ( px < 0 ) src_bounds.x() = src_bounds.w() - dst_bounds.w();
        if ( py < 0 ) src_bounds.y() = src_bounds.h() - dst_bounds.h();
        src_bounds.w() = dst_bounds.w();
        src_bounds.h() = dst_bounds.h();
    }
    return false;
}

} /* namespace pei */
#endif /* RECTANGLE_H_ */
