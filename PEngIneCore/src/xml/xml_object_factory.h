/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef XML_OBJECT_FACTORY_H_
#define XML_OBJECT_FACTORY_H_

#include <interfaces/xml_interface.h>
#include <xml/xml_element.h>

#include <string.h>

#include <map>
#include <string>
#include <assert.h>

#include <boost/shared_ptr.hpp>

namespace pei
{
    /*
     * Interface class to create an object
     * Used in the factory to setup a virtual CTOR list
     */
    class XmlObjectCreator
    {
    protected:
        XmlInterface::OnXmlCreateFunc m_OnCreateFunc;
        XmlInterface::OnXmlInitFunc   m_OnInitFunc;
    public:
        XmlObjectCreator( ) {}
        XmlObjectCreator( const XmlInterface::OnXmlInitFunc& func ) { m_OnInitFunc = func; }
        XmlObjectCreator( const XmlInterface::OnXmlCreateFunc& func ) { m_OnCreateFunc = func; }
        XmlObjectCreator( const XmlInterface::OnXmlCreateFunc& onCreateFunc, const XmlInterface::OnXmlInitFunc& onInitFunc ) {
            m_OnCreateFunc = onCreateFunc;
            m_OnInitFunc   = onInitFunc;
        }
        virtual ~XmlObjectCreator() {}

        virtual XmlInterfacePtr CreateObject() = 0;
    };
    typedef boost::shared_ptr<XmlObjectCreator> XmlObjectCreatorPtr;

    /*
     * Template class to create the object
     */
    template < class T >
    class XmlObjectCreatorT : public XmlObjectCreator
    {
    public:
        XmlObjectCreatorT( ) {}
        XmlObjectCreatorT( const XmlInterface::OnXmlInitFunc& func ) : XmlObjectCreator(func) { }
        XmlObjectCreatorT( const XmlInterface::OnXmlCreateFunc& func ) : XmlObjectCreator(func) { }
        XmlObjectCreatorT( const XmlInterface::OnXmlCreateFunc& onCreateFunc, const XmlInterface::OnXmlInitFunc& onInitFunc )
            :  XmlObjectCreator(onCreateFunc,onInitFunc) {}
        virtual ~XmlObjectCreatorT() {}

        virtual XmlInterfacePtr CreateObject() {
            XmlInterfacePtr ifp( new T() );
            if ( m_OnCreateFunc || m_OnInitFunc ) {
                ifp->xmlRegisterCallbacks( m_OnCreateFunc, m_OnInitFunc );
            }
            return ifp;
        }
    };

    /*
     * static factory.
     */
    class XmlObjectFactory;

    typedef boost::shared_ptr<XmlObjectFactory> XmlObjectFactoryPtr;

    class XmlObjectFactory
    {
    public:
        XmlObjectFactory() {};

        virtual ~XmlObjectFactory();

        virtual bool Init() { return true; }

        virtual void Register( const char* class_name, XmlObjectCreatorPtr creator );

        virtual void Clear();

        virtual pei::XmlInterfacePtr GetDefaultObject();

        virtual XmlInterfacePtr CreateObject( const char* class_name );

        static XmlObjectFactoryPtr GetFactory() {
            assert( m_self );
            return m_self;
        }
        static XmlObjectFactoryPtr CreateFactory();

    protected:

        static XmlObjectFactoryPtr m_self;

        std::map<std::string, XmlObjectCreatorPtr> m_object_creator_map;
    };

    #define XML_OBJECT_FACTORY() XmlObjectFactory::GetFactory()

    //////////////////////////////////////////////////////////////////////////
    // Add this macro below to your object if you want to make it script able, e.g.
    //
    // my_xml_object.h:
    //
    // #include "xml_node.h"
    //
    // class MyXmlObject : public XmlNode
    // {
    // };
    //
    // ...
    //
    // my_xml_object.cpp:
    //
    // #include "xml_object_factory.h"
    //
    // XML_DECLARE_OBJECT( MyXmlObject );
    //
    // MyXmlObject::MyXmlObject()
    // {
    // }
    //
    // This will create a template object creator which calls your MyXmlObject::MyXmlObject()
    // Make sure the class has an default constructor!
    //////////////////////////////////////////////////////////////////////////

    #define XML_DECLARE_OBJECT( CLASS, KEY ) \
        struct Create##CLASS                                                \
        {                                                                   \
            const char *key;                                                \
            Create##CLASS () : key( #KEY )                                \
            {                                                               \
                pei::XmlObjectFactory::CreateFactory();                                 \
                pei::XmlObjectFactoryPtr f = pei::XmlObjectFactory::GetFactory();       \
                if ( f ) f->Register( key, pei::XmlObjectCreatorPtr(new pei::XmlObjectCreatorT<CLASS>()) );\
            }                                                               \
        };                                                                  \
        static Create##CLASS __creator##CLASS;                              \


    #define XML_DECLARE_OBJECT2( FACTORY, CLASS, KEY ) \
        FACTORY->Register( #KEY, pei::XmlObjectCreatorPtr(new pei::XmlObjectCreatorT<CLASS>() ));

}; //namespace pei

#endif /*XML_OBJECT_FACTORY_H_*/
