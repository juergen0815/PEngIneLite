/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

/*
 * xml_interface.h
 *
 * @short
 *
 * Every object created through the xml scripting unit needs to implement
 * a minimalistic XmlInterface. This is used to allow the xml object factory
 * to xmlCreate an object and set attributes of the objects when creating the object.
 * The object does not need to implment any node interface of any kind.
 *
 * The factory creates an internal object tree to store the XML tree. This
 * tree is not supposed to be used in later stages (e.g. render or event loop),
 * but is only a temporary layout tree. Who ever needs the structured data,
 * must use their own container to process the objects (e.g. render (bsp) tree,
 * object event list, etc).
 */

#ifndef XML_INTERFACE_H
#define XML_INTERFACE_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>

namespace pei
{
    class XmlNode;

    class XmlInterface;
    typedef boost::shared_ptr<XmlInterface> XmlInterfacePtr;

    class XmlInterface
    {
    public:
        typedef boost::function<void( XmlInterfacePtr& ifp )> OnXmlCreateFunc;
		typedef boost::function<bool( const char* base, XmlNode *node )> OnXmlInitFunc;
    protected:
		OnXmlCreateFunc m_OnCreateFunc;
		OnXmlInitFunc   m_OnInitFunc;

		/*!
		 *
		 */
        virtual ~XmlInterface() {};
    public:
        /*!
         * Make self a smart pointer
         * @param ptr smartptr returned from Create()
         * @return smart pointer to self
         */
        virtual XmlInterfacePtr LinkSmartPtr( XmlInterfacePtr ptr ) = 0;

        /*!
         *
         * @param ifp
         */
        virtual void xmlOnCreate( XmlInterfacePtr ifp ) {
            if ( m_OnCreateFunc ) {
                m_OnCreateFunc( ifp );
            }
        }

        /*!
         *  Feed in attributes from the script to be processed by the object
         *  Note: this is one exception to pass strings as objects, rather then chars.
         *  The reason is, attributes and values are pre-processed. The strings are
         *  always lower case and allow a simple '==' operator to be used. No additional
         *  handling in the class is required. That said, attributes and values are
         *  not case sensitive (but need to be compared against lower case, like a boolean
         *  can be "true", but "TRUE" will fail (in your class, not in the script!).
         *
         * @param attribute the attribute to set. straight from XML, no case conversion, etc.
         * @param value The value to set the attribute to
         * @return not used
         */
        virtual bool xmlSetAttribute( const char* attribute, const char* value ) = 0;

        /*!
         *
         * @param name
         * @return
         */
        virtual const char* xmlGetAttribute( const char * name ) const = 0;

        /*!
         *
         * @param num
         * @return
         */
        virtual std::string xmlGetText( size_t num = 0 ) const = 0;

        /*!
         *  Creates an empty clone of self - needed for default element
         * @return
         */
        virtual XmlInterfacePtr xmlCreate() const = 0;

        /*!
         *
         * @return
         */
        virtual const char* xmlGetName() const = 0;

        /*!
         *
         * @param text
         */
        virtual void xmlHandleTextSection( const char* text ) = 0;

        /*!
         * @brief Init node. Called after all children have been created and initialized
         * @param base basepath of the XML file
         * @param node link to self (as an XmlNode)
         * @return
         */
        virtual bool xmlInit( const char* base, XmlNode *node = NULL ) = 0;

    public:
        /*!
         * called by parser after xmlInit() to allow app spec init (e.g. custom param, rendermanager, event manager, register events, etc)
         * @param base
         * @param node
         * @return
         */
        virtual bool xmlOnPostInit( const char* base, XmlNode *node )
        {
        	if ( m_OnInitFunc ) {
        		return m_OnInitFunc( base, node );
        	}
        	return true;
        }

        /*!
         *
         * @param onInit
         * @param onCreate
         */
        virtual void xmlRegisterCallbacks( const OnXmlCreateFunc& onCreate, const OnXmlInitFunc& onInit )
        {
            if ( onCreate ) m_OnCreateFunc = onCreate;
        	if ( onInit )   m_OnInitFunc   = onInit;
        }
    };


}; //namepsace pei

#endif // XML_INTERFACE_H
