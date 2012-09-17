#include "event_profile.h"
#include "event_manager.h"

pei::EventPtr pei::EventProfile::PollEvent()
{
    return pei::EventPtr();
}

pei::EventPtr pei::EventProfile::WaitEvent()
{
    return pei::EventPtr();
}

bool pei::EventProfile::ProcessEvent( pei::EventManagerPtr eventManager, pei::EventPtr event )
{
	return eventManager->ProcessEvent( event );
}

void pei::EventProfile::LinkEventManager( pei::EventManagerPtr eventManager )
{
	m_EventManager = eventManager;
}

pei::EventManagerPtr pei::EventProfile::GetEventManager() const
{
	return m_EventManager;
}
