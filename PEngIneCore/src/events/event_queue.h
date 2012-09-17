/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <events/event.h>

#include <queue>

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>

namespace pei {

    class EventQueue
    {
    private:
        std::queue< EventPtr >      m_Queue;
        mutable boost::mutex        m_Lock;
        boost::condition_variable   m_ConditionVar;
    public:
        /*
         *
         */
        void Send( EventPtr const& data);

        /*
         *
         */
        bool IsEmpty() const;

        /*
         *
         */
        EventPtr Poll( );

        /*
         *
         */
        EventPtr Wait();

    };

} // namespace pei

#endif // EVENT_QUEUE_H
