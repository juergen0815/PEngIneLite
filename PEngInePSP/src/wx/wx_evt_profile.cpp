/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "wx_evt_profile.h"

#include <events/event_manager.h>

pei::wx::EventType::EventType() : m_EventId(0)
{
}

pei::wx::EventType::EventType( int id ) : m_EventId(id)
{
}

bool pei::wx::EventType::IsEqual( const pei::EventTypePtr& o ) const
{
	try {
	    const pei::wx::EventTypePtr& t = boost::dynamic_pointer_cast<pei::wx::EventType>(o);
	    if ( t ) {
	        return GetId() == t->GetId();
	    }
	} catch ( std::bad_cast& badCast ) {

	}
    return false;
}

bool pei::wx::EventType::LessThan( const pei::EventTypePtr& o ) const
{
	try {
	    const pei::wx::EventTypePtr& t = boost::dynamic_pointer_cast<pei::wx::EventType>(o);
		if ( t ) {
			return GetId() < t->GetId();
		}
	} catch ( std::bad_cast& badCast ) {

	}
    return false;
}

pei::wx::Event::Event( )
	: pei::EventT< wxEvent* >( NULL, 0 )
	, m_eType( new pei::wx::EventType( wxID_NONE ) )
{
}

pei::wx::Event::Event( wxEvent& event )
	: pei::EventT< wxEvent* >( event.Clone(), 0 )
	, m_eType( new pei::wx::EventType( event.GetEventType() ) )
{
}

pei::wx::Event::~Event()
{
	if ( m_Event ) {
		delete m_Event;
	}
}

const wxEvent& pei::wx::Event::Get() const
{
	// there is nothing we can do if this is NULL
	assert( m_Event );
	return *m_Event;
}

pei::EventTypePtr pei::wx::Event::GetType() const
{
	// we might need to inject the real event here
    return m_eType;
}

pei::EventPtr pei::wx::EventProfile::PollEvent()
{
	pei::wx::Event *pEvt = new pei::wx::Event();
	// how do I poll wxEvents??
    return pei::EventPtr(pEvt);
}

pei::EventPtr pei::wx::EventProfile::WaitEvent()
{
	pei::wx::Event *pEvt = new pei::wx::Event();
	// needs to be installed into the default event handler
    return pei::EventPtr(pEvt);
}

bool pei::wx::EventProfile::ProcessEvent( pei::EventManagerPtr eventManager, pei::EventPtr event )
{
	// overload this or call from wxApp::FilterEvent
	return pei::EventProfile::ProcessEvent( eventManager, event );
}


bool pei::wx::EventProfile::ProcessEvent(wxEvent& event)
{
	pei::EventPtr evt( new pei::wx::Event( event ) );
	return ProcessEvent( GetEventManager(), evt);
}
