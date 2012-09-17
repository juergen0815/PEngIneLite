/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "render_param.h"

#include <interfaces/render_node_interface.h>

namespace pei
{
    RenderParam::RenderParam()
        : m_XOrigin(0.0)
        , m_YOrigin(0.0)
        , m_ZOrigin(0.0)
        , pad0(0)
        , m_XScale(1.0)
        , m_YScale(1.0)
        , m_ZScale(1.0)
        , pad1(0)
        , m_XRotation(0.0)
        , m_YRotation(0.0)
        , m_ZRotation(0.0)
        , pad2(0)
        , m_X(0.0)
        , m_Y(0.0)
        , m_Z(0.0)
        , pad3(1)
        , m_Width(0.0)
        , m_Height(0.0)
        , m_Depth(0.0)
        , m_Color(1.0,1.0,1.0,1.0)
        , m_Volume(1.0)
        , m_RenderPass( pei::DEFAULT_PASS )
    {}

    RenderParam::RenderParam( const RenderParam& o )
        : m_XOrigin(o.m_XOrigin)
        , m_YOrigin(o.m_YOrigin)
        , m_ZOrigin(o.m_ZOrigin)
        , pad0(0)
        , m_XScale(o.m_XScale)
        , m_YScale(o.m_YScale)
        , m_ZScale(o.m_ZScale)
        , pad1(0)
        , m_XRotation(o.m_XRotation)
        , m_YRotation(o.m_YRotation)
        , m_ZRotation(o.m_ZRotation)
        , pad2(0)
        , m_X(o.m_X)
        , m_Y(o.m_Y)
        , m_Z(o.m_Z)
        , pad3(1)
        , m_Width(o.m_Width)
        , m_Height(o.m_Height)
        , m_Depth(o.m_Depth)
        , m_Color(o.m_Color)
        , m_Volume(o.m_Volume)
        , m_RenderPass(o.m_RenderPass)
        , m_MatrixRef(o.m_MatrixRef)
    {}

    RenderParam::RenderParam( const Rectangle& rect, const Color& col )
        : m_XOrigin(0.0)
        , m_YOrigin(0.0)
        , m_ZOrigin(0.0)
        , pad0(0)
        , m_XScale(1.0)
        , m_YScale(1.0)
        , m_ZScale(1.0)
        , pad1(0)
        , m_XRotation(0)
        , m_YRotation(0)
        , m_ZRotation(0)
        , pad2(0)
        , m_Z(0.0)
        , pad3(1)
        , m_Depth(0.0)
        , m_Color(col)
        , m_Volume(1.0)
        , m_RenderPass(pei::DEFAULT_PASS)
    {
        SetRectangle( rect );
    }

} /* namespace pei */
