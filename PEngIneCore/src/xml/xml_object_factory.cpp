/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "xml_object_factory.h"

#include "interfaces/xml_interface.h"

#include <iostream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

using namespace pei;

XmlObjectFactoryPtr XmlObjectFactory::m_self; // XmlObjectFactory::CreateFactory();

#ifdef XML_THREAD_SAFE
boost::mutex lock;
#endif

XmlObjectFactoryPtr XmlObjectFactory::CreateFactory() {
#ifdef XML_THREAD_SAFE
    boost::mutex::scoped_lock locker(lock);
#endif
    if ( m_self == NULL ) {
        m_self = XmlObjectFactoryPtr(new XmlObjectFactory());
    }
    return m_self;
}

XmlObjectFactory::~XmlObjectFactory()
{
}

void XmlObjectFactory::Register( const char* class_name, XmlObjectCreatorPtr creator )
{
    m_object_creator_map[ std::string(class_name) ] = creator;
}


void XmlObjectFactory::Clear()
{
    m_object_creator_map.clear();
}

XmlInterfacePtr XmlObjectFactory::CreateObject( const char* class_name )
{
    std::map<std::string, XmlObjectCreatorPtr >::iterator it = m_object_creator_map.find( class_name );
    if ( it != m_object_creator_map.end() )
    {
        XmlObjectCreatorPtr creator = (*it).second;
        if ( creator ) {
        	return creator->CreateObject();
        }
    }
    return XmlInterfacePtr();
}

pei::XmlInterfacePtr pei::XmlObjectFactory::GetDefaultObject()
{
    return pei::XmlInterfacePtr( new pei::XmlElement());
}
