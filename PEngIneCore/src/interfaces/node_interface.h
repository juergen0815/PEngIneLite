/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef NODE_INTERFACE_H
#define NODE_INTERFACE_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace pei
{
    class NodeInterface
    {
    public:
    	typedef const boost::function<bool(NodeInterface*)> CompareFunc;

    	virtual ~NodeInterface() {}

        virtual bool compareName( const char* name ) = 0;
        /*! 
        */
        virtual const char* getName() const = 0;

        /*! 
        */
        virtual void setName( const char* name ) = 0;

        /*!
         */
        virtual void sortChildren( ) = 0;

        /*! 
        */
        virtual NodeInterface* addChild( NodeInterface *child, bool sort = true ) = 0;

        /*! 
        */
        virtual bool removeChild( NodeInterface* child ) = 0;

        /*! 
        */
        virtual bool removeChild( const char* name, bool all = true ) = 0;

        /*! 
        */
        virtual NodeInterface* getChild( int num ) = 0;

        /*! 
        */
        virtual const NodeInterface* getChild( int num ) const = 0;

        /*! 
        */
        virtual NodeInterface* findChild( const char* what, const NodeInterface::CompareFunc& comp ) const = 0;


        /*! = (const NodeInterface::CompareFunc&)boost::bind( &NodeInterface::compareName, _1, name )
        */
        virtual NodeInterface* findNextChild( const char* what, const NodeInterface::CompareFunc& comp ) const = 0;

        /*!
        */
        virtual NodeInterface* getParent() = 0;

        /*! 
        */
        virtual size_t getNumberOfChildren() const = 0;

        /*! 
        */
        virtual int getChildIndex( NodeInterface *child ) const = 0;

        /*!
         */
        virtual void setParent( NodeInterface *node )  = 0;
    };
    typedef boost::shared_ptr<NodeInterface> NodeInterfacePtr;
}; // namespace pei
 
#endif // NODE_INTERFACE_H
