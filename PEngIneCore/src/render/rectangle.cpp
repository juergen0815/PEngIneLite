/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "rectangle.h"

namespace pei
{

bool intersectRect( const Rectangle& bounds, const Rectangle& rect, Rectangle& trc )
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
        trc.x() = (min_x2 >= min_x1) ? min_x2 : min_x1;
        trc.y() = (min_y2 >= min_y1) ? min_y2 : min_y1;

        trc.w() = ((max_x2 <= max_x1) ? max_x2 : max_x1) - trc.x() + 1;
        trc.h() = ((max_y2 <= max_y1) ? max_y2 : max_y1) - trc.y() + 1;

        clip_it = true;
    } else {
        // out of bounds!
        trc.x() = rect.x();
        trc.y() = rect.y();
        trc.w() = 0;
        trc.h() = 0;
        clip_it = true;
    }
    return clip_it;
}


} /* namespace pei */
