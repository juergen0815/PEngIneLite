/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <interfaces/render_node_interface.h>

#include <render/render_profile.h>
#include <render/render_target.h>
#include <render/render_container.h>

#include <video/render_tools.h>

namespace pei
{

    const char* RenderProfile::DEFAULT_TARGET = "DEFAULT_TARGET";
    pei::RenderTargetInterfacePtr RenderProfile::m_DefaultRenderTarget;

    RenderProfile::RenderProfile()
        : m_Screen( new pei::RenderContainer )
    #if defined (__MACOSX__) || defined (__APPLE__)
        , m_Flags( 0 ) // MacOS runs within event loop - main thread
    #else
        , m_Flags( EN_RENDER_ASYNC )
    #endif
        , m_MatrixMode(0)
        , m_ClippingEnabled(false)
    {
        // create a default render target - is NULL until now
        RenderTargetInterfacePtr rti( new pei::RenderTarget() );

        m_Screen->LinkSmartPtr( m_Screen );
        m_Screen->SetRenderTarget( rti );

        // set a default render target - will be over written by implementation
        SetDefaultRenderTarget( rti );
        // do not clone default. We can use this directly
        AddRenderTarget( DEFAULT_TARGET, rti );
    }

    RenderProfile::~RenderProfile()
    {
    }

    void RenderProfile::SetDefaultRenderTarget( const RenderTargetInterfacePtr& rti ) {
        m_DefaultRenderTarget = rti;
    }

    RenderTargetInterfacePtr RenderProfile::GetDefaultRenderTarget() {
        if ( m_DefaultRenderTarget == NULL ) {
            // this is dummy but avoids NULL check
            m_DefaultRenderTarget = RenderTargetInterfacePtr( new RenderTarget() );
        }
        return m_DefaultRenderTarget->Create();
    }

    double RenderProfile::GetTicks()
    {
        return 0;
    }

    void RenderProfile::RenderFrame( pei::RenderProfilePtr& self, double ticks )
    {
        m_Screen->Update( self, ticks, 0 );
        m_Screen->Draw( self, m_BackBuffer, LoadIdentity() );
    }

    void RenderProfile::SwapBuffers()
    {
    }

    void RenderProfile::Lock()
    {
        m_Screen->Lock();
    }

    bool RenderProfile::TryLock()
    {
        return m_Screen->TryLock();
    }

    void RenderProfile::Unlock()
    {
        m_Screen->Unlock();
    }

    void RenderProfile::AddRenderTarget( const char* name, const pei::RenderTargetInterfacePtr& rti )
    {
        // overwrite existing target!
        m_RenderTargets[ name ] = rti;
    }

    pei::RenderTargetInterfacePtr RenderProfile::GetRenderTarget( const char* name /* = DEFAULT_TARGET */) const
    {
        RenderTargetList::const_iterator it = m_RenderTargets.find( name );
        if ( it != m_RenderTargets.end() ) {
            // clone it, do not use it directly! This is a factory!
            return it->second->Create();
        }
        // never NULL!
        return pei::RenderTargetInterfacePtr( new RenderTarget() );
    }

    bool RenderProfile::OpenScreen( size_t width, size_t height, size_t flags, const char* title )
    {
        m_Screen->SetSize( (double)width, (double)height );
        return false;
    }

    pei::RenderNodeInterfacePtr RenderProfile::ResizeScreen( int w, int h )
    {
        m_Screen->SetSize( (double)w, (double)h );
        return m_Screen;
    }

    pei::RenderNodeInterfacePtr RenderProfile::GetScreen() const
    {
        return m_Screen;
    }

    bool RenderProfile::Init()
    {
        RenderProfilePtr self(m_WeakSelf);
        return m_Screen->Init( self );
    }

    void RenderProfile::Close()
    {
        if ( m_Screen ) {
            // will stop render loop
            m_Screen->Activate(false);
            // removes all children - waits until renderloop is done
            m_Screen->RemoveAllChildren();
        }
    }

    bool RenderProfile::IsActive()
    {
        return m_Screen && m_Screen->IsActive();
    }

    void RenderProfile::Delay( double t )
    {
    }

    void RenderProfile::MakeCurrent()
    {
    }

    void RenderProfile::ClearBuffers()
    {
    }

    void RenderProfile::Fill( SurfacePtr surface, const RenderParam& param )
    {
        // SW render into surface with bounds
        pei::Fill32( surface, param.GetColor(), param.GetRectangle() );
    }

    pei::SurfacePtr RenderProfile::CreateSurface( const pei::Format& fmt )
    {
        return pei::SurfacePtr();
    }

    RenderParam RenderProfile::LoadIdentity( const pei::RenderParam& client /* = pei::RenderParam */ )
    {
        m_RenderParam = client;
        // must have initial size
        m_RenderParam.SetWidth( m_Screen->GetWidth() ).SetHeight( m_Screen->GetHeight( ));
        return m_RenderParam;
    }

    void RenderProfile::SetMatrixMode( unsigned int mode )
    {
        m_MatrixMode = mode;
    }

    unsigned int RenderProfile::GetMatrixMode( ) const
    {
        return m_MatrixMode;
    }

    void RenderProfile::LoadMatrix( const Matrix4x4& m )
    {
        m_Matrix = m;
    }

    const RenderParam& RenderProfile::PushMatrix( const RenderParam& p )
    {
        // default does not push, but override last
        m_RenderParam = p;
        m_ParamStack.push_back( p );
        return m_RenderParam;
    }

    const RenderParam& RenderProfile::PushMatrix( )
    {
        // default does not push, but override last
        m_ParamStack.push_back( m_RenderParam );
        return m_RenderParam;
    }

    pei::RenderParam RenderProfile::Translate( const RenderParam& param )
    {
        // so, this is almost like a matrix multiplication - what it should be
        // Add = Translate
        // Mul = Scale
        // ??? = Rotate ??? - in Add()...
        // current Renderparam always holds transformed matrix
        m_RenderParam.SetModelView( GetMatrix() );
        return param.Add(m_RenderParam).Mul(m_RenderParam);
    }

    const RenderParam& RenderProfile::PopMatrix()
    {
        m_ParamStack.pop_back();
        if ( m_ParamStack.size() ) {
            m_RenderParam = m_ParamStack[ m_ParamStack.size()-1 ];
        } else {
            m_RenderParam = pei::RenderParam();
        }
        return m_RenderParam;
    }

    void RenderProfile::SetClipRegion( const pei::Rectangle& bounds )
    {
        m_ClipRegion = bounds;
    }

    const Rectangle& RenderProfile::GetClipRegion() const
    {
        return m_ClipRegion;
    }

    void RenderProfile::ClearClipRegion()
    {
        m_ClipRegion = pei::Rectangle();
    }

    void RenderProfile::EnableClipping()
    {
        m_ClippingEnabled = true;
    }

    void RenderProfile::DisableClipping()
    {
        m_ClippingEnabled = false;
    }

    bool RenderProfile::IsClippingEnabled() {
        return m_ClippingEnabled;
    }

} /* namespace pei */
