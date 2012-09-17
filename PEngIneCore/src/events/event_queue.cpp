/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "event_queue.h"

#include <queue>

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>

namespace pei {

    void EventQueue::Send( EventPtr const& data)
    {
        m_Lock.lock();
        m_Queue.push(data);
        m_Lock.unlock();
        m_ConditionVar.notify_one();
    }

    bool EventQueue::IsEmpty() const
    {
        boost::mutex::scoped_lock lock(m_Lock);
        return m_Queue.empty();
    }

    EventPtr EventQueue::Poll( )
    {
        boost::mutex::scoped_lock lock(m_Lock);
        if (m_Queue.empty() )
        {
            return EventPtr();
        }

        EventPtr event = m_Queue.front();
        m_Queue.pop();
        return event;
    }

    EventPtr EventQueue::Wait()
    {
        boost::mutex::scoped_lock lock(m_Lock);
        while(m_Queue.empty())
        {
            m_ConditionVar.wait(lock);
        }
        EventPtr event = m_Queue.front();
        m_Queue.pop();
        return event;
    }

} // namespace pei
