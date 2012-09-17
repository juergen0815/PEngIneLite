/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <interfaces/node_interface.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>
#include <functional>

#include <boost/functional/hash.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <time.h>

namespace pei {

/*
 * This is a default interface class to implement sorting.
 * On default, the pointers will be compared (note! Must not always return true or false!
 * but the result must be unique and valid!)
 * Implement your own sorter interface and feed it into the template as a second
 * parameter. You do not derive from "DefaultInterface", this is just a fall back, 
 * in face your MyInterface::lesserThen() method must differ in the sense
 * it uses "const MyInterface* other" as a parameter. 
 */
class DefaultNodeComparator
{
public:
	virtual ~DefaultNodeComparator() {}
    virtual bool lesserThen( const DefaultNodeComparator* other ) const
    {
        return this < other; // compares pointers!!!
    }
};

class Comparator
{
    const char* name;
public:
    Comparator( const char *n );
    virtual ~Comparator();

    virtual const char* getName() const;

    virtual bool compare( NodeInterface* const a );
    
    bool operator( ) ( NodeInterface* const a );
};

template< class T, class I = DefaultNodeComparator >
class Node : public NodeInterface
           , public I
{
public:

    typedef Node<T,I> NodeT;
	typedef boost::shared_ptr<NodeT> NodeTPtr;
    typedef std::deque< NodeT* > ChildNodes;

    Node( const Node& other );

    Node( const char* name = "Node" )
        : m_name( name )
        , m_parent(NULL)
    {
    }
    virtual ~Node();

    virtual bool compareName( const char* name ) {
    	return strcmp( name, getName() ) == 0;
    }

    //////////////////////////////////////////////////////////////////////////
    // Node interface 

    virtual const char* getName() const { return m_name.c_str(); }

    virtual void setName( const char* name ) { m_name = name; }

    /*!
    */
    virtual void sortChildren( );

    virtual NodeInterface* addChild( NodeInterface *child, bool sort = true  );
    virtual bool removeChild( NodeInterface* child );
    virtual bool removeChild( const char* name, bool all = true );

    virtual NodeInterface* getChild( int num );
    virtual const NodeInterface* getChild( int num ) const;
    virtual NodeInterface* findChild( const char* what, const NodeInterface::CompareFunc& comp ) const;
    virtual NodeInterface* findNextChild( const char* what, const NodeInterface::CompareFunc& comp ) const;

    virtual NodeInterface* findChild( const char* name ) const {
        return findChild( name, (const NodeInterface::CompareFunc&)boost::bind( &NodeInterface::compareName, _1, name ) );
    }
    virtual NodeInterface* findNextChild( const char* name ) const {
        return findNextChild( name, (const NodeInterface::CompareFunc&)boost::bind( &NodeInterface::compareName, _1, name ) );
    }

    virtual NodeInterface* getParent();
    virtual void setParent( NodeInterface* parent );

    virtual size_t getNumberOfChildren() const { return m_children.size(); }

    virtual int getChildIndex( NodeInterface *child ) const 
    {
        int i = 0;
        for ( ; i < (int)getNumberOfChildren(); i++ )
        {
            if ( child == getChild(i) ) {
                return i;
            }
        }
        return -1;
    }

    static bool Sort( const Node<T,I>* a, const Node<T,I>* b );

protected:
    std::string              m_name;
    NodeInterface           *m_parent;
    ChildNodes  m_children;
    std::map< size_t, typename ChildNodes::const_iterator > m_lastSearch;

};

template<class T, class I>
Node<T,I>::~Node()
{
    int i = 0;
    typename ChildNodes::iterator child = m_children.begin();
    while ( child < m_children.end() ) {
        Node<T,I> *c = *child++;
        if ( c ) {
            delete c;
        }
        i++;
    }
    m_children.clear();
}

template<class T, class I>
bool Node<T,I>::Sort( const Node<T,I>* a, const Node<T,I>* b )
{
    return a->lesserThen( b );
}

template<class T, class I>
void Node<T,I>::sortChildren( ) 
{
    std::sort( m_children.begin(), m_children.end(), Node<T,I>::Sort );
}

template<class T, class I>
NodeInterface* Node<T,I>::addChild( NodeInterface *child, bool sort /*= true*/  )
{
    m_children.push_back( (Node<T,I>*)child );
    child->setParent( this );

    if ( sort ) {
        sortChildren();
    }
    return child;
}

template<class T, class I>
bool Node<T,I>::removeChild( NodeInterface* child )
{
    if ( child && child->getParent() == this ) {
        child->setParent( NULL );

        /* this is actually wrong. remove removes all "child" children.
           If a node is attached multiple times, the array will only shrink
           by one, but all children with the same address will be removed! */
        typename ChildNodes::iterator _first = m_children.begin();
        typename ChildNodes::iterator _end   = m_children.end();
        _end = std::remove( _first, _end, child );
        // remove does not resize the array!
        size_t s = _end - _first;
        m_children.resize( s ); 
        // resort children
        sortChildren();

        return true;
    } 
    return false; 
}

template<class T, class I>
bool Node<T,I>::removeChild( const char* name, bool all /*= true*/ )
{
    NodeInterface *node = findChild( name );
    if ( node ) {
        removeChild( node );
    }
    return true;
}

template<class T, class I>
NodeInterface* Node<T,I>::getChild( int num )
{
    return (size_t)num >= m_children.size() ? NULL : m_children[num];   
}

template<class T, class I>
const NodeInterface* Node<T,I>::getChild( int num ) const
{
    return (size_t)num >= m_children.size() ? NULL : m_children[num];   
}

template<class T, class I>
NodeInterface* Node<T,I>::findChild( const char* what, const NodeInterface::CompareFunc& comp ) const
{
    size_t key = boost::hash<const char*>()(what);
    const_cast< Node<T,I> *>( this )->m_lastSearch[ key ] = m_children.begin();
    return findNextChild( what, comp );
}

template<class T, class I>
NodeInterface* Node<T,I>::findNextChild( const char* what, const NodeInterface::CompareFunc& comp ) const
{
    size_t key = boost::hash<const char*>()(what);
    typename ChildNodes::const_iterator from = const_cast< Node<T,I> *>( this )->m_lastSearch[key];
    typename ChildNodes::const_iterator cit = std::find_if( from, m_children.end(), comp );
    if ( cit != m_children.end() ) {
        NodeInterface* node = *cit++;
        const_cast< Node<T,I> *>( this )->m_lastSearch[key] = cit;
        return node;
    }
    const_cast< Node<T,I> *>( this )->m_lastSearch[boost::hash<const char*>()(what)] = m_children.end();
    return NULL;
}
// 
// template<class T, class I>
// NodeInterface* Node<T,I>::findChild( const char* name, const Comparator& comp ) const
// {
// 
// }
// 
// template<class T, class I>
// NodeInterface* Node<T,I>::findNextChild( const char* name, const Comparator& comp ) const
// {
// 
// }

template<class T, class I>
NodeInterface* Node<T,I>::getParent()
{
    return m_parent;
}

template<class T, class I>
void Node<T,I>::setParent( NodeInterface* parent )
{
    m_parent = parent;
}

}

#endif /*NODE_H*/
