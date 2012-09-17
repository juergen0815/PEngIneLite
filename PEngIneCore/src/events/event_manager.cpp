/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "event_manager.h"

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

//#ifdef __LINUX__
#define NO_ASYNC_EVENTS
//#endif

using namespace pei;

#define LOCK_EM() boost::lock_guard< boost::recursive_mutex > lock( m_Lock );

static bool compareEventProcessorPtr ( const pei::EventProcessorPtr& a, const pei::EventProcessorPtr& b ) {
    return a->GetPriority() < b->GetPriority();
}

static bool isEqualEventProcessor( const pei::EventProcessorPtr& a, const pei::EventProcessorPtr& b ) {
    return a.get() == b.get();
}

EventProcessor::~EventProcessor()
{
    // if this thing goes out of scope, we better detach ourselfs here
    m_EventManager = pei::EventManagerPtr();
}

void EventProcessor::Unregister()
{
    if ( m_EventManager ) {
        // Must be NULL deleter! remove_if calls d'tor! Event handler MUST NOT delete EventProcessor!
        EventProcessorPtr ptr( this, NullDeleter() );
        // unregister all event handlers
        m_EventManager->UnregisterEventHandler( ptr );
        m_EventManager->UnregisterEventProcessor( ptr );
    }
    m_EventManager = pei::EventManagerPtr();
}

void EventProcessor::SetPriority( double pri, bool sort /*= true*/ )
{
    // do not resort if priority does not change!
    if ( pri != m_Priority ) {
        m_Priority = pri;
        if ( sort && m_EventManager ) {
            m_EventManager->SortEventPriorities();
        }
    }
}


EventManager::EventManager( en_PollType type )
: m_PollType( type )
, m_MainLoopRunning(false)
, m_StartTicks(0)
, m_UpdateType(AUTO_UPDATE)
{
}

EventManager::~EventManager()
{
}

void pei::EventManager::SetEventProfile( EventProfilePtr profile )
{
    m_EventProfile = profile;
    m_EventProfile->LinkEventManager( EventManagerPtr( m_WeakSelf ) );
}

void pei::EventManager::Release()
{
    // do not kill render manager here - might have pending events. Killing render manager
    // might destroy objects which are attached to an event handler
    LOCK_EM();

    // remove all event handlers - make sure render nodes outlive event handlers!
    m_SyncHandlers.clear();
    m_AsyncHandlers.clear();
    m_SyncEventProcessors.clear();
    m_AsyncEventProcessors.clear();

    // we always need to shut down the render manager - no matter if auto update or not - unless the render manager is not attached at all
    if ( m_RenderManager ) {
        m_RenderManager->ShutDown();
        m_RenderManager->WaitThreadFinished();
    }
    m_RenderManager = pei::RenderManagerInterfacePtr();
    m_EventProfile = pei::EventProfilePtr();
}

void pei::EventManager::SetRendeManager( RenderManagerInterfacePtr renderManager )
{
    m_RenderManager = renderManager;
    if ( m_UpdateType == AUTO_UPDATE && renderManager.get() && renderManager->StartRenderLoop() == false ) {
    	m_PollType = BUSY_POLLING;
    }
}


void pei::EventManager::SetAutoUpdate( en_RefreshType type )
{
	m_UpdateType = type;
}

int pei::EventManager::MainLoop( double startTicks /*= 0*/ )
{
    m_StartTicks = startTicks;
	if ( m_EventProfile ) {
		EventManagerPtr pSelf = EventManagerPtr( m_WeakSelf );
		m_MainLoopRunning = true;
		// could be a functor or something - but either loop should be as fast as possible.
		// Low overhead with explicit implementation
		if ( m_PollType == BUSY_POLLING ) {
			// renderer runs within event loop
			while ( m_MainLoopRunning ) {
				if ( m_UpdateType == AUTO_UPDATE && m_RenderManager.get() != NULL ) {
					// must lock! Async events can clear render tree!
					m_RenderManager->Lock();
					m_StartTicks = m_RenderManager->RenderFrame( m_StartTicks );
					m_RenderManager->Unlock();
				}
				pei::EventPtr event = PollEvent();
				while ( event ) {
					m_EventProfile->ProcessEvent( pSelf, event );
					event = PollEvent();
				};
			}
		}
		else if ( m_PollType == WAIT_EVENT ) {
			// runs separate render thread
			do {
				pei::EventPtr event = WaitEvent();
				do {
					m_EventProfile->ProcessEvent( pSelf, event );
					event = PollEvent();
				} while ( event );
			} while ( m_MainLoopRunning );
		}
	}

    if ( m_RenderManager ) {
        m_RenderManager->ShutDown();
    }
    Release();
    return 0;
}

pei::EventPtr pei::EventManager::PollEvent()
{
    if ( m_EventProfile ) {
        return m_EventProfile->PollEvent();
    }
    return pei::EventPtr();
}

pei::EventPtr pei::EventManager::WaitEvent()
{
    if ( m_EventProfile ) {
        return m_EventProfile->WaitEvent();
    }
    return pei::EventPtr();
}

bool pei::EventManager::ProcessEvent( EventPtr event )
{
	bool r = ProcessSyncEvents( event );
    if (!r) {
#ifdef NO_ASYNC_EVENTS
        r = ProcessAsyncEvents( event );
#else
        // do not spawn excessive threads if nobody is listening to async events
        if ( m_AsyncHandlers.size() || m_AsyncEventProcessors.size() ) {
            boost::shared_ptr<boost::thread>( new boost::thread( &EventManager::ProcessAsyncEvents, this, event ) );
        }
#endif
    }
    return r;
}

bool pei::EventManager::ProcessAsyncEvents( EventPtr event )
{
	// we run the event list in reverse because we draw from bottom up if the z-order is equal
    LOCK_EM();

	// Do not create a thread per event - is hard to synchronize later.
	// E.g drop 5 files and you'll get 5 threads loading assets - trying to update the tree 5 times - and maybe spawning child threads...ends in a mess!
    // ref to smart_ptr is fine...we won't go out of scope while we are in here

    // per event handlers - only these are called async
    EventHandlerMap::iterator fit = m_AsyncHandlers.find( event->GetType() );
    if ( fit != m_AsyncHandlers.end() ) {
        EventHandlerList& eventProcessors = fit->second;
        EventHandlerList::reverse_iterator eit = eventProcessors.rbegin();
        while ( eit != eventProcessors.rend() ) {
            EventProcessorPtr& processor = *eit++;
            // default async method. only spawn thread for events which request that - no global async events
#ifdef NO_ASYNC_EVENTS
            boost::shared_ptr<boost::thread>( new boost::thread( &pei::EventProcessor::Call, processor, m_RenderManager, event ));
#else
           if ( processor->Call( *this, m_RenderManager, event ) ) {
               return true ;
           }
#endif
        }
    }

    // Async Event Handling - this should probably be called before sync - but will introduce lag!
	EventHandlerList::reverse_iterator eit = m_AsyncEventProcessors.rbegin();
	while ( eit != m_AsyncEventProcessors.rend() ) {
		EventProcessorPtr& processor = *eit++;
        if ( processor->Call( m_RenderManager, event ) ) {
            return true;
        }
	}
    return false;
}

bool pei::EventManager::ProcessSyncEvents( EventPtr event )
{
	// we run the event list in reverse because we draw from bottom up if the z-order is equal
    LOCK_EM();

	// ref to smart_ptr is fine...we won't go out of scope while we are in here
    EventHandlerMap::iterator fit = m_SyncHandlers.find( event->GetType() );
    if ( fit != m_SyncHandlers.end() ) {
        EventHandlerList& eventProcessors = fit->second;
        EventHandlerList::reverse_iterator eit = eventProcessors.rbegin();
        while ( eit != eventProcessors.rend() ) {
            EventProcessorPtr& processor = *eit++;
            if ( processor->Call( m_RenderManager, event ) ) {
                return true;
            }
        }
    }

    // Sync Event Handling
    EventHandlerList::reverse_iterator eit = m_SyncEventProcessors.rbegin();
    while ( eit != m_SyncEventProcessors.rend() ) {
        EventProcessorPtr& processor = *eit++;
        if ( processor->Call( m_RenderManager, event ) ) {
            return true;
        }
    }
    return false;
}

void pei::EventManager::Exit()
{
    // call from event handler to terminate main loop
    m_MainLoopRunning = false;
}

void pei::EventManager::AddEventProcessor( const EventProcessorPtr& eventProcessor )
{
    // link to self
    eventProcessor->LinkSmartPtr( eventProcessor );
    // ref to smart_ptr is fine...we won't go out of scope while we are in here
    eventProcessor->SetEventManager( EventManagerPtr( m_WeakSelf ) );
    LOCK_EM();
    m_SyncEventProcessors.push_back( eventProcessor );
    m_SyncEventProcessors.sort(compareEventProcessorPtr);
}

void pei::EventManager::AddAsyncEventProcessor( const EventProcessorPtr& eventProcessor )
{
    // link to self
    eventProcessor->LinkSmartPtr( eventProcessor );

    // ref to smart_ptr is fine...we won't go out of scope while we are in here
    eventProcessor->SetEventManager( EventManagerPtr( m_WeakSelf ) );
    LOCK_EM();
    m_AsyncEventProcessors.push_back( eventProcessor );
    m_AsyncEventProcessors.sort(compareEventProcessorPtr);
}

void pei::EventManager::AddEventProcessor( const EventProcessor::OnEventFunc& onProcessEvent, double pri /* = 0 */ )
{
    EventProcessorPtr ptr( new EventProcessor( pri, onProcessEvent ));
	AddEventProcessor( ptr );
}

void pei::EventManager::AddAsyncEventProcessor( const EventProcessor::OnEventFunc& onProcessEvent, double pri /* = 0 */ )
{
    EventProcessorPtr ptr( new EventProcessor( pri, onProcessEvent ));
	AddAsyncEventProcessor( ptr );
}

bool pei::EventManager::RegisterSyncEventHandler( const EventTypePtr key, const EventProcessorPtr& eventProcessor )
{
	if ( key ) {
		// link to self
		eventProcessor->LinkSmartPtr( eventProcessor );

		// ref to smart_ptr is fine...we won't go out of scope while we are in here
		eventProcessor->SetEventManager( EventManagerPtr( m_WeakSelf ) );

		LOCK_EM();

		// creates new key if not available!
		EventHandlerList& ehl = m_SyncHandlers[ key ];
		ehl.push_back( eventProcessor );
		ehl.sort(compareEventProcessorPtr);
		return true;
	}
	return false;
}

bool pei::EventManager::RegisterSyncEventHandler( const EventTypePtr key, const EventProcessor::OnEventFunc& event_handler, double pri /*= 0*/ )
{
	if ( key ) {
		EventProcessorPtr ep( new EventProcessor( pri, event_handler ));
		return RegisterSyncEventHandler( key, ep );
	}
	return false;
}

bool pei::EventManager::RegisterAsyncEventHandler( const EventTypePtr key, const EventProcessorPtr& eventProcessor )
{
	if ( key ) {
		// link to self
		eventProcessor->LinkSmartPtr( eventProcessor );

		// ref to smart_ptr is fine...we won't go out of scope while we are in here

		eventProcessor->SetEventManager( EventManagerPtr( m_WeakSelf ) );

		LOCK_EM();

		EventHandlerList& ehl = m_AsyncHandlers[ key ];
		ehl.push_back( eventProcessor );
		ehl.sort(compareEventProcessorPtr);

		return true;
	}
	return false;
}

bool pei::EventManager::RegisterAsyncEventHandler( const EventTypePtr key, const EventProcessor::OnEventFunc& event_handler, double pri /*= 0*/ )
{
	if ( key ) {
		EventProcessorPtr ep( new EventProcessor( pri, event_handler ));
		return RegisterAsyncEventHandler( key, ep );
	}
	return false;
}

void pei::EventManager::SortEventPriorities()
{
    LOCK_EM();

    m_SyncEventProcessors.sort(compareEventProcessorPtr);
    m_AsyncEventProcessors.sort(compareEventProcessorPtr);

    EventHandlerMap::iterator sit = m_SyncHandlers.begin();
    for ( ; sit != m_SyncHandlers.end(); sit++ ) {
        EventHandlerList& eventProcessors = sit->second;
        eventProcessors.sort(compareEventProcessorPtr);
    }

    EventHandlerMap::iterator ait = m_AsyncHandlers.begin();
    for ( ; ait != m_AsyncHandlers.end(); ait++ ) {
        EventHandlerList& eventProcessors = ait->second;
        eventProcessors.sort(compareEventProcessorPtr);
    }
}

bool pei::EventManager::UnregisterEventProcessor( const EventProcessorPtr& eventProcessor )
{
    LOCK_EM();

    m_SyncEventProcessors.remove_if( boost::bind( &isEqualEventProcessor, _1, eventProcessor ) );
    m_AsyncEventProcessors.remove_if( boost::bind( &isEqualEventProcessor, _1, eventProcessor ) );
    return true;
}

bool pei::EventManager::UnregisterEventHandler( const EventProcessorPtr& eventProcessor )
{
    LOCK_EM();

    bool r(false);
    EventHandlerMap::iterator fit = m_SyncHandlers.begin();
    while ( fit != m_SyncHandlers.end() ) {
        EventHandlerList& ehl = fit->second;
        ehl.remove_if( boost::bind( &isEqualEventProcessor, _1, eventProcessor )  );
        EventHandlerMap::iterator erasor = fit++;
        if ( ehl.empty( ) ) {
            m_SyncHandlers.erase(erasor);
        }
        r |= true;
    }
    fit = m_AsyncHandlers.begin();
    while ( fit != m_AsyncHandlers.end() ) {
        EventHandlerList& ehl = fit->second;
        ehl.remove_if( boost::bind( &isEqualEventProcessor, _1, eventProcessor )  );
        EventHandlerMap::iterator erasor = fit++;
        if ( ehl.empty( ) ) {
            m_AsyncHandlers.erase(erasor);
        }
        r |= true;
    }
    return r;
}

bool pei::EventManager::UnregisterEventHandler( const pei::EventTypePtr key )
{
    LOCK_EM();

    bool r(false);
    EventHandlerMap::iterator fit = m_SyncHandlers.find( key );
    if ( fit != m_SyncHandlers.end() ) {
        m_SyncHandlers.erase(fit);
        r |= true;
    }
    fit = m_AsyncHandlers.find( key );
    if ( fit != m_AsyncHandlers.end() ) {
        m_AsyncHandlers.erase(fit);
        r |= true;
    }
    return r;
}
