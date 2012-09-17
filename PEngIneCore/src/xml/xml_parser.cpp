/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "xml_parser.h"

#include <interfaces/xml_interface.h>

#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace pei;

#define BUFFSIZE 32768

//#define LOG_ME
#ifdef LOG_ME
static char *logName = (char*)"log.xml";
static std::ofstream logFile;
#endif

void XmlParser::cbTagStart(void *data, const XML_Char *el, const XML_Char **attr)
{
    XmlParser *self = static_cast<XmlParser*>(data);
    self->tagStart( el, attr );
}

void XmlParser::cbTagEnd(void *data, const XML_Char *el)
{
    XmlParser *self = static_cast<XmlParser*>(data);
    self->tagEnd( el );
}


void XmlParser::cbCharacterDataHandler(void *data, const XML_Char *s, int len)
{
    XmlParser *self = static_cast<XmlParser*>(data);
    self->characterDataHandler( s, len );
}

XmlParser::XmlParser()
    : m_parser( NULL )
    , m_depth( 0 )
    , m_element_node( NULL )
    , m_root( NULL )
    , m_default_element( new XmlElement() )
    , m_element_active(false)
    , m_sort_children(false)
{
#ifdef LOG_ME
    if ( !logFile.is_open() ) {
        logFile.open( logName );
    }
#endif
}

XmlParser::~XmlParser()
{
    close( true );
#ifdef LOG_ME
    if ( logFile.is_open() ) {
        logFile.close();
    }
#endif
}

bool XmlParser::init( XmlObjectFactoryPtr factory )
{
    factory->Init();
    m_xml_factory = factory;
    init();
    return true;
}

bool XmlParser::init()
{
    if ( m_parser ) {
        close();
    }
    m_parser = XML_ParserCreate(NULL);
    if (!m_parser)
    {
        printf("Couldn't allocate memory for parser\n");
        return false;
    }
    XML_SetUserData( m_parser, static_cast<void*>(this) );
    XML_SetParamEntityParsing( m_parser, XML_PARAM_ENTITY_PARSING_ALWAYS );
    XML_UseForeignDTD( m_parser, XML_TRUE ); // ignore unknown/unresolved entity references
    XML_SetExternalEntityRefHandler( m_parser, parseEntity );
    XML_SetSkippedEntityHandler( m_parser, skipEntity );
    XML_SetUnknownEncodingHandler( m_parser, unknownEncodingHandler, this );
    XML_SetElementHandler(m_parser, cbTagStart, cbTagEnd);
    XML_SetCharacterDataHandler( m_parser, cbCharacterDataHandler );
    m_root         = NULL;
    m_element_node = NULL;

    if ( m_xml_factory == NULL ) {
        //         XmlObjectFactory::CreateFactory();
        m_xml_factory = XmlObjectFactory::GetFactory();
    }
    return true;
}

void XmlParser::close( bool destroy_tree /*= true*/  )
{
	m_element_node = NULL;
    if ( destroy_tree ) {
        destroyTree();
    }
    if (m_parser) {
        XML_SetUserData( m_parser, NULL );
        XML_ParserFree( m_parser );
        m_parser = NULL;
    }
}

bool XmlParser::reset( bool destroy_tree /*= true*/ )
{
    close( destroy_tree );
    return init( m_xml_factory );
}

void XmlParser::setDefaultElement( XmlInterfacePtr default_element )
{
    m_default_element = default_element;
}

void XmlParser::destroyTree( )
{
    if ( m_root ) {
        delete m_root;
        m_root = NULL;
    }
}

void XmlParser::tagStart(const XML_Char *el, const XML_Char **attr)
{
	// this is the target object this type creates
    XmlInterfacePtr object = m_xml_factory->CreateObject( (const char*)el );
    if ( !object ) {
        /* a default element can be any type of object which exports a valid XmlInterface.
           This can e.g. implement an error handler, etc. */
#if SHOW_WARNING
        fprintf( stderr, "No ObjectCreator for element '%s' or no XmlInterface.\n"\
                         "Creating default fall back element '%s'\n",
                         el, m_default_element->xmlGetName() );
#endif
        object = m_default_element->xmlCreate();
    }
    object->LinkSmartPtr( object );

    /* Even if the object creation fails, or is the wrong type, we hook in a node
       with a NULL object to keep the tree consistent. Null-ptr check needs to performed
       while iterating through the XML tree. */


#ifdef LOG_ME
    // just debug output
    for (int i = 0; i < m_depth; i++) {
        fprintf( stderr, "  ");
        logFile << "  ";
    }
    //
    fprintf( stderr, "<%s\n", el);
    logFile << "<" << el << "\n";
#endif
    // set the attributes of the object
    int p = 0;
    for ( ; object && attr[p]; p+=2 ) {
#ifdef LOG_ME
        for (int i = 0; i < m_depth; i++) {
            fprintf( stderr, "  ");
            logFile << "  ";
        }
        fprintf( stderr, "  %s='%s'\n", attr[p], attr[p+1]);
        logFile << "  " << attr[p] << " = \"" << attr[p+1] << "\"\n";
#endif
#ifdef _HAS_STD_STRINGS
        std::string attribute( attr[p] );
        std::string value    ( attr[p+1] );
        object->xmlSetAttribute( attribute, value );
#else
        object->xmlSetAttribute( attr[p], attr[p+1] );
#endif
    }
    // this is the XML tag name, not the name attribute!
    object->xmlSetAttribute( "xmlTagName", el ); // reserved attribute. can be ignored by the impl.
    // if this is parsed with a local buffer, the base path is NULL
    const XML_Char * base = XML_GetBase( m_parser );
    if ( base ) {
        object->xmlSetAttribute( "xmlBasePath", base );
    }

    // Call this after we have set all attributes. Allows us to read attributes on create
    object->xmlOnCreate( object );

    XmlNode *element = new XmlNode;
    element->setObject( object );
    element->setName( el );

    // if we got no root, make the new element the root
    if ( m_root == NULL ) {
         m_root = element;
    }
    // cached element. Is it NULL ? (this moves with the nesting level inside the XML)
    if ( m_element_node == NULL ) {
         m_element_node = m_root;
    }
    // check if the new one is the root, if not, add it to the prev one.
    if ( (void*)element != (void*)m_root ) {
        m_element_node->addChild( element, m_sort_children );
        m_element_node = element;
    }

#ifdef LOG_ME
    for (int i = 0; i < m_depth; i++) {
        fprintf( stderr, "  ");
        logFile << "  ";
    }
    fprintf( stderr, ">\n", el);
    logFile << ">\n" << std::flush;
#endif
    m_element_active = true;
    m_depth++;
}

void XmlParser::tagEnd(const XML_Char *el)
{
    // step up a little
    if ( m_element_node && m_element_node->getObject() ) {
        const XML_Char * base = XML_GetBase( m_parser );
        XmlInterfacePtr xmli = m_element_node->getObject();
        xmli->xmlInit( (const char*)base, m_element_node );
        xmli->xmlOnPostInit( (const char*)base, m_element_node );
    }

    if ( m_element_node && (void*)m_element_node != (void*)m_root )
    {
        // this never fails
        m_element_node = dynamic_cast<XmlNode*>(m_element_node->getParent());
    }
    --m_depth;
    m_element_active = false;

#ifdef LOG_ME
    int i;
    for (i = 0; i < m_depth; i++) {
        fprintf( stderr, "  ");
        logFile << "  ";
    }
    fprintf( stderr, "</%s>\n", el);
    logFile << "</" << el << ">\n" << std::flush;
#endif
}

void XmlParser::characterDataHandler( const XML_Char *s, int len )
{
    if ( m_element_active && len ) {
        std::string text( s, len );
        if ( m_element_node && m_element_node->getObject() ) {
            m_element_node->getObject()->xmlHandleTextSection( text.c_str() );
#ifdef LOG_ME
            if ( !(len == 1 && (s[0] == 9 || s[0] == '\n' || s[0] == '\r' )) )
            {
                for ( long i = 0; i < m_depth; i++) {
                    fprintf( stderr, "  ");
                    logFile << "  ";
                }
                fprintf( stderr, "%s\n", text.c_str() );
                logFile << text << "\n";
            }
#endif
        }
    }
}

int XmlParser::parseEntity(XML_Parser parser_in,
                           const char* context,
                           const char* base,
                           const char* system_id,
                           const char* publicId)
{
    if ( publicId ) {

        // not used!
        return XML_STATUS_OK;

    } else if ( system_id ) {

        std::string filename;
        std::string path = base;
        size_t ps = path.rfind( "\\" );
        if ( ps == std::string::npos ) {
            ps = path.rfind( "/" );
        }
        if ( ps != std::string::npos ) {
            filename = path.substr( 0, ps );
        }
        filename += "/";
        filename += system_id;
        if ( filename.size() > 0 ) {
            XML_Parser new_parser = XML_ExternalEntityParserCreate( parser_in, context, NULL );
            if ( new_parser ) {
                XML_UseForeignDTD( new_parser, XML_TRUE );
                bool nerr = parseFile( new_parser, filename.c_str() );
                XML_ParserFree( new_parser );
                return nerr ? XML_STATUS_OK : XML_STATUS_ERROR;
            }
        }
    }
    // always return OK. Skip entities
    return XML_STATUS_OK;
}

bool XmlParser::parseData( const char *data, size_t len, bool done )
{
    return parseData( m_parser, data, len, done );
}

bool XmlParser::parseData( XML_Parser parser, const char *data, size_t len, bool done )
{
    if (!parser) {
        fprintf( stderr, "No XML_Parser created! Init() not called?\n");
        return false;
    }
    if (!XML_Parse(parser, data, (int)len, done)) {
        fprintf( stderr, "ParseData error at line %d:\n%s\n",
                    (int)XML_GetCurrentLineNumber(parser),
                    XML_ErrorString(XML_GetErrorCode(parser)));
        return false;
    }
    return true;
}

bool XmlParser::parseFile( XML_Parser parser, const char *file_name )
{
    if (!parser) {
        fprintf( stderr, "No XML_Parser created! Init() not called?\n");
        return false;
    }
    FILE* fp = fopen(file_name, "r");
    if ( !fp )
    {
        fprintf( stderr, "Couldn't open file '%s'\n", file_name );
        return false;
    }

    if ( !XML_SetBase(parser, file_name) )
    {
        fclose(fp);
        return false;
    }

    char *buffer = new char[BUFFSIZE];
    bool done = false;
    while (!done) {
        size_t len = fread(buffer, 1, BUFFSIZE, fp);
        if (ferror(fp)) {
            fprintf( stderr, "Read error\n");
            fclose(fp);
            delete [] buffer;
            return false;
        }
        done = feof(fp) != 0;

        if ( parseData( parser, buffer, len, done ) == false ) {
            fprintf(stderr, "File '%s' parse error at line %d:\n%s\n",
                file_name,
                (int)XML_GetCurrentLineNumber(parser),
                XML_ErrorString(XML_GetErrorCode(parser)));
            fclose(fp);
            delete [] buffer;
            return false;
        }
    }
    fclose(fp);
    delete [] buffer;
    return true;
}

bool XmlParser::parseFile( const char* file_name )
{
    return parseFile( m_parser, file_name );
}

int XmlParser::getErrorLineNumber()
{
    return XML_GetCurrentLineNumber(m_parser);
}

const char* XmlParser::getErrorString()
{
    return XML_ErrorString(XML_GetErrorCode(m_parser));
}

void pei::XmlParser::skipEntity( void *userData, const XML_Char *entityName, int is_parameter_entity )
{
    XmlParser *self = static_cast<XmlParser*>(userData);

    int line = XML_GetCurrentLineNumber(self->m_parser);
    int column = XML_GetCurrentColumnNumber(self->m_parser);
    // set some error log: like Entity not found "[entityName]"
    char buffer[512];
    sprintf(buffer, "Entity not found '&%s;' parse error at line %d:%d",
        entityName, line, column );
}

int pei::XmlParser::unknownEncodingHandler( void *encodingHandlerData, const XML_Char *name, XML_Encoding *info )
{
    return 0;
}
