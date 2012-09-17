/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef EVENTS_MANAGER_H_
#define EVENTS_MANAGER_H_

#include <interfaces/render_manager_interface.h>

#include <events/event.h>
#include <events/event_profile.h>

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace pei
{
    class EventManager;
    typedef boost::shared_ptr<EventManager> EventManagerPtr;

    class EventProcessor;
    typedef boost::shared_ptr<EventProcessor> EventProcessorPtr;

    class EventProcessor
    {
    public:
        typedef boost::function<bool( EventManagerPtr, RenderManagerInterfacePtr, EventPtr )> OnEventFunc;

        typedef boost::function<bool( EventManagerPtr )> OnPriorityChangedFunc;

    private:
        struct NullDeleter {
            void operator()( void const* ) const
            {
            }
        };
        EventProcessor( const EventProcessor& o );

    protected:
        friend class EventManager;

        double 		                m_Priority;
        bool                        m_Enabled;
        OnEventFunc                 m_Func;

        mutable EventManagerPtr     m_EventManager; // set when added to list from EventManager itself

        mutable boost::weak_ptr<EventProcessor> m_WeakSelf; // need to call with const...

        void LinkSmartPtr( EventProcessorPtr self ) const {
            m_WeakSelf = self;
        }

        EventProcessorPtr Get() {
            EventProcessorPtr smartSelf;
            if ( m_WeakSelf.expired() ) {
                // we create a temporary smart ptr. As soon as this is linked into the scene graph, this pointer will become valid
                smartSelf = EventProcessorPtr( this, NullDeleter() );
                LinkSmartPtr( smartSelf );
            } else {
                smartSelf = EventProcessorPtr( m_WeakSelf );
            }
            return smartSelf;
        }
     public:
        EventProcessor( double pri, const OnEventFunc& handler )
            : m_Priority(pri), m_Enabled(true), m_Func( handler ) {}

        virtual ~EventProcessor();

        virtual void Unregister();

        virtual void SetEventManager( EventManagerPtr eventManager ) const {
            m_EventManager = eventManager;
        }

        virtual bool operator<( const EventProcessor& p ) const {
            return m_Priority < p.m_Priority;
        }

        virtual void SetPriority( double pri, bool sort = true );

        virtual double GetPriority() const { return m_Priority; }

        virtual void Enable( bool enable = true ) { m_Enabled = enable; }

        virtual bool IsEnabled() const { return m_Enabled; }

        virtual bool Call( RenderManagerInterfacePtr rmi , EventPtr e ) {
            if ( m_Func && m_Enabled ) {
                return m_Func(m_EventManager,rmi,e);
            }
            return false;
        }
    };

    class EventManager
    {
        friend class EventProcessor;
        struct NullDeleter {
            void operator()( void const* ) const
            {
            }
        };
    public:
        enum en_PollType {
            BUSY_POLLING = 0,
            WAIT_EVENT   = 1
        } ;
        enum en_RefreshType {
        	NO_UPDATE = 0,
        	AUTO_UPDATE,
        };


    protected:
        friend class EventProfile;

        boost::recursive_mutex  m_Lock;

        // EventProcessor must be smart_ptr!!!
        struct CompareEventProcessorPtr {
            bool operator () ( const pei::EventProcessorPtr& a, const pei::EventProcessorPtr& b ) const {
                return a->GetPriority() < b->GetPriority();
            }
        };
        typedef std::list< EventProcessorPtr > EventHandlerList;

        struct CompareEventTypePtr {
            bool operator () ( const pei::EventTypePtr& a, const pei::EventTypePtr& b ) const {
                return a->LessThan(b);
            }
        };
        typedef std::map< EventTypePtr, EventHandlerList, CompareEventTypePtr > EventHandlerMap;

        EventHandlerList               m_SyncEventProcessors;
        EventHandlerList               m_AsyncEventProcessors;

        /** synchronized event handlers - will return true if processed */
        EventHandlerMap                m_SyncHandlers;
        /** asynchronous event handler - will always propagate! */
        EventHandlerMap                m_AsyncHandlers;
        /** we store async events threads to sync them before we process events again */
        // std::deque< boost::shared_ptr<boost::thread> > m_AsyncThreadList;

        en_PollType                    m_PollType;
        bool                           m_MainLoopRunning;
        boost::weak_ptr<EventManager>  m_WeakSelf;

        EventProfilePtr                m_EventProfile;
        RenderManagerInterfacePtr      m_RenderManager;

        // rendermanager is handled by event handler. If render manager does not run a render thread
        // we run synchronously with the ProcessEvent() handling in the main thread. Use async event
        // handling and polling in that case (e.g. MacOS)
        double 						   m_StartTicks;

        // if this is set to AUTO_UPDATE, EventManager takes care of calling render manager
        en_RefreshType                 m_UpdateType;

        // must be created by factory! There can only be one!!
        EventManager( en_PollType type );

        void InitSharedPtr( EventManagerPtr self ) {
            m_WeakSelf = self;
        }
    public:
        static EventManagerPtr Create( en_PollType type = WAIT_EVENT ) {
            EventManagerPtr self( new EventManager( type )) ;
            self->InitSharedPtr( self );
            return self;
        }
	    virtual ~EventManager();

        virtual void SetEventProfile( EventProfilePtr profile );

        virtual void Release();

        virtual void AddEventProcessor( const EventProcessor::OnEventFunc& onProcessEvent, double pri = 0 );

        virtual void AddAsyncEventProcessor( const EventProcessor::OnEventFunc& onProcessEvent, double pri = 0 );

        virtual void AddEventProcessor( const EventProcessorPtr& eventProcessor );

        virtual void AddAsyncEventProcessor( const EventProcessorPtr& eventProcessor );

        virtual bool RegisterSyncEventHandler( const EventTypePtr key, const EventProcessor::OnEventFunc& event_handler, double pri = 0 );

        virtual bool RegisterAsyncEventHandler( const EventTypePtr key, const EventProcessor::OnEventFunc& event_handler, double pri = 0 );

        virtual bool RegisterSyncEventHandler( const EventTypePtr key, const EventProcessorPtr& eventProcessor );

        virtual bool RegisterAsyncEventHandler( const EventTypePtr key, const EventProcessorPtr& eventProcessor );


        // searched for all (!) event handlers
        virtual bool UnregisterEventProcessor( const EventProcessorPtr& eventProcessor );

        // searched for all (!) event handlers
        virtual bool UnregisterEventHandler( const EventTypePtr key );

        virtual bool UnregisterEventHandler( const EventProcessorPtr& eventProcessor );

        // needed to lock rendering while handling events
        virtual void SetRendeManager( RenderManagerInterfacePtr renderManager );

        virtual void SetAutoUpdate( en_RefreshType type );

        virtual int MainLoop( double startTicks = 0 );

        virtual EventPtr PollEvent();

        virtual EventPtr WaitEvent();

        virtual void Exit();

    protected:
        virtual bool ProcessEvent( EventPtr event );

        virtual bool ProcessSyncEvents( EventPtr event );

        virtual bool ProcessAsyncEvents( EventPtr event );

        // sort all event priorities - necessary if e.g. ZOrder changes. This is an expensive method!
        virtual void SortEventPriorities();
    };

} /* namespace peiTesting */
#endif /* EVENTS_MANAGER_H_ */
