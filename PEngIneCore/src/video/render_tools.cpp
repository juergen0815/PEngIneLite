/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <video/render_tools.h>

namespace pei {

	unsigned int MapColor( const pei::Format& format, const pei::Color& col )
    {
		unsigned int AMask  = format.m_AMask; unsigned int RMask  = format.m_RMask; unsigned int GMask  = format.m_GMask; unsigned int BMask  = format.m_BMask;
		unsigned int AShift = format.m_AShift;unsigned int RShift = format.m_RShift;unsigned int GShift = format.m_GShift;unsigned int BShift = format.m_BShift;
		unsigned int ALoss  = format.m_ALoss; unsigned int RLoss  = format.m_RLoss; unsigned int GLoss  = format.m_GLoss; unsigned int BLoss  = format.m_BLoss;

        return ( (((col.r >> RLoss) << RShift) & RMask) |
                 (((col.g >> GLoss) << GShift) & GMask) |
                 (((col.b >> BLoss) << BShift) & BMask) |
                 (((col.a >> ALoss) << AShift) & AMask) );
    }

    void FillCarret( SurfacePtr s, size_t blocks )
    {
    	if ( s ) {
			const pei::Format& format = s->GetFormat();
			unsigned int pattern[] = { MapColor( format, pei::Color(0x99, 0x99, 0x99, pei::Color::PIXF_OPAQUE) ),
									   MapColor( format, pei::Color(0x66, 0x66, 0x66, pei::Color::PIXF_OPAQUE) ) } ;
			unsigned int pidx = 0;
			s->Lock();
			char* pixels = (char*)s->GetPixels();
			if  (pixels) {
				unsigned int pitch = s->GetPitch();
				unsigned int w = s->GetWidth();
				unsigned int h = s->GetHeight();
				for ( unsigned int y = 0; y < h; y++)
				{
					if (y % blocks == 0) pidx ^= 1;

					unsigned int *pData = (unsigned int*)(pixels + y * pitch);
					unsigned int xidx = pidx;
					for (unsigned int x = 0; x < w; x++)
					{
						if ( x % blocks == 0) xidx ^= 1;
						*pData++ = pattern[xidx];
					}
				}
			}
			s->Unlock();
    	}
    }

	void Fill32( SurfacePtr s, const Color& color )
	{
		if ( s )
		{
			const pei::Format& format = s->GetFormat();
			unsigned int col =  MapColor( format, color );
			s->Lock();
			char* pixels = (char*)s->GetPixels();
			if ( pixels && format.GetBytesPerPixel() == 4 ) {
				unsigned int pitch = s->GetPitch();
				unsigned int w = s->GetWidth();
				unsigned int h = s->GetHeight();
				for ( unsigned int y = 0; y < h; y++)
				{
					unsigned int *pData = (unsigned int*)(pixels + y * pitch);
					for (unsigned int x = 0; x < w; x++)
					{
						*pData++ = col;
					}
				}
			}
			s->Unlock();
		}
	}

	void Fill32( SurfacePtr s, const Color& color,const pei::Rectangle& bounds )
	{
		if ( s )
		{
			const pei::Format& format = s->GetFormat();
			unsigned int col =  MapColor( format, color );
			s->Lock();
			char* pixels = (char*)s->GetPixels();
			if (pixels && format.GetBytesPerPixel() == 4) {
				unsigned int pitch = s->GetPitch();
				int x0 = (int)bounds.x();
				int y0 = (int)bounds.y();
				int x1 = (int)bounds.maxX();
				int y1 = (int)bounds.maxY();
				if ( x1 > (int)s->GetWidth()  ) x1 = (int)s->GetWidth();
				if ( y1 > (int)s->GetHeight() ) y1 = (int)s->GetHeight();
				if ( x0 < 0 ) x0 = 0;
				if ( y0 < 0 ) y0 = 0;
				for ( int y = y0; y <= y1; y++)
				{
					unsigned int *pData = (unsigned int*)(pixels + x0*4 + y * pitch);
					for (int x = x0; x <= x1; x++)
					{
						*pData++ = col;
					}
				}
			}
			s->Unlock();
		}
	}

	void Fill( SurfacePtr s, const Color& color, PixOpPtr pop )
	{
		if ( s ) {
			const pei::Format& format = s->GetFormat();
			unsigned int col = MapColor( format, color );
			s->Lock();
			char* pixels = (char*)s->GetPixels();
			if ( pixels ) {
				unsigned int pitch = s->GetPitch();
				unsigned int w = s->GetWidth();
				unsigned int h = s->GetHeight();
				for ( size_t y = 0; y < h; y++)
				{
					unsigned int *pData = (unsigned int*)(pixels + y * pitch);
					for (size_t x = 0; x < w; x++)
					{
						*pData = pop->Compose( col, *pData );
						pData++;
					}
				}
			}
			s->Unlock();
		}
	}

	void Fill( SurfacePtr s, const Color& color, const pei::Rectangle& bounds, PixOpPtr pop )
	{
		if ( s ) {

			const pei::Format& format = s->GetFormat();
			unsigned int col = MapColor( format, color );
			s->Lock();
			char* pixels = (char*)s->GetPixels();
			if ( pixels ) {
				unsigned int pitch = s->GetPitch();
				int x0 = (int)bounds.x();
				int y0 = (int)bounds.y();
				int x1 = (int)bounds.maxX();
				int y1 = (int)bounds.maxY();
				if ( x1 > (int)s->GetWidth()  ) x1 = (int)s->GetWidth()-1;
				if ( y1 > (int)s->GetHeight() ) y1 = (int)s->GetHeight()-1;
				for ( int y = y0; y <= y1; y++)
				{
					unsigned int *pData = (unsigned int*)(pixels + x0*s->GetBytesPerPixel() + y * pitch);
					for (int x = x0; x <= x1; x++)
					{
						*pData = pop->Compose( col, *pData );
						pData++;
					}
				}
			}
			s->Unlock();
		}
	}

    Rectangle DetectSurfaceEdge( const SurfacePtr& s, int x_offset/* = 0*/, int y_offset /*= 0*/ )
    {
        // surface has already been created from asset,
        // calc bounding box
        if ( s ) {

            int w = s->GetWidth()  - x_offset;
            int h = s->GetHeight() - y_offset;

            int minX(x_offset);
            int maxX(w);

            int minY(y_offset);
            int maxY(h);

            const pei::Format& format = s->GetFormat();
            int x(x_offset),y(y_offset);
            size_t pitch = s->GetPitch();
            unsigned int alphaMask  = format.m_AMask;
            unsigned int alphaShift = format.m_AShift;
            unsigned int alphaLoss  = format.m_ALoss;
            unsigned int alphaThreshold = 0;

            bool edge_found(false);
            s->Lock();
            for ( y = y_offset; y < h && !edge_found; y++ )
            {
            	unsigned int *pDstData = (unsigned int*)((char*)s->GetPixels() + (y * pitch ));
                for ( x = x_offset; x < w && !edge_found; x++ ) {
                	unsigned int p = *pDstData++;
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
            	unsigned int *pDstData = (unsigned int*)((char*)s->GetPixels() + (y * pitch ));
                for ( x = x_offset; x < w && !edge_found; x++ ) {
                	unsigned int p = *pDstData++;
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
            	unsigned int *pDstData = (unsigned int*)((char*)s->GetPixels() + x*4);
                for ( y = y_offset; y < h && !edge_found; y++ ) {
                	unsigned int p = *pDstData;
                    if ( ((p & alphaMask) >> alphaShift << alphaLoss) > alphaThreshold ) {
                        minX = x;
                        edge_found = true;
                        break;
                    }
                    pDstData = (unsigned int*)((char*)pDstData + pitch);
                }
            }
            edge_found = false;
            maxX = minX;
            for ( x = w-1; x >= minX && !edge_found; x-- )
            {
            	unsigned int *pDstData = (unsigned int*)((char*)s->GetPixels() + x*4);
                for ( y = y_offset; y < h && !edge_found; y++ ) {
                	unsigned int p = *pDstData;
                    if ( ((p & alphaMask) >> alphaShift << alphaLoss) > alphaThreshold ) {
                        maxX = x;
                        edge_found = true;
                        break;
                    }
                    pDstData = (unsigned int*)((char*)pDstData + pitch);
                }
            }
            s->Unlock();

            return pei::Rectangle( minX, minY, ((maxX > minX) ? ((maxX - minX) + 1) : 0), ((maxY > minY) ? ((maxY - minY) + 1) : 0) );
        }
        return pei::Rectangle();
    }


    void ClearAlpha( SurfacePtr buffer, SurfacePtr brush, int x_pos, int y_pos, bool clear_alpha /*= true */ )
    {
        // setup
        int w = brush->GetWidth();
        int h = brush->GetHeight();
        int x0 = x_pos - w/2;
        int y0 = y_pos - h/2;

        // bounds
        Rectangle draw_bounds( x0, y0, w, h );
        Rectangle dst_bounds( 0, 0, buffer->GetWidth(), buffer->GetHeight() );

        // clip render bounds
        pei::intersectRect( draw_bounds, dst_bounds, dst_bounds );
        if ( dst_bounds.w() == 0 || dst_bounds.h() == 0 ) {
            // clipped away
            return;
        }
        // clip brush
        Rectangle src_bounds( (x0 < 0 ? w - dst_bounds.w() : 0), (y0 < 0 ? h - dst_bounds.h() : 0),
                            dst_bounds.w(), dst_bounds.h() );

        size_t spitch = brush->GetPitch();
        size_t dpitch = buffer->GetPitch();
        size_t sbpp   = brush->GetBytesPerPixel();
        size_t dbpp   = buffer->GetBytesPerPixel();

        unsigned int dAlphaMask  = buffer->GetFormat().m_AMask;

        // raster operators (per pixel ops - could be a shader, huh ?)
        class PixelOperator
        {
        public:
        	virtual ~PixelOperator() {}

            virtual unsigned int execute(unsigned int p, unsigned int s) = 0;
        };
        // clear alpha
        class PopClearAlpha : public PixelOperator
        {
        	unsigned int mask;
        public:
            virtual ~PopClearAlpha() {}

            PopClearAlpha( unsigned int m ) : mask(m) {}

            virtual unsigned int execute(unsigned int d, unsigned int s)
            {
                return (d & ~mask) | ((~(s & mask)) & (d&mask));
            };
        } clearAlpha(dAlphaMask);
        // set alpha
        class PopSetAlpha : public PixelOperator
        {
        	unsigned int mask;
        public:
            virtual ~PopSetAlpha() {}

            PopSetAlpha( unsigned int m ) : mask(m) {}

            virtual unsigned int execute(unsigned int d, unsigned int s)
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

        brush->Lock();
        buffer->Lock();

        // render loop
        int yd = MIN_Y(dst_bounds);
        int ys = MIN_Y(src_bounds);
        for ( ; ys <= MAX_Y(src_bounds) && yd <= MAX_Y(dst_bounds); ys++, yd++ ) {
            int xs = MIN_X(src_bounds);
            int xd = MIN_X(dst_bounds);

            unsigned int *pSrcData = (unsigned int*)((char*)brush->GetPixels() + (ys * spitch ) + xs * sbpp);
            unsigned int *pDstData = (unsigned int*)((char*)buffer->GetPixels() + (yd * dpitch ) + xd * dbpp);
            for ( ; xs <= MAX_X(src_bounds) && xd <= MAX_X(dst_bounds); xs++, xd++ )
            {
                *pDstData = pop->execute(*pDstData, *pSrcData);
                pDstData++; pSrcData++;
            }
        }

        buffer->Unlock();
        brush->Unlock();
    }

    void AlphaReduce( SurfacePtr buffer, double threshhold )
    {
        if ( buffer )
        {
            int w = buffer->GetWidth();
            int h = buffer->GetHeight();

            size_t dpitch = buffer->GetPitch();
            size_t alphaMask  = buffer->GetFormat().m_AMask;
            size_t alphaShift = buffer->GetFormat().m_AShift;
            size_t alphaLoss  = buffer->GetFormat().m_ALoss;

            buffer->Lock();
            for ( int y = 0; y < h; y++ )
            {
                int x = 0;
                unsigned int *pDstData = (unsigned int*)((char*)buffer->GetPixels() + (y * dpitch ) + x*4);
                for ( ; x < w; x++ ) {
                	unsigned int pixel = *pDstData;
                    double alpha = (double)((pixel & alphaMask) >> alphaShift << alphaLoss) / 255.0;
                    if ( alpha > 0.0 && alpha <= threshhold ) {
                        // clear a!
                        *pDstData = pixel & ~alphaMask;
                    }
                    pDstData++;
                }
            }
            buffer->Unlock();
        }
    }

    void DrawAlpha( SurfacePtr alpha_buffer, const Rectangle &bounds, SurfacePtr dest, int dx, int dy )
    {
        // surface has already been created from asset,
        // calc bounding box
        if ( dest ) {

            int w = dest->GetWidth() - (dx+bounds.x()) + 1;
            int h = dest->GetHeight() - (dy+bounds.y()) + 1;

            if ( bounds.w() < w ) w = bounds.w();
            if ( bounds.h() < h ) h = bounds.h();
            if ( w == 0 || h == 0 ) {
                return;
            }
            int minSx = bounds.x();
            int minSy = bounds.y();
            int maxSx = minSx + w - 1;
            int maxSy = minSy + h - 1;

            size_t dpitch = dest->GetPitch();
            size_t spitch = alpha_buffer->GetPitch();
            size_t sbpp   = alpha_buffer->GetBytesPerPixel();
            size_t dbpp   = dest->GetBytesPerPixel();
            size_t alphaMask  = alpha_buffer->GetFormat().m_AMask;
            size_t alphaShift = alpha_buffer->GetFormat().m_AShift;
            size_t alphaLoss  = alpha_buffer->GetFormat().m_ALoss;

            size_t red      = pei::MapColor( dest->GetFormat(), pei::Color( 0xff, 0x00, 0x00 ) );
            size_t yellow   = pei::MapColor( dest->GetFormat(), pei::Color( 0xff, 0xff, 0x00 ) );
            size_t green    = pei::MapColor( dest->GetFormat(), pei::Color( 0x00, 0xff, 0x00 ) );
            size_t mangenta = pei::MapColor( dest->GetFormat(), pei::Color( 0x00, 0xff, 0xff ) );

            alpha_buffer->Lock();
            dest->Lock();

            int y(minSy);
            for ( ; y <= maxSy; y++ )
            {
                int x = minSx;
                unsigned int *pSrcData = (unsigned int*)((char*)alpha_buffer->GetPixels() + (int)((y * spitch ) + bounds.x()*sbpp));
                unsigned int *pDstData = (unsigned int*)((char*)dest->GetPixels() + ((y+dy) * dpitch ) + (x+dx)*dbpp);
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
            dest->Unlock();
            alpha_buffer->Unlock();
        }
    }

    void DrawBevelBox( SurfacePtr s, const Rectangle &bounds,
                       const Color& light /*= XC_WHITE*/, const Color& dark /*= XC_BLACK */ )
    {
        if ( s ) {
            double x = bounds.x();
            double y = bounds.y();
            double w = bounds.w();
            double h = bounds.h();

            Fill32( s, light, Rectangle(x,       y,       w, 1 ) );
            Fill32( s, light, Rectangle(x,       y,       1, h ) );
            Fill32( s, dark,  Rectangle(x + w-1, y,       1, h ) );
            Fill32( s, dark,  Rectangle(x,       y + h-1, w, 1 ) );
        }
    }

    void DrawBevelBox( SurfacePtr s,
                       const Color& light /*= XC_WHITE*/, const Color& dark /*= XC_BLACK */ )
    {
        if ( s ) {
            double x = 0;
            double y = 0;
            double w = s->GetWidth();
            double h = s->GetHeight();

            Fill32( s, light, Rectangle(x,       y,       w, 1 ) );
            Fill32( s, light, Rectangle(x,       y,       1, h ) );
            Fill32( s, dark,  Rectangle(x + w-1, y,       1, h ) );
            Fill32( s, dark,  Rectangle(x,       y + h-1, w, 1 ) );
        }
    }

    void DrawBevelBox( const RenderProfilePtr& profile, SurfacePtr s, const Rectangle &bounds,
                       const Color& light /*= XC_WHITE*/, const Color& dark /*= XC_BLACK */ )
    {
        double x = bounds.x();
        double y = bounds.y();
        double w = bounds.w();
        double h = bounds.h();

        profile->Fill( s, RenderParam( Rectangle(x,       y,       w, 1 ), light ) );
        profile->Fill( s, RenderParam( Rectangle(x,       y,       1, h ), light ) );
        profile->Fill( s, RenderParam( Rectangle(x + w-1, y,       1, h ), dark ) );
        profile->Fill( s, RenderParam( Rectangle(x,       y + h-1, w, 1 ), dark ) );
    }

    void DrawBevelBox( const RenderProfilePtr& profile, SurfacePtr s,
                       const Color& light /*= XC_WHITE*/, const Color& dark /*= XC_BLACK */ )
    {
        DrawBevelBox( profile, s, Rectangle(0, 0, s->GetWidth(), s->GetHeight() ), light, dark );
    }

    unsigned int WritePixelQuick( SurfacePtr surface, int x, int y, unsigned int color, unsigned int num /*= 1*/)
    {
        size_t pitch = surface->GetPitch();
        size_t bpp   = surface->GetBytesPerPixel();

        unsigned int n_written = 0;
        surface->Lock();
        char* pixels = (char*)surface->GetPixels();
        int h = surface->GetHeight();
        int w = surface->GetWidth();
        if ( y < h ) {
            unsigned int *p = (unsigned int*)(pixels + x*bpp + y*pitch);
            while ( x < w && num-- > 0 ) {
                *p = color; x++; n_written++; p++;
            }
        }
        surface->Unlock();

        return n_written;
    }

    size_t WritePixelPop( PixOpPtr pop, SurfacePtr surface, int x, int y, unsigned int color, size_t num /*= 1*/ )
    {
        size_t pitch = surface->GetPitch();
        size_t bpp   = surface->GetBytesPerPixel();

        size_t n_written = 0;
        surface->Lock();
        char* pixels = (char*)surface->GetPixels();
        int h = surface->GetHeight();
        int w = surface->GetWidth();
        if ( y < h ) {
        	unsigned int *p = (unsigned int*)(pixels + x*bpp + y*pitch);
            while ( x < w && num-- > 0 ) {
                *p = pop->Compose( color, *p ); x++; n_written++;
                p++;
            }
        }
        surface->Unlock();
        return n_written;
    }

    size_t ReadPixel( const SurfacePtr& surface, int x, int y )
    {
        size_t pitch = surface->GetPitch();
        size_t bpp   = surface->GetBytesPerPixel();

        size_t pix = 0;
        // either I implement this mutable or I just cast the const away...
        const_cast<SurfacePtr&>(surface)->Lock();
        if ( y < (int)surface->GetHeight() && x < (int)surface->GetHeight() ) {
            pix = *((size_t*)((char*)surface->GetPixels() + x*bpp + y*pitch));
        }
        const_cast<SurfacePtr&>(surface)->Unlock();
        return pix;
    }

    static void DrawCircleSegmenPt( PixOpPtr pop, SurfacePtr surface, int xc, int yc, int x1, int y1, unsigned int color, int segment )
    {
        switch ( segment )
        {
        case 1: // top right segment
            WritePixelQuick( surface, xc + x1, yc - y1, color, 1 );
            WritePixelQuick( surface, xc + y1, yc - x1, color, 1 );
            break;
        case 2: //  bottom right segment
            WritePixelQuick( surface, xc + x1, yc + y1, color, 1 );
            WritePixelQuick( surface, xc + y1, yc + x1, color, 1 );
            break;
        case 3: // bottom left segment
            WritePixelQuick( surface, xc - x1, yc + y1, color, 1 );
            WritePixelQuick( surface, xc - y1, yc + x1, color, 1 );
            break;
        case 4: // top left segment
            WritePixelQuick( surface, xc - x1, yc - y1, color, 1 );
            WritePixelQuick( surface, xc - y1, yc - x1, color, 1 );
            break;
        }
    }

    void DrawCircle( SurfacePtr surface, int xc, int yc, double r, unsigned int color )
    {
        // printf "Draw a circle @ $xc - $yc, Radius $r with color $color..."
        double x(0),y(r);
        double p = 1-r;

        PixOpPtr pop( new PixOpCopySrcAlpha( surface->GetFormat(),surface->GetFormat()));
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


} // namespace pei
