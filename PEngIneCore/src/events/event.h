/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef EVENT_H
#define EVENT_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>

namespace pei
{
    // interface class to implement event comparators
    class EventType;
    typedef boost::shared_ptr<EventType> EventTypePtr;

    class EventType
    {
    public:
        virtual ~EventType();

        virtual bool IsEqual(const EventTypePtr& o) const = 0;

        virtual bool LessThan(const EventTypePtr& o) const = 0;

    };

    // interface class to hold event data
    class Event;
    typedef boost::shared_ptr<Event> EventPtr;

    class Event {
    public:
        virtual ~Event();

        // ref makes sure you don't allocate an object in this method's implementation
        virtual EventTypePtr GetType() const = 0;

        // events will be processed by priority!
        virtual void SetPriority( double pri ) = 0;

        virtual double GetPriority() const = 0;
    };

    template<class E>
    class EventT : public Event {
    protected:
        E            m_Event;
        double       m_Priority;
    public:
        EventT( E event, double pri = 0 ) : m_Event(event), m_Priority(pri) {}

        EventT( double pri = 0 ) : m_Priority(pri) {}

        virtual ~EventT() {}

        // events will be processed by priority!
        virtual void SetPriority( double pri ) { m_Priority = pri; }

        virtual double GetPriority() const { return m_Priority; }

        const E& GetEvent() const { return m_Event; }

        E& GetEvent() { return m_Event; }

    };

} // namespace pei

#endif // EVENT_H

