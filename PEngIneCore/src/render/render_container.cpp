/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "render_container.h"

namespace pei {

    /** */
    RenderContainer::RenderContainer( )
    {
    }

    /** */
    RenderContainer::~RenderContainer()
    {
    }

    int RenderContainer::Draw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam & param)
    {
        /* for performance reason, this node does not manipulate its current render state!
         * You cannot move a "group" of objects. Use a plain RenderNode instead!
         * No flags are support! We always "draw". */

        int n(0);
        RenderNodeList::iterator it = m_Children.begin();
        while ( it != m_Children.end() ) {
            RenderNodeInterfacePtr& ro = *it++;
            n += ro->Draw( profile, buffer, param );
        }
        return n;
    }

    void RenderContainer::Render( SurfacePtr& buffer, const RenderParam& param )
    {
    }

    void RenderContainer::OnDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param  )
    {
    }

    // draw the raw data to destination
    SurfacePtr RenderContainer::OnPreDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param  )
    {
        return buffer;
    }


} /* namespace render */
