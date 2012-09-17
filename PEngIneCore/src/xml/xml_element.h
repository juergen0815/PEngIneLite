/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef XML_ELEMENT_H
#define XML_ELEMENT_H

#include <tree/node.h>
#include <interfaces/xml_interface.h>

#include <string>

#include <boost/function.hpp>

namespace pei {
    class XmlElement;
    typedef boost::shared_ptr<XmlElement> XmlElementPtr;

	class XmlElement : public XmlInterface
	{
    public:
    protected:
        std::string                   m_Name;
		boost::weak_ptr<XmlInterface> m_WeakPtr;

		virtual XmlInterfacePtr LinkSmartPtr( XmlInterfacePtr ptr );

    public:
        XmlElement( const char* name = "XmlElement" ) : m_Name ( name ) { }
        virtual ~XmlElement();

		//////////////////////////////////////////////////////////////////////////
		// XML Interface

        virtual void xmlOnCreate( XmlInterfacePtr ifp );

        // this is useless...
		virtual XmlInterfacePtr xmlCreate() const;

		virtual bool xmlSetAttribute( const char* attribute, const char* value );

		virtual const char* xmlGetAttribute( const char* attribute ) const { return ""; }

		virtual const char* xmlGetName() const;

		virtual bool xmlInit( const char* base, XmlNode *node = NULL ) { return true; }

		virtual void xmlHandleTextSection( const char* text ) {};

		virtual std::string xmlGetText( size_t num = 0 ) const { return ""; }

	#ifdef _DEBUG
		virtual size_t xmlGetObjectSize() { return sizeof(XmlElement); };
	#endif

	};

} // namespace pei

#endif /*XML_ELEMENT_H*/
