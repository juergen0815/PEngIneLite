/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <render/render_target.h>

#include <render/render_profile.h>
#include <render/render_surface.h>
#include <render/render_param.h>

#include <video/blitter.h>
#include <video/render_tools.h>

namespace pei
{

    RenderTarget::RenderTarget()
        : m_RenderMode( EN_ALPHA_F )
        , m_LineSize(1)
    {
    }

    RenderTarget::~RenderTarget()
    {
    }

    SurfacePtr RenderTarget::SetSurface( const SurfacePtr& surface, int idx )
    {
        return SurfacePtr();
    }

    SurfacePtr RenderTarget::GetSurface( int idx ) const
    {
        return SurfacePtr();
    }

    void RenderTarget::Render( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
    {}

    void RenderTarget::PreRender( RenderProfilePtr& profile, const RenderParam& param )
    {}

    void RenderTarget::PostRender( RenderProfilePtr& profile, const RenderParam& param )
    {}

    //////////////////////////////////////////////////////////////////////////

    BufferedRenderTarget::BufferedRenderTarget()
    {
        // reserve at least two entries for textures
        m_Surface.reserve(2);
    }

    SurfacePtr BufferedRenderTarget::SetSurface( const  SurfacePtr& surface, int idx /*= 0*/ )
    {
        if ( idx >= (int)m_Surface.size() ) {
            m_Surface.resize( idx+1 );
        }
        m_Surface[idx] = surface;
        return m_Surface[idx];
    }

    SurfacePtr BufferedRenderTarget::GetSurface( int idx /*= 0*/ ) const
    {
        if ( idx < (int)m_Surface.size() ) {
            return m_Surface[idx];
        }
        return SurfacePtr();
    }

    void BufferedRenderTarget::Render( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
    {
        if ( buffer ) {
            SurfacePtr source = GetSurface();
            if ( source ) {
                if ( param.GetXScale() != 1.0 || param.GetYScale() != 1.0 ) {
                    pei::Blitter blitter( pei::PixOpPtr( new pei::PixOpAlphaBlit( source->GetFormat(), buffer->GetFormat(), param.GetAlpha() )));
                    blitter.ScaleBlit( source, buffer, param.GetX(), param.GetY(), source->GetWidth() * param.GetXScale(), source->GetHeight() * param.GetYScale(), false );
                } else {
                    pei::Rectangle dr( param.GetX(), param.GetY(), buffer->GetWidth(), buffer->GetHeight() );
                    pei::Rectangle sr( 0, 0, source->GetWidth(), source->GetHeight() );
                    pei::Blitter::Blit( source, sr, buffer, dr, pei::PixOpPtr( new pei::PixOpAlphaBlit( source->GetFormat(), buffer->GetFormat(), param.GetAlpha() )));
                }
            } else {
                pei::Fill( buffer, param.GetColor(), param.GetRectangle(), pei::PixOpPtr( new pei::PixOpAlphaBlit( pei::Format(), buffer->GetFormat())) );
            }
        }
    }


} /* namespace pei */
