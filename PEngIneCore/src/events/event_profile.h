/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef EVENT_PROFILE_H
#define EVENT_PROFILE_H

#include <events/event.h>

#include <boost/shared_ptr.hpp>

namespace pei
{
	class EventManager;
    typedef boost::shared_ptr<EventManager> EventManagerPtr;

    class EventProfile;
    typedef boost::shared_ptr<EventProfile> EventProfilePtr;

    class EventProfile
    {
    	friend class EventManager;

    	EventManagerPtr m_EventManager;
    public:
    	virtual ~EventProfile() {}

        virtual pei::EventPtr PollEvent();

        virtual pei::EventPtr WaitEvent();

        virtual bool ProcessEvent( pei::EventManagerPtr eventManager, pei::EventPtr event );
    protected:
        virtual void LinkEventManager( pei::EventManagerPtr eventManager );

        virtual pei::EventManagerPtr GetEventManager() const;

    };
} // namespace pei

#endif // EVENT_PROFILE_H
