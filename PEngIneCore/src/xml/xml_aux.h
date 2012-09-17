/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef XML_AUX_H
#define XML_AUX_H

#include <interfaces/xml_interface.h>
#include <xml/xml_element.h>

#include <map>
#include <string>
#include <vector>

#include <stdio.h>
#include <string.h>

namespace pei {
    class XmlNode;

    class XmlAux;
    typedef boost::shared_ptr<XmlAux> XmlAuxPtr;

    class XmlAux : public pei::XmlElement
    {
    protected:
        // a map doesn't work for this app, because we do not want to re-sort the attributes
        std::map< std::string, std::string >  m_attributes;
        std::vector<std::string>              m_text_section;
    public:
        XmlAux( );
        virtual ~XmlAux();

        // XmlInterface implementation
        virtual pei::XmlInterfacePtr xmlCreate() const;

        virtual bool xmlSetAttribute( const char* attribute, const char* value );

        virtual void xmlHandleTextSection( const char* text );

        virtual const char* xmlGetAttribute( const char* name ) const;

        virtual std::string xmlGetText( size_t num = 0 ) const;

        //
        //////////////////////////////////////////////////////////////////////////
    };


    class XmlTagValidator;
    typedef boost::shared_ptr<XmlTagValidator> XmlTagValidatorPtr;

    class XmlTagValidator : public XmlElement
    {
    protected:
        std::string m_Name;
    public:

        XmlTagValidator( const char* name = "VALID_XML" ) : m_Name(name) {}

        virtual ~XmlTagValidator() {}

        virtual const char* GetName() const { return m_Name.c_str(); }

        virtual bool IsTag( const char* name ) { return (strcmp( xmlGetName(), name ) == 0 ); }

        static bool IsValid( pei::XmlInterfacePtr xi ) {
            pei::XmlTagValidatorPtr e = boost::dynamic_pointer_cast<pei::XmlTagValidator>(xi);
            return ( e && (strcmp( e->xmlGetName(), e->GetName() ) == 0 ));
        }
    };

} // namespace pei

#endif // XML_AUX_H
