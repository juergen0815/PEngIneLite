/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef WX_RENDER_CANVAS_H
#define WX_RENDER_CANVAS_H

#include <profile_config.h>

#include <render/render_profile.h>
#include <render/render_manager.h>

#include <events/event_manager.h>

#include <wx/wx.h>
#include <wx/glcanvas.h>

//#include <wx/wx_evt_profile.h>

namespace pei {
    namespace wx {

        class RenderProfile;
        typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

        class EventProfile;
        typedef boost::shared_ptr<EventProfile> EventProfilePtr;

        class RenderCanvas : public wxGLCanvas
        {
        protected:
            enum
            {
                UpdateTimer = wxID_HIGHEST + 1
            };

            //TODO: Move timer into main frame to allow all render panels to refresh (split windows)
            wxTimer               m_UpdateTimer;
            double                m_CurrentTicks;
            pei::RenderManagerInterfacePtr m_RenderManager;
            pei::EventManagerPtr  m_EventManager;
            // we must call the profile directly because we run wx's main loop!
            pei::wx::EventProfilePtr m_EventProfile;

        public:
            RenderCanvas( wxWindow* parent, wxWindowID id, long style = 0 );

            RenderCanvas( wxWindow* parent, wxWindowID id, RenderCanvas* other );

            virtual ~RenderCanvas();

            virtual pei::RenderManagerInterfacePtr GetRenderManager() const ;

            virtual pei::EventManagerPtr GetEventManager() const;

            virtual void Init( );

            virtual bool Destroy();

            virtual void Activate( bool active = true );

        protected:

            virtual void Close();

			virtual bool ProcessEvent(wxEvent& event);

            virtual void OnPaint(wxPaintEvent& WXUNUSED(event));

            virtual void OnSizeEvent(wxSizeEvent& WXUNUSED(event));

            virtual void OnRedrawTimer(wxTimerEvent& WXUNUSED(event));

            virtual void RenderFrame();

        };

    } /* namespace wx */
} /* namespace pei */
#endif /* WX_RENDER_CANVAS_H */
