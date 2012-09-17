/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef COLOR_H
#define COLOR_H

#include <video/pix_op.h>

#include <render/rectangle.h>
#include <render/render_surface.h>

namespace pei
{
    inline double ClampTop( double v, double threshold = 1.0 )
    {
        if ( v > threshold ) { v = threshold; }
        return v;
    }

    inline double ClampBottom( double v, double threshold = 0.0 )
    {
        if ( v < threshold ) { v = threshold; }
        return v;
    }

    // multiply with constant - saturate
    inline Color& operator*=( Color& c, double sat )
    {
        c.R( ClampTop(c.R()*sat) );
        c.G( ClampTop(c.G()*sat) );
        c.B( ClampTop(c.B()*sat) );
        return c;
    }

    inline Color operator*( const Color& c, double sat )
    {
        Color col(c);
        return operator*=(col, sat );
    }

    // multiply with constant - de-saturate
    inline Color& operator/=( Color& c, double sat )
    {
        if ( sat > 0 ) {
            c.R( ClampBottom(c.R()/sat) );
            c.G( ClampBottom(c.G()/sat) );
            c.B( ClampBottom(c.B()/sat) );
        }
        return c;
    }

    inline Color operator/( const Color& c, double sat )
    {
        Color col(c);
        return operator/=(col, sat );
    }

    // multiply with constant - saturate
    inline Color& operator+=( Color& c, double val )
    {
        c.R( ClampTop(c.R()+val) );
        c.G( ClampTop(c.G()+val) );
        c.B( ClampTop(c.B()+val) );
        return c;
    }

    inline Color operator+( const Color& c, double val )
    {
        Color col(c);
        return operator+=(col, val );
    }

    // multiply with constant - de-saturate
    inline Color& operator-=( Color& c, double val )
    {
        c.R( ClampBottom(c.R()-val) );
        c.G( ClampBottom(c.G()-val) );
        c.B( ClampBottom(c.B()-val) );
        return c;
    }

    inline Color operator-( const Color& c, double val )
    {
        Color col(c);
        return operator/=(col, val );
    }
    inline Color& ToGray( Color& col ) {
        // separate luma and "normalize" rgb
        double Y = (double)(0.2126 * col.R() + 0.7152 * col.G() + 0.0722 * col.B());
        Y = ClampTop(ClampBottom(Y));
        col.R(Y);
        col.G(Y);
        col.B(Y);
        return col;
    }

    inline Color AsGray( const Color& col )
    {
        Color c(col);
        return ToGray(c);
    }


} /* namespace pei */
#endif /* COLOR_H */
