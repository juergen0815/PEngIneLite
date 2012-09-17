/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "xml_node.h"

#include <string.h>

using namespace pei;


bool XmlNode::compareTag( NodeInterface* ni, const char* what )
{
	XmlInterfacePtr xi;
    const XmlNode* xmlNode = dynamic_cast<const XmlNode*>(ni);
    if ( xmlNode && ( xi = xmlNode->getObject()) != NULL )
    {
        const char* c = xi ->xmlGetName();
        return strcmp(c, what ) == 0;
    }
    return false;
}

XmlNode::~XmlNode()
{
}

XmlInterfacePtr XmlNode::getObject() const
{
	return m_xml_object;
}

void XmlNode::setObject( XmlInterfacePtr object )
{
	m_xml_object = object;
}

bool XmlNode::lesserThen( const XmlNode* other ) const
{
	return strcmp( getName(), other->getName() ) < 0;  // this must return a valid result, or the sorter will loop
}

const char* pei::XmlNode::getName() const
{
    if ( m_xml_object ) {
        const char* c = m_xml_object->xmlGetAttribute("name");
        if ( strlen(c) > 0 ) {
            return c;
        }
    }
    return Node<XmlNode>::getName();
}
