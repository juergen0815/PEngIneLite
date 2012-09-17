/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "xml_element.h"
#include "xml_object_factory.h"

using namespace pei;

//XML_DECLARE_OBJECT( XmlElement, XmlElement );

XmlElement::~XmlElement()
{
}

XmlInterfacePtr XmlElement::LinkSmartPtr( XmlInterfacePtr ptr ) {
    m_WeakPtr = boost::weak_ptr<XmlInterface>( ptr );
    return ptr;
}

void XmlElement::xmlOnCreate( XmlInterfacePtr ifp )
{
	assert( ifp.get() == this );
	XmlInterface::xmlOnCreate( LinkSmartPtr( ifp ) );
}

XmlInterfacePtr XmlElement::xmlCreate() const
{
    return XmlInterfacePtr(new XmlElement());
}

const char* XmlElement::xmlGetName() const
{
    return m_Name.c_str();
}

bool XmlElement::xmlSetAttribute( const char* attribute, const char* value )
{
    if ( strcmp( attribute, "xmlTagName" ) == 0 )
    {
        m_Name = value;
        return true;
    }
    return false;
}
