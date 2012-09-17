/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "xml_aux.h"

namespace pei {

    XmlAux::XmlAux()
    {}

    XmlAux::~XmlAux()
    {}

    pei::XmlInterfacePtr XmlAux::xmlCreate() const
    {
        return pei::XmlInterfacePtr(new XmlAux());
    }

    bool XmlAux::xmlSetAttribute( const char* attribute, const char* value )
    {
        if ( attribute && value ) {
            XmlElement::xmlSetAttribute( attribute, value );
            m_attributes[ attribute ] = value;
            return true;
        }
        return false;
    }

    void XmlAux::xmlHandleTextSection( const char* text )
    {
        if ( text ) {
            // filter text buffer and store the result for later
            std::string buffer;
            while ( *text != 0 ) {
                // remove all tabs, LF and CR
                switch ( *text )
                {
                case 9:  // tab
                case 10: // '\n'
                case 13: // '\r'
                    break;
                default:
                    buffer += *text;
                    break;
                }
                text++;
            }
            if ( buffer.length() > 0 )
            {
                // trim whitespace at the beginning of string
                size_t start = buffer.find_first_not_of( " " );
                if ( start != std::string::npos ) {
                    m_text_section.push_back( buffer.substr( start ) );
                }
            }
        }
    }

    const char* XmlAux::xmlGetAttribute( const char* name ) const
    {
        std::map< std::string, std::string >::const_iterator fit = m_attributes.find( name );
        if ( fit != m_attributes.end() ) {
            return fit->second.c_str();
        }
        return "";
    }

    std::string XmlAux::xmlGetText( size_t num /*= 0*/ ) const
    {
        if ( num < m_text_section.size() )
        {
            return m_text_section[num];
        }
        return "";
    }

} // namespace pei
