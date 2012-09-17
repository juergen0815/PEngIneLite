/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef XML_NODE_H
#define XML_NODE_H

#include <tree/node.h>
#include <interfaces/xml_interface.h>

namespace pei {
	class XmlNode;

    class XmlNode : public Node<XmlNode>
    {
    protected:
        /* link to object attached to xml node */
       XmlInterfacePtr m_xml_object;

    public:
     	static bool compareTag( NodeInterface* ni, const char* val );

        XmlNode() : Node<XmlNode>( "XmlNode" ) {}

        virtual ~XmlNode();

        template< class T >
        boost::shared_ptr<T> as() const {
            return boost::dynamic_pointer_cast<T>( getObject() );
        }

        virtual XmlInterfacePtr getObject() const;

        virtual void setObject( XmlInterfacePtr object );

        virtual bool lesserThen( const XmlNode* other ) const;

         virtual const char* getName() const;

        virtual XmlNode* getChild( int num ) { return static_cast<XmlNode*>(Node<XmlNode>::getChild(num)); }
        virtual const XmlNode* getChild( int num ) const { return static_cast<const XmlNode*>(Node<XmlNode>::getChild(num)); }
        virtual XmlNode* findChild( const char* name ) const { return static_cast<XmlNode*>(Node<XmlNode>::findChild(name)); }
        virtual XmlNode* findNextChild( const char* name ) const { return static_cast<XmlNode*>(Node<XmlNode>::findNextChild(name)); }

        virtual XmlNode* findChild( const char* what, const NodeInterface::CompareFunc& comp ) const {
        	return static_cast<XmlNode*>(Node<XmlNode>::findChild( what, comp)); }
        virtual XmlNode* findNextChild( const char* what, const NodeInterface::CompareFunc& comp ) const {
        	return static_cast<XmlNode*>(Node<XmlNode>::findNextChild(what,comp)); }

    };

} // namespace pei

#endif /*XML_NODE_H*/
