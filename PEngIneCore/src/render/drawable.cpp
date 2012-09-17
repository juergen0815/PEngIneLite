/*
 * rnd_zoom_widget.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: Jurgen
 */

#include "drawable.h"

namespace pei {

    /** */
    Drawable::Drawable( )
    {
    }

    /** */
    Drawable::Drawable( const RenderParam& p, const SurfacePtr& s /*= SurfacePtr()*/ )
        : RenderNode(p,s)
    {
    }

    /** */
    Drawable::Drawable( const SurfacePtr& s )
        : RenderNode(s)
    {
    }

    /** */
    Drawable::~Drawable()
    {
    }

    // draw the raw data to destination
    pei::SurfacePtr Drawable::OnPreDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const pei::RenderParam& param  )
    {
        // e.g. enable shader program
        m_RenderTarget->PreRender( profile, param );
        // this allows an object to create and render into a back buffer (or even FBO)
        if ( (m_Flags & EN_DRAW_BEFORE_CHILDREN) != 0 ) {
            Render( profile, buffer, param );
        }
        return buffer;
    }

    void Drawable::OnDraw( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param  )
    {
        // RenderNode does not draw! Use Drawable() instead
        if ( (m_Flags & EN_DRAW_BEFORE_CHILDREN) == 0 ) {
            Render( profile, buffer, param );
        }
        // e.g. disable shader program
        m_RenderTarget->PostRender( profile, param );
    }


    void Drawable::Render( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
    {
        m_RenderTarget->Render( profile, buffer, param );
    }


} /* namespace render */
