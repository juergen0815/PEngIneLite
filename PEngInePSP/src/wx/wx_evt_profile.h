/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef WX_EVENT_PROFILE_H
#define WX_EVENT_PROFILE_H

#include <events/event_profile.h>
#include <events/event.h>

#include <wx/event.h>

namespace pei {
    namespace wx {

    	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

    	// a event type bound to event ID - generated as key
        class EventType : public pei::EventType
        {
        protected:
        	int		m_EventId; // this is actually the wxEventType
        public:
            virtual ~EventType() {}

            EventType( );

            EventType( int id );

            virtual bool IsEqual(const pei::EventTypePtr& o) const;

            virtual bool LessThan(const pei::EventTypePtr& o) const;

            virtual int GetId() const { return m_EventId; }

            virtual void SetId( int id ) { m_EventId = id; }
        };
        typedef boost::shared_ptr<EventType> EventTypePtr;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Must use a pointer because the wxEvent is abstract! (Clone() method!) This is dangerous for async events!
        class Event : public pei::EventT< wxEvent* >
        {
            pei::wx::EventTypePtr m_eType;
        public:
            Event();
            Event( wxEvent& event );

            virtual ~Event();

            // make sure this is never called outside of the lifetime of m_Event!! - we spare the NULL checks
            virtual const wxEvent& Get() const;

            virtual pei::EventTypePtr GetType() const;
        };
        typedef boost::shared_ptr<Event> EventPtr;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////

        class EventProfile : public pei::EventProfile
        {
            virtual pei::EventPtr PollEvent(); // NOP

            virtual pei::EventPtr WaitEvent(); // NOP
		public:
            virtual bool ProcessEvent( pei::EventManagerPtr eventManager, pei::EventPtr event );

            /** called from main frame to dispatch events to local event processor */
			virtual bool ProcessEvent(wxEvent& event);

        };
        typedef boost::shared_ptr<EventProfile> EventProfilePtr;

    }
}

#endif // WX_EVENT_PROFILE_H
