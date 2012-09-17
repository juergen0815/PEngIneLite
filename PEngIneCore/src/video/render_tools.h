/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_TOOLS_H
#define RENDER_TOOLS_H

#include <vector>
#include <string>

#include <render/rectangle.h>
#include <render/render_surface.h>
#include <render/render_profile.h>

#include <video/blitter.h>

namespace pei
{
	static inline int MakePow2(int num)
	{
		int value = 1;
		while ( value < num ) { value <<= 1; }
		return value;
	}

	unsigned int MapColor( const pei::Format& format, const pei::Color& col );

	void FillCarret( SurfacePtr s, size_t blocks = 16 );

	void Fill32( SurfacePtr s, const Color& color );

	void Fill32( SurfacePtr s, const Color& color, const pei::Rectangle& bounds );

	void Fill( SurfacePtr s, const Color& color, PixOpPtr pop );

	void Fill( SurfacePtr s, const Color& color, const pei::Rectangle& bounds, PixOpPtr pop );

	Rectangle DetectSurfaceEdge( const SurfacePtr& s, int x_offset = 0, int y_offset = 0 );

    void ClearAlpha( SurfacePtr buffer, SurfacePtr brush, int x_pos, int y_pos, bool clear_alpha  = true  );

    void AlphaReduce( SurfacePtr buffer, double threshhold );

    void DrawAlpha( SurfacePtr alpha_buffer, const Rectangle &bounds, SurfacePtr dest, int dx, int dy );

    void DrawBevelBox( SurfacePtr s, const Rectangle &bounds,
                       const pei::Color& light = COL_WHITE, const pei::Color& dark = COL_BLACK );

    void DrawBevelBox( SurfacePtr s, const pei::Color& light = COL_WHITE, const pei::Color& dark = COL_BLACK  );

    void DrawBevelBox( const RenderProfilePtr& profile, SurfacePtr s, const Rectangle &bounds,
                       const pei::Color& light = COL_WHITE, const pei::Color& dark = COL_BLACK );

    void DrawBevelBox( const RenderProfilePtr& profile, SurfacePtr s, const pei::Color& light = COL_WHITE, const pei::Color& dark = COL_BLACK  );

    unsigned int WritePixelQuick( SurfacePtr surface, int x, int y, unsigned int color, unsigned int num = 1);

    size_t WritePixelPop( PixOpPtr pop, SurfacePtr surface, int x, int y, unsigned int color, size_t num /*= 1*/ );

    size_t ReadPixel( const SurfacePtr& surface, int x, int y );

    void DrawCircle( SurfacePtr surface, int xc, int yc, double r, unsigned int color );
};

#endif // __UTILS_H__
