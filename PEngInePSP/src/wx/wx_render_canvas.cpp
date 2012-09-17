/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "wx_render_canvas.h"
#include "wx_render_profile.h"

#include <wx/wx_evt_profile.h>

#include <iostream>

namespace pei {
  namespace wx {

    static const int attribs[] =
    {
    		WX_GL_DOUBLEBUFFER, 1,
    		0, 0
    };

    RenderCanvas::RenderCanvas( wxWindow* parent, wxWindowID id, long style )
        : wxGLCanvas(parent, id, (int*)attribs, wxDefaultPosition, wxDefaultSize, (style & ~wxFULL_REPAINT_ON_RESIZE) )
        , m_UpdateTimer(this, UpdateTimer)
        , m_CurrentTicks(0)
    	, m_EventProfile(new pei::wx::EventProfile())
    {
        pei::RenderProfilePtr renderProfile = pei::wx::RenderProfile::Create( this );
        renderProfile->LinkSmartPtr( renderProfile );

        m_RenderManager = pei::RenderManager::Create();
        m_RenderManager->SetRenderProfile( renderProfile );

        // do never start this event handler! wx takes over main loop!
        m_EventManager = pei::EventManager::Create( );
        m_EventManager->SetEventProfile( m_EventProfile );
        m_EventManager->SetRendeManager( m_RenderManager );
    }

    RenderCanvas::RenderCanvas( wxWindow* parent, wxWindowID id, RenderCanvas* other )
        : wxGLCanvas(parent, id, (int*)attribs, wxDefaultPosition, wxDefaultSize, (other->GetWindowStyle() & ~wxFULL_REPAINT_ON_RESIZE ) )
        , m_UpdateTimer(this, UpdateTimer)
        , m_CurrentTicks(0)
        , m_EventProfile(new pei::wx::EventProfile())
    {
        m_RenderManager = other->GetRenderManager();
        m_EventManager = other->GetEventManager();
    }

    RenderCanvas::~RenderCanvas()
    {
        Close();
    }

	bool RenderCanvas::ProcessEvent(wxEvent& event) {
		// this injects the event into our local event handler
		if (m_EventProfile) m_EventProfile->ProcessEvent(event);
		return wxGLCanvas::ProcessEvent( event );
	}

    void RenderCanvas::Init(  )
    {
        // must be called explicitly after the parent frame is opened
		const wxSize clientSize = GetClientSize();
		m_RenderManager->GetRenderProfile()->OpenScreen( clientSize.GetWidth(), clientSize.GetHeight(), 0, "" );
		Activate( true );
    }

    void RenderCanvas::Close()
    {
        m_UpdateTimer.Stop();
        Disconnect(wxEVT_PAINT, wxPaintEventHandler(RenderCanvas::OnPaint));
        Disconnect(wxEVT_TIMER, wxTimerEventHandler(RenderCanvas::OnRedrawTimer));

        if ( m_EventManager ) {
            m_EventManager->Exit(); // This should have no effect - only for async event loops
            m_EventManager->Release(); // calls RenderManager::ShutDown, which call RenderProfile::Close which removes all render nodes
            m_EventManager = pei::EventManagerPtr();
        }
        m_EventProfile = pei::wx::EventProfilePtr();
        m_RenderManager = pei::RenderManagerInterfacePtr();
    }

    bool RenderCanvas::Destroy()
    {
        Close();
    	return wxGLCanvas::Destroy();
    }

    void RenderCanvas::Activate( bool active )
    {
    	if ( active ) {
    		m_UpdateTimer.Start( 10 );
    		// init start ticks
    		m_CurrentTicks = m_RenderManager->GetRenderProfile()->GetTicks();
            Connect(wxEVT_PAINT, wxPaintEventHandler(RenderCanvas::OnPaint));
            Connect(wxEVT_TIMER, wxTimerEventHandler(RenderCanvas::OnRedrawTimer));
    	} else {
    		m_UpdateTimer.Stop();
            Disconnect(wxEVT_PAINT, wxPaintEventHandler(RenderCanvas::OnPaint));
            Disconnect(wxEVT_TIMER, wxTimerEventHandler(RenderCanvas::OnRedrawTimer));
    	}
    }

    void RenderCanvas::RenderFrame()
    {
        if ( m_RenderManager ) {
            // SwapBuffers is called by render manager
            m_CurrentTicks = m_RenderManager->RenderFrame( m_CurrentTicks );
        }
    }

    void RenderCanvas::OnSizeEvent( wxSizeEvent& WXUNUSED(event) )
    {
        // Set the OpenGL viewport according to the client size of this canvas.
        Refresh(false);
    }

    void RenderCanvas::OnPaint( wxPaintEvent& event )
    {
        // This is required even though dc is not used otherwise.
        // PrepareDC(dc);
        wxPaintDC dc(this);
        const wxSize size = GetClientSize();
        if ( m_RenderManager ) m_RenderManager->ResizeViewport( size.GetWidth(), size.GetHeight() );
        RenderFrame();
        event.Skip();
    }

    void RenderCanvas::OnRedrawTimer( wxTimerEvent& WXUNUSED(event) )
    {
        Refresh(false);
    }

    pei::RenderManagerInterfacePtr RenderCanvas::GetRenderManager() const
    {
        return m_RenderManager;
    }

    pei::EventManagerPtr RenderCanvas::GetEventManager() const
    {
    	return m_EventManager;
    }

  }
}
