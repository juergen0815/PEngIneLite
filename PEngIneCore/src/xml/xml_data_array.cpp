/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "xml_data_array.h"

#include <utils/utils.h>

namespace pei {

    XmlDataArray::XmlDataArray( const char* separator /*= "," */, int type /*= TP_FLOAT */)
        : m_Stride(1), m_Type(type), m_Separator(separator)
    {
        switch ( type )
        {
        case TP_BYTE:
            m_DataContainer = DataContainerInterfacePtr(new ByteDataContainer());
            break;
        case TP_SHORT:
            m_DataContainer = DataContainerInterfacePtr(new ShortDataContainer());
            break;
        case TP_INT:
#if defined _HAS_OPENGL_ || defined _HAS_EGL2_
            m_DataContainer = DataContainerInterfacePtr(new IntDataContainer());
#else
            m_DataContainer = DataContainerInterfacePtr(new ShortDataContainer());
#endif
            break;
        case TP_FLOAT:
            m_DataContainer = DataContainerInterfacePtr(new FloatDataContainer());
            break;
        case TP_DOUBLE:
#ifdef _HAS_OPENGL_
            m_DataContainer = DataContainerInterfacePtr(new DoubleDataContainer());
#else
            m_DataContainer = DataContainerInterfacePtr(new FloatDataContainer());
#endif
            break;
        default:
            break;
        }
    }

    DataContainerInterfacePtr XmlDataArray::GetDataContainer()
    {
        return m_DataContainer;
    }
#if 0
    const char* XmlDataArray::asByteArray() const
    {
        ByteDataContainerPtr fdc = as<ByteDataContainer>();
        if ( fdc ) {
            return &fdc->getData()[0];
        }
        return NULL;
    }

    const short* XmlDataArray::asShortArray() const
    {
        ShortDataContainerPtr fdc = as<ShortDataContainer>();
        if ( fdc ) {
            return &fdc->getData()[0];
        }
        return NULL;
    }

    const int* XmlDataArray::asIntArray() const
    {
        IntDataContainerPtr fdc = as<IntDataContainer>();
        if ( fdc ) {
            return &fdc->getData()[0];
        }
        return NULL;
    }

    const float* XmlDataArray::asFloatArray() const
    {
        FloatDataContainerPtr fdc = as<FloatDataContainer>();
        if ( fdc ) {
            return &fdc->getData()[0];
        }
        return NULL;
    }
#endif

#if 0
    const double* XmlDataArray::asDoubleArray() const
    {
        DoubleDataContainerPtr fdc = as<DoubleDataContainer>();
        if ( fdc ) {
            return &fdc->getData()[0];
        }
        return NULL;
    }
#endif

    bool XmlDataArray::xmlSetAttribute( const char* attribute, const char* value )
    {
        bool r = XmlAux::xmlSetAttribute( attribute, value );
        if ( strcmp( attribute, "type" ) == 0 ) {
            if ( strcmp( value, "float") == 0 )
            {
                m_DataContainer = DataContainerInterfacePtr(new FloatDataContainer());
                m_Type = TP_FLOAT;
                r = true;
            }
            else if ( strcmp( value, "double") == 0 )
            {
                m_DataContainer = DataContainerInterfacePtr(new DoubleDataContainer());
                m_Type = TP_DOUBLE;
                r = true;
            }
            else if ( strcmp( value, "byte") == 0 )
            {
                m_DataContainer = DataContainerInterfacePtr(new ByteDataContainer());
                m_Type = TP_BYTE;
                r = true;
            }
            else if ( strcmp( value, "short") == 0 )
            {
                m_DataContainer = DataContainerInterfacePtr(new ShortDataContainer());
                m_Type = TP_SHORT;
                r = true;
            }
            else if ( strncmp( value, "int", 3) == 0 )
            {
                m_DataContainer = DataContainerInterfacePtr(new IntDataContainer());
                m_Type = TP_INT;
                r = true;
            }
        }
        else if ( strcmp( attribute, "stride") == 0 ) {
            m_Stride = atoi( value );
            r = true;
        }
        else if ( strncmp( attribute, "separator", 3 ) == 0 ) {
            m_Separator = value;
            r = true;
        }
        return r;
    }

    void XmlDataArray::xmlHandleTextSection( const char* text )
    {
        if ( m_text_section.size() == 0 ) {
            m_text_section.push_back( std::string() );
        }
        std::string& buffer = m_text_section[0];
        buffer += text;
    }

    bool XmlDataArray::xmlInit( const char* base, XmlNode *node /*= NULL*/ )
    {
        if ( m_DataContainer != NULL ) {
            int line(0);
            std::string text = xmlGetText(line++);
            while ( text.length() > 0) {
                std::vector<std::string> values = pei::split( text.c_str(), m_Separator.c_str(), false );
                std::vector<std::string>::iterator it = values.begin();
                while ( it != values.end() ) {
                    const std::string& s = *it++;
                    m_DataContainer->add( s );
                }
                text = xmlGetText(line++);
            }
        }
        return true;
    }

} // namespace pei
