/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef XML_PARSER_H_
#define XML_PARSER_H_

#include <expat.h>

#include <xml/xml_node.h>
#include <xml/xml_element.h>
#include <xml/xml_object_factory.h>

#include <tree/tree_iterator.h>

#include <boost/shared_ptr.hpp>

namespace pei
{
    class XmlParser
    {
    public:    
        XmlParser();
        ~XmlParser();
        
        bool init( );
        bool init( XmlObjectFactoryPtr factory);
        bool reset( bool destroy_tree = true );
        void close( bool destroy_tree = true );

        /* set a default element in case a proper element tag does not
         * exist in the tree. This is a temporary element and must implement
         * its own Create() function. It will not be used directly, but
         * allows you to implement a "virtual" ctor. Note: This is not
         * the same as Clone() - which would clone the existing object
         * (incl. children), but rather creates a new empty object */    
        void setDefaultElement( XmlInterfacePtr default_element );
        void destroyTree();

        bool parseData( const char *data, size_t len, bool done );
        bool parseFile( const char *file );

        int getErrorLineNumber();

        const char* getErrorString();
        
        /* verifies a XML tree against a certain type of object - which is not (just) an XmlInterface.
        e.g. If you want to make sure, that all elements in the XML tree are */
        template< class T > 
        static bool verify( XmlParser& self );

        XmlNode* getRootNode() {
            return m_root;
        }

	    XmlObjectFactoryPtr getFactory() { return m_xml_factory; }

        void setSortFlag( bool sort_children = true ) { m_sort_children = sort_children; }

    protected:  
        static bool parseFile( XML_Parser parser, const char *file );
        static bool parseData( XML_Parser parser, const char *data, size_t len, bool done );

        static int  parseEntity(XML_Parser parser_in, const char* context, const char* base,
                                const char* system_id, const char* publicId);

        static void skipEntity ( void *userData, const XML_Char *entityName, int is_parameter_entity);

        static int unknownEncodingHandler( void *encodingHandlerData, const XML_Char *name, XML_Encoding *info);

        void tagStart( const XML_Char *el, const XML_Char **attr );
        void tagEnd(const XML_Char *el);
        
        static void cbTagStart(void *data, const XML_Char *el, const XML_Char **attr); 
        static void cbTagEnd(void *data, const XML_Char *el); 

        void characterDataHandler( const XML_Char *s, int len );

        static void cbCharacterDataHandler(void *data, const XML_Char *s, int len); 
    protected:
        XML_Parser  m_parser;
        long        m_depth;

        // this tree is private! No shared ptr here!
        XmlNode    *m_element_node;
        XmlNode    *m_root;
                     
        XmlInterfacePtr m_default_element;

        bool        m_element_active;
        bool        m_sort_children;

        XmlObjectFactoryPtr m_xml_factory;
    };

    typedef boost::shared_ptr<XmlParser> XmlParserPtr;

    template< class T > 
    bool XmlParser::verify( XmlParser& self )
    {
        bool valid = true;
        XmlNode *root = self.getRootNode( );
        if ( root ) {
            pei::TreeIterator<pei::XmlNode*> it( root );
            while ( !it.isNull() ) {
                // this usually never fails
                pei::XmlNode *node = *it++;
                if ( node && node->getObject() ) {
                    T el = dynamic_cast<T>( node->getObject() );
                    if (!el) { 
                        valid = false;
                        break;
                    }
                } else {
                    valid = false;
                    break;
                }
            }
        } else {
            valid = false;
        }
        return valid;
    }

}; // namespace pei

#endif /*XML_PARSER_H_*/
