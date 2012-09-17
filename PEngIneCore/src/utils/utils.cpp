/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "utils.h"

#include "string.h"

namespace pei {

std::string trimWhiteSpaces( const char* c, int len )
{
    std::string c_out;
    if ( c )
    {
        int i = 0;
        const char *c_start = c;
        // trim white spaces
        while ( *c_start != '\0' && *c_start == ' ' && i++ < len )
        {
            c_start++;
        }
        len -= i;
        if ( len > 0 )
        {
            const char *c_end = &c_start[len-1];
            while ( c_end != c && *c_end == ' ' )
            {
                c_end--;
            }
            len = (int)(c_end - c_start + 1);
        } else {

        }
        if ( len > 0 ){
            c_out = std::string( c_start , 0, len );
        }
    }
    return c_out;
}

std::string trimWhiteSpaces( const char* c )
{
    return c ? trimWhiteSpaces( c, (int)strlen( c ) ) : "";
}

std::vector<std::string> breakLine( const char* text, size_t max_len /*= ~0UL*/, bool no_trim_ws /*= false*/  )
{
    std::vector<std::string> lines;

    const char* c_start = text;
    while ( *c_start != '\0' )
    {
        const char* c_end = c_start;
        size_t len = 0;
        while ( *c_end != '\0' && *c_end != '\n' && len < max_len ) { len++; c_end++; }
        if ( c_start == c_end ) {
            break;
        }
        if ( no_trim_ws ) {
            lines.push_back( std::string( c_start , 0, (int)(c_end-c_start) ));
        } else {
            lines.push_back( trimWhiteSpaces( c_start, (int)(c_end-c_start) ) );
        }
        c_start = c_end;
    }
    return lines;
}


std::vector<std::string> split( const char* text, const char* pat, bool no_trim_ws /*= false */ )
{
    std::vector<std::string> lines;

    const char* c_start = text;
    while ( *c_start != '\0' && pat )
    {
        const char* c_end = c_start;
        while ( *c_end != '\0' && strncmp( c_end, pat, strlen(pat) ) != 0 ) { c_end++; }
        if ( c_start == c_end ) {
            break;
        }
        if ( no_trim_ws ) {
            std::string s( c_start , 0, (int)(c_end-c_start) );
            if ( s.length() > 0 ) {
                lines.push_back( s );
            }
        } else {
            std::string s( trimWhiteSpaces( c_start, (int)(c_end-c_start) ) );
            if ( s.length() > 0 ) {
                lines.push_back( s );
            }
        }
        if ( *c_end == '\0' ) break;
        c_start = c_end + strlen(pat);
    }
    if ( lines.size() == 0 && strlen(text) > 0 ) {
        if ( no_trim_ws ) {
            lines.push_back( text );
        } else {
            lines.push_back( trimWhiteSpaces( text, strlen(text) ) );
        }
    }
    return lines;
}

#if 0

bool pei::intersectRect( const SDL_Rect& bounds, const SDL_Rect& rect,
                         SDL_Rect& trc )
{
    bool clip_it(false);
    int min_x1(MIN_X(bounds));
    int min_x2(MIN_X(rect));
    int min_y1(MIN_Y(bounds));
    int min_y2(MIN_Y(rect));

    int max_x1(MAX_X(bounds));
    int max_x2(MAX_X(rect));
    int max_y1(MAX_Y(bounds));
    int max_y2(MAX_Y(rect));

    if ( min_x2 >= min_x1 && max_x2 <= max_x1 &&
         min_y2 >= min_y1 && max_y2 <= max_y1 )
    {
        // rect inside!
        trc = rect;
    }
    else if ( ( min_x2 <= max_x1 ) && ( max_x2 >= min_x1 ) &&
              ( min_y2 <= max_y1 ) && ( max_y2 >= min_y1 ) )
    {
        // clip it
        trc.x = (min_x2 >= min_x1) ? min_x2 : min_x1;
        trc.y = (min_y2 >= min_y1) ? min_y2 : min_y1;

        trc.w = ((max_x2 <= max_x1) ? max_x2 : max_x1) - trc.x + 1;
        trc.h = ((max_y2 <= max_y1) ? max_y2 : max_y1) - trc.y + 1;

        clip_it = true;
    } else {
        // out of bounds!
        trc.x = rect.x;
        trc.y = rect.y;
        trc.w = 0;
        trc.h = 0;
        clip_it = true;
    }
    return clip_it;
}

void pei::ClearAlpha( SDL_Surface* buffer, SDL_Surface* brush, int x_pos, int y_pos, bool clear_alpha /*= true */ )
{
    // setup
    int w = brush->w;
    int h = brush->h;
    int x0 = x_pos - w/2;
    int y0 = y_pos - h/2;

    // bounds
    SDL_Rect draw_bounds = { x0, y0, w, h };
    SDL_Rect dst_bounds  = { 0, 0, buffer->w, buffer->h };

    // clip render bounds
    pei::intersectRect( draw_bounds, dst_bounds, dst_bounds );
    if ( dst_bounds.w == 0 || dst_bounds.h == 0 ) {
        // clipped away
        return;
    }
    // clip brush
    SDL_Rect src_bounds = { (x0 < 0 ? w - dst_bounds.w : 0), (y0 < 0 ? h - dst_bounds.h : 0),
        dst_bounds.w, dst_bounds.h };

    Uint32 spitch = brush->pitch;
    Uint32 dpitch = buffer->pitch;
    Uint32 sbpp   = brush->format->BytesPerPixel;
    Uint32 dbpp   = buffer->format->BytesPerPixel;

    Uint32 dAlphaMask  = buffer->format->Amask;

    Uint32 sAlphaMask  = brush->format->Amask;

    // raster operators (per pixel ops - could be a shader, huh ?)
    class PixelOperator
    {
    public:
        virtual Uint32 execute(Uint32 p, Uint32 s) = 0;
    };
    // clear alpha
    class PopClearAlpha : public PixelOperator
    {
        Uint32 mask;
    public:
        PopClearAlpha( Uint32 m ) : mask(m) {}

        virtual Uint32 execute(Uint32 d, Uint32 s)
        {
            return (d & ~mask) | ((~(s & mask)) & (d&mask));
        };
    } clearAlpha(dAlphaMask);
    // set alpha
    class PopSetAlpha : public PixelOperator
    {
        Uint32 mask;
    public:
        PopSetAlpha( Uint32 m ) : mask(m) {}

        virtual Uint32 execute(Uint32 d, Uint32 s)
        {
            return (d & ~mask) | ((s|d) & mask);
        };

    } setAlpha(dAlphaMask);

    // choose raster operator (pixel shader ???)
    PixelOperator *pop;
    if ( clear_alpha == true) {
        pop = &clearAlpha;
    } else {
        pop = &setAlpha;
    }

    // render loop
    int yd = MIN_Y(dst_bounds);
    int ys = MIN_Y(src_bounds);
    for ( ; ys <= MAX_Y(src_bounds) && yd <= MAX_Y(dst_bounds); ys++, yd++ ) {
        int xs = MIN_X(src_bounds);
        int xd = MIN_X(dst_bounds);

        Uint32 *pSrcData = (Uint32*)((char*)brush->pixels + (ys * spitch ) + xs * sbpp);
        Uint32 *pDstData = (Uint32*)((char*)buffer->pixels + (yd * dpitch ) + xd * dbpp);
        for ( ; xs <= MAX_X(src_bounds) && xd <= MAX_X(dst_bounds); xs++, xd++ )
        {
            *pDstData = pop->execute(*pDstData, *pSrcData);
            pDstData++; pSrcData++;
        }
    }

}

void pei::AlphaReduce( SDL_Surface *buffer, double threshhold )
{
    if ( buffer )
    {
        int w = buffer->w;
        int h = buffer->h;

        Uint32 dpitch = buffer->pitch;
        Uint32 alphaMask  = buffer->format->Amask;
        Uint32 alphaShift = buffer->format->Ashift;
        Uint32 alphaLoss  = buffer->format->Aloss;

        bool edge_found(false);
        double adj = 0.25;
        if (SDL_MUSTLOCK( buffer ) ) SDL_LockSurface(buffer);
        for ( int y = 0; y < h; y++ )
        {
            int x = 0;
            Uint32 *pDstData = (Uint32*)((char*)buffer->pixels + (y * dpitch ) + x*4);
            for ( ; x < w; x++ ) {
                Uint32 pixel = *pDstData;
                double alpha = (double)((pixel & alphaMask) >> alphaShift << alphaLoss) / 255.0;
                if ( alpha > 0.0 && alpha <= threshhold ) {
                    // clear alpha!
                    *pDstData = pixel & ~alphaMask;
                }
                pDstData++;
            }
        }
        SDL_UnlockSurface(buffer);
    }
}

void pei::DetectSurfaceEdge( SDL_Surface* s, SDL_Rect &bounds, int x_offset/* = 0*/, int y_offset /*= 0*/ )
{
    // surface has already been created from asset,
    // calc bounding box
    if ( s ) {

        int w = s->w - x_offset;
        int h = s->h - y_offset;

        int minX(x_offset);
        int maxX(w);

        int minY(y_offset);
        int maxY(h);

        int x(x_offset),y(y_offset);
        Uint32 pitch = s->pitch;
        Uint32 alphaMask  = s->format->Amask;
        Uint32 alphaShift = s->format->Ashift;
        Uint32 alphaLoss = s->format->Aloss;
        Uint32 alphaThreshold = 0;

        bool edge_found(false);
        if (SDL_MUSTLOCK( s ) ) SDL_LockSurface(s);
        for ( y = y_offset; y < h && !edge_found; y++ )
        {
            Uint32 *pDstData = (Uint32*)((char*)s->pixels + (y * pitch ));
            for ( x = x_offset; x < w && !edge_found; x++ ) {
                Uint32 p = *pDstData++;
                if ( ((p & alphaMask) >> alphaShift << alphaLoss) > alphaThreshold ) {
                    minY = y;
                    edge_found = true;
                    break;
                }
            }
        }
        edge_found = false;
        maxY = minY;
        for ( y = h-1; y >= minY && !edge_found; y-- )
        {
            Uint32 *pDstData = (Uint32*)((char*)s->pixels + (y * pitch ));
            for ( x = x_offset; x < w && !edge_found; x++ ) {
                Uint32 p = *pDstData++;
                if ( ((p & alphaMask) >> alphaShift << alphaLoss) > alphaThreshold ) {
                    maxY = y;
                    edge_found = true;
                    break;
                }
            }
        }
        edge_found = false;
        for ( x = x_offset; x < w && !edge_found; x++ )
        {
            Uint32 *pDstData = (Uint32*)((char*)s->pixels + x*4);
            for ( y = y_offset; y < h && !edge_found; y++ ) {
                Uint32 p = *pDstData;
                if ( ((p & alphaMask) >> alphaShift << alphaLoss) > alphaThreshold ) {
                    minX = x;
                    edge_found = true;
                    break;
                }
                pDstData = (Uint32*)((char*)pDstData + pitch);
            }
        }
        edge_found = false;
        maxX = minX;
        for ( x = w-1; x >= minX && !edge_found; x-- )
        {
            Uint32 *pDstData = (Uint32*)((char*)s->pixels + x*4);
            for ( y = y_offset; y < h && !edge_found; y++ ) {
                Uint32 p = *pDstData;
                if ( ((p & alphaMask) >> alphaShift << alphaLoss) > alphaThreshold ) {
                    maxX = x;
                    edge_found = true;
                    break;
                }
                pDstData = (Uint32*)((char*)pDstData + pitch);
            }
        }
        SDL_UnlockSurface(s);

        bounds.x = minX;
        bounds.y = minY;
        bounds.w = (maxX > minX) ? ((maxX - minX) + 1) : 0;
        bounds.h = (maxY > minY) ? ((maxY - minY) + 1) : 0;
    }
}

void pei::DrawAlpha( SDL_Surface *alpha_buffer, const SDL_Rect &bounds, SDL_Surface *dest, int dx, int dy )
{
    // surface has already been created from asset,
    // calc bounding box
    if ( dest ) {

        int w = dest->w - (dx+bounds.x) + 1;
        int h = dest->h - (dy+bounds.y) + 1;

        if ( bounds.w < w ) w = bounds.w;
        if ( bounds.h < h ) h = bounds.h;
        if ( w == 0 || h == 0 ) {
            return;
        }
        Sint32 minSx = bounds.x;
        Sint32 minSy = bounds.y;
        Uint32 maxSx = minSx + w - 1;
        Uint32 maxSy = minSy + h - 1;

        Uint32 dpitch = dest->pitch;
        Uint32 spitch = alpha_buffer->pitch;
        Uint32 sbpp   = alpha_buffer->format->BytesPerPixel;
        Uint32 dbpp   = dest->format->BytesPerPixel;
        Uint32 alphaMask  = alpha_buffer->format->Amask;
        Uint32 alphaShift = alpha_buffer->format->Ashift;
        Uint32 alphaLoss  = alpha_buffer->format->Aloss;

        Uint32 red      = pei::XColor( 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE )( dest );
        Uint32 yellow   = pei::XColor( 0xff, 0xff, 0x00, SDL_ALPHA_OPAQUE )( dest );
        Uint32 green    = pei::XColor( 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE )( dest );
        Uint32 mangenta = pei::XColor( 0x00, 0xff, 0xff, SDL_ALPHA_OPAQUE )( dest );

        if (SDL_MUSTLOCK( alpha_buffer ) ) SDL_LockSurface(alpha_buffer);
        if (SDL_MUSTLOCK( dest ) )         SDL_LockSurface(dest);

        int y(minSy);
        for ( ; y <= maxSy; y++ )
        {
            int x = minSx;
            Uint32 *pSrcData = (Uint32*)((char*)alpha_buffer->pixels + (y * spitch ) + bounds.x*sbpp);
            Uint32 *pDstData = (Uint32*)((char*)dest->pixels + ((y+dy) * dpitch ) + (x+dx)*dbpp);
            for ( ; x <= maxSx; x++ ) {
                double p = (double)((*pSrcData++ & alphaMask) >> alphaShift << alphaLoss) / 255.0;
                if ( p > 0.0 && p <= 0.05 ) {
                    *pDstData = mangenta;
                } else if ( p > 0.05 && p <= 0.10 ) {
                    *pDstData = green;
                } else if ( p > 0.10 && p <= 0.40 ) {
                    *pDstData = yellow;
                } else if ( p > 0.40 && p <= 1.00 ) {
                    *pDstData = red;
                }
                pDstData++;
            }
        }
        SDL_UnlockSurface(dest);
        SDL_UnlockSurface(alpha_buffer);
    }
}

SDL_Surface* pei::FillCarret( SDL_Surface* s, Uint32 blocks )
{
    Uint32 pattern[2];

    Uint32 AMask  = s->format->Amask; Uint32 RMask  = s->format->Rmask; Uint32 GMask  = s->format->Gmask; Uint32 BMask  = s->format->Bmask;
    Uint32 AShift = s->format->Ashift;Uint32 RShift = s->format->Rshift;Uint32 GShift = s->format->Gshift;Uint32 BShift = s->format->Bshift;
    Uint32 ALoss  = s->format->Aloss; Uint32 RLoss  = s->format->Rloss; Uint32 GLoss  = s->format->Gloss; Uint32 BLoss  = s->format->Bloss;

    pattern[0] = ( 0x99 >> RLoss << RShift & RMask |
                   0x99 >> GLoss << GShift & GMask |
                   0x99 >> BLoss << BShift & BMask |
                   SDL_ALPHA_OPAQUE >> ALoss << AShift & AMask
                 );
    pattern[1] = ( 0x66 >> RLoss << RShift & RMask |
                   0x66 >> GLoss << GShift & GMask |
                   0x66 >> BLoss << BShift & BMask |
                   SDL_ALPHA_OPAQUE >> ALoss << AShift & AMask
                 );
    Uint32 pidx = 0;
    if (SDL_MUSTLOCK( s ) ) SDL_LockSurface(s);
    for ( int y = 0; y < s->h; y++)
    {
        if (y % blocks == 0) pidx ^= 1;

        Uint32 *pData = (Uint32*)((char*)s->pixels + y * s->pitch);
        Uint32 xidx = pidx;
        for (int x = 0; x < s->w; x++)
        {
            if ( x % blocks == 0) xidx ^= 1;
            *pData++ = pattern[xidx];
        }
    }
    SDL_UnlockSurface(s);
    return s;
}

void pei::DrawBevelBox( SDL_Surface *s, const SDL_Rect &bounds, const XColor& light /*= XC_WHITE*/, const XColor& dark /*= XC_BLACK */ )
{
    Uint32 white = light.operator ()(s);
    Uint32 black = dark.operator ()(s);

    Sint16 x = bounds.x;
    Sint16 y = bounds.y;
    Uint16 w = bounds.w;
    Uint16 h = bounds.h;

    // frame around layer
    { SDL_Rect rc = { x,       y,       w, 1 }; SDL_FillRect( s, &rc, white ); }
    { SDL_Rect rc = { x,       y,       1, h }; SDL_FillRect( s, &rc, white ); }
    { SDL_Rect rc = { x,       y + h-1, w, 1 }; SDL_FillRect( s, &rc, black ); }
    { SDL_Rect rc = { x + w-1, y,       1, h }; SDL_FillRect( s, &rc, black ); }
}

void pei::DrawBevelBox( SDL_Surface *s, const XColor& light /*= XC_WHITE*/, const XColor& dark /*= XC_BLACK */ )
{
    SDL_Rect bounds = { 0, 0, s->w, s->h };
    DrawBevelBox(s, bounds, light, dark );
}

Uint32 pei::WritePixelQuick( SDL_Surface *surface, Sint16 x, Sint16 y, Uint32 color, Uint32 num /*= 1*/ )
{
    Uint32 pitch = surface->pitch;
    Uint32 bpp   = surface->format->BytesPerPixel;

    Uint32 n_written = 0;
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
    if ( y < surface->h ) {
        Uint32 *p = (Uint32*)((char*)surface->pixels + x*bpp + y*pitch);
        while ( x < surface->w && num-- > 0 ) {
            *p++ = color; x++; n_written++;
        }
    }
    SDL_UnlockSurface(surface);
    return n_written;
}

Uint32 pei::WritePixelPop( PixOp &pop, SDL_Surface *surface, Sint16 x, Sint16 y, Uint32 color, Uint32 num /*= 1*/ )
{
    Uint32 pitch = surface->pitch;
    Uint32 bpp   = surface->format->BytesPerPixel;

    Uint32 n_written = 0;
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
    if ( y < surface->h ) {
        Uint32 *p = (Uint32*)((char*)surface->pixels + x*bpp + y*pitch);
        while ( x < surface->w && num-- > 0 ) {
            *p = pop.Compose( color, *p ); x++; n_written++;
            p++;
        }
    }
    SDL_UnlockSurface(surface);
    return n_written;
}

Uint32 pei::ReadPixel( SDL_Surface *surface, Sint16 x, Sint16 y )
{
    Uint32 pitch = surface->pitch;
    Uint32 bpp   = surface->format->BytesPerPixel;

    Uint32 pix = 0;
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
    if ( y < surface->h && x < surface->w ) {
        pix = *((Uint32*)((char*)surface->pixels + x*bpp + y*pitch));
    }
    SDL_UnlockSurface(surface);
    return pix;
}

namespace pei {
    static void DrawCircleSegmenPt( PixOp &pop, SDL_Surface* surface, int xc, int yc, int x1, int y1, int color, int segment ) {

        switch ( segment )
        {
        case 1: // top right segment
            pei::WritePixelQuick( surface, xc + x1, yc - y1, color, 1 );
            pei::WritePixelQuick( surface, xc + y1, yc - x1, color, 1 );
            break;
        case 2: //  bottom right segment
            pei::WritePixelQuick( surface, xc + x1, yc + y1, color, 1 );
            pei::WritePixelQuick( surface, xc + y1, yc + x1, color, 1 );
            break;
        case 3: // bottom left segment
            pei::WritePixelQuick( surface, xc - x1, yc + y1, color, 1 );
            pei::WritePixelQuick( surface, xc - y1, yc + x1, color, 1 );
            break;
        case 4: // top left segment
            pei::WritePixelQuick( surface, xc - x1, yc - y1, color, 1 );
            pei::WritePixelQuick( surface, xc - y1, yc - x1, color, 1 );
            break;
        }
    }

}

void pei::DrawCircle( SDL_Surface* surface, int xc, int yc, double r, Uint32 color )
{
    // printf "Draw a circle @ $xc - $yc, Radius $r with color $color..."
    double x(0),y(r);
    double p = 1-r;

    PixOpAlphaCopy pop(*surface->format,*surface->format);
    do {
        if ( p < 0 ) {
            p += 2*x + 1;
        } else {
            y -= 0.75;
            p += 2*(x - y) + 1;
        }
        x += 0.75;
        pei::DrawCircleSegmenPt( pop, surface, xc, yc, (int)x, (int)y, color, 1 );
        pei::DrawCircleSegmenPt( pop, surface, xc, yc, (int)x, (int)y, color, 2 );
        pei::DrawCircleSegmenPt( pop, surface, xc, yc, (int)x, (int)y, color, 3 );
        pei::DrawCircleSegmenPt( pop, surface, xc, yc, (int)x, (int)y, color, 4 );
    } while ( x < y );
}

void pei::DrawCircleQuad( SDL_Surface* surface, int xc, int yc, double r, Uint32 color, int segment )
{
    double x(0),y(r);
    double p = 1-r;

    PixOpAlphaCopy pop(*surface->format,*surface->format);
    do {
        pei::DrawCircleSegmenPt( pop, surface, xc, yc, (int)x, (int)y, color, segment );
        if ( p < 0 ) {
            p += 2*x + 1;
        } else {
            y -= 0.5;
            p += 2*(x - y) + 1;
        }
        x += 0.5;
    } while ( x <= y );
}

void pei::Blitter::ScaleBlit( SDL_Surface *src, SDL_Surface *dst, Sint32 x_pos, Sint32 y_pos, Uint32 scale_width, Uint32 scale_height, bool keep_aspect /*= false */ )
{
    // render bounds is withing the destination bitmap
    SDL_Rect render_bounds = { 0, 0, dst->w, dst->h };
    // that's the rect we want to blit into
    SDL_Rect dst_bounds    = { x_pos, y_pos, scale_width, scale_height };

    double scale_x = (double)scale_width / (double)src->w;
    double scale_y = (double)scale_height / (double)src->h;
    // clip render bounds
    if ( pei::intersectRect( render_bounds, dst_bounds, dst_bounds ) )
    {
        if ( dst_bounds.w == 0 || dst_bounds.h == 0 )
        {
            // off screen!
            return;
        }
        //             // adjust src rectangle if clipped
        //             if ( px < 0 ) src_bounds.x = src_bounds.w - dst_bounds.w;
        //             if ( py < 0 ) src_bounds.y = src_bounds.h - dst_bounds.h;
        //             src_bounds.w = dst_bounds.w;
        //             src_bounds.h = dst_bounds.h;
    }
    SDL_Rect src_bounds = { 0, 0, src->w, src->h };

    Uint32 spitch = src->pitch;
    Uint32 dpitch = dst->pitch;
    Uint32 sbpp   = src->format->BytesPerPixel;
    Uint32 dbpp   = dst->format->BytesPerPixel;

    if ( SDL_MUSTLOCK( src ) ) SDL_LockSurface( src );
    if ( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );

    // render loop
    int ys = MIN_Y(src_bounds);
    double yd = (double)MIN_Y(dst_bounds);
    for ( ; ys <= MAX_Y(src_bounds) && (int)yd <= MAX_Y(dst_bounds); ys++, yd += scale_y ) {
        int xs = MIN_X(src_bounds);
        double xd = (double)MIN_X(dst_bounds);

        Uint32 *pSrcData = (Uint32*)((char*)src->pixels + (ys * spitch ) + xs * sbpp);
        for ( ; xs <= MAX_X(src_bounds) && (int)xd <= MAX_X(dst_bounds); xs++, xd += scale_x )
        {
            Uint32 *pDstData = (Uint32*)((char*)dst->pixels + ((int)yd * dpitch ) + (int)xd * dbpp);
            *pDstData = m_PixOp->Compose( *pSrcData++, *pDstData );
        }
    }
    SDL_UnlockSurface( dst );
    SDL_UnlockSurface( src );
}

SDL_Surface * pei::CreateSurfaceFromBrush( const Brush& brush )
{
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        (void*)brush.pixel_data,
        brush.width, brush.height,
        brush.bytes_per_pixel * 8, brush.width * brush.bytes_per_pixel,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 );
    return surface;
}

SDL_Surface* pei::CreateSurface( Uint16 w, Uint16 h )
{
    SDL_Surface *s = SDL_CreateRGBSurface( SDL_SWSURFACE,
                                           w, h, 32,
                                           0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
    if ( s )
    {
        SDL_Surface *ts = s;
        s = SDL_DisplayFormatAlpha( ts );
        SDL_FreeSurface(ts);
    }
    return s;

}

SDL_Surface* pei::CreateSurface( Uint16 w, Uint16 h, const XColor &fill_color )
{
    SDL_Surface *s = pei::CreateSurface( w, h );
    if ( s )
    {
        Uint32 flags = s->flags;
        SDL_SetAlpha( s, 0, SDL_ALPHA_OPAQUE ); // switch off alpha because we fill color incl. alpha!
        SDL_Rect rc = { 0, 0, w, h };
        SDL_FillRect( s, &rc, fill_color(s) );
        SDL_SetAlpha( s, flags, fill_color.xalpha );
    }
    return s;

}

SDL_Surface* pei::CreateSurfaceFromPixelDescriptor( const pei::VideoAsset::Pixmap *pd )
{
    SDL_Surface *s = NULL;
    if ( pd && pd->pixels )
    {
        if ( pd->format->m_type == pei::VideoAsset::TYPE_YV12 ) {
            // to do: add some YUV conversion code here.
            // this allows an asset to be used with video overlay
            // we here run a windowed system and need to convert to RGB, though
            // SDL cannot create a buffered YUV overlay window (must be on screen)
        } else {
            s = SDL_CreateRGBSurfaceFrom(  const_cast<void*>(pd->pixels),
                pd->format->m_width, pd->format->m_height,
                pd->format->m_depth, pd->format->m_pitch,
                pd->format->m_rmask,
                pd->format->m_gmask,
                pd->format->m_bmask,
                pd->format->m_amask);

            // color keyed data - not supported yet (always RGBA32)
            if ( s ) {
                //                 SDL_SetColorKey(surface, (SDL_SRCCOLORKEY/*|SDL_RLEACCEL*/),
                //                 SDL_MapRGB( surface->format,
                //                             pd->ColorKey.red, pd->ColorKey.green, pd->ColorKey.blue ) );
            }
        }
    }
    return s;
}

void pei::ConvertPixelFormat2SDL( const VideoAsset::Format &pd, SDL_PixelFormat& format )
{
    memset ( &format, 0, sizeof( SDL_PixelFormat ) );
    format.Amask = pd.m_amask;
    format.Rmask = pd.m_rmask;
    format.Gmask = pd.m_gmask;
    format.Bmask = pd.m_bmask;

    format.Ashift = pd.m_ashift;
    format.Rshift = pd.m_rshift;
    format.Gshift = pd.m_gshift;
    format.Bshift = pd.m_bshift;

    format.BitsPerPixel  = pd.m_depth;
    format.BytesPerPixel = pd.m_bytes_per_pixel;
}

#endif

}
