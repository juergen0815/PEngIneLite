/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef __TREE_CHILD_ITERATOR_H__
#define __TREE_CHILD_ITERATOR_H__

#include "interfaces/node_interface.h"

/**  

ex1:

// take an (child) object and iterate thru all children
TreeChildIterator<Type*> it( node );
current = this;
while ( !it.isNull() )
{
    Type* t = *it++;
}


ex2: iterator

XmlNode::iterator it(obj);
for ( !it.IsNull(); it++ );
{
    (*it)->DoWhatEver();
}

*/

template< class T >
class TreeChildIterator
{
protected:

public:
    TreeChildIterator( const TreeChildIterator& other )
        : m_current( other.m_current )
        , m_parent( other.m_parent )
    { }

    TreeChildIterator( NodeInterface* root )
        : m_current( NULL )
        , m_parent( root )
    { 
    }

    TreeChildIterator begin()
    {
        if ( m_parent ) {
            m_current = m_parent->getChild( 0 );
        }
        return this;
    }

    bool IsNull()
    {
        return ( m_current == NULL );
    }

    virtual T operator*()  
    { return dynamic_cast<T>(m_current); }
    virtual T operator->() 
    { return dynamic_cast<T>(m_current); }

    // post-increment
    TreeChildIterator operator++(int)
    {
        TreeChildIterator current( *this );
        m_current = iterate_tree( m_current, m_parent );
        return current;
    }
    // pre-increment
    TreeChildIterator operator++()
    {
        m_current = iterate_tree( m_current, m_parent );
        return *this;
    }

    static inline TreeChildIterator Head( NodeInterface *root )
    {
        while ( root && root->getParent() ) root = root->getParent();
        return TreeChildIterator( root );
    }

    int getNestingLevel() { return m_nested_level; }

protected:

    NodeInterface *iterate_tree( NodeInterface* current, NodeInterface* &parent )
    {
        return NULL;
    }

    NodeInterface *m_current, *m_parent;
    int            m_current_index;
};

#endif // __TREE_CHILD_ITERATOR_H__
