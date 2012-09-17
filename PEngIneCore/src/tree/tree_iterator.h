/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef __TREE_ITERATOR_H__
#define __TREE_ITERATOR_H__

#include <interfaces/node_interface.h>

namespace pei {
/**  

ex1: iterator

TreeIterator<Type> it(obj);
for ( !it.isNull(); it++ );
{
    (*it)->DoWhatEver();
}


*/

	template< class T >
	class TreeIterator
	{
	protected:

	public:
		TreeIterator( const TreeIterator& other )
			: m_current( other.m_current )
			, m_parent( other.m_parent )
			, m_nested_level( other.m_nested_level )
		{ }

		TreeIterator( NodeInterface* node )
			: m_current( node )
			, m_parent( NULL )
			, m_nested_level( 0 )
		{
			if ( node ) {
				m_parent = node->getParent();
			}
		}

		virtual ~TreeIterator() {}

		bool isNull()
		{
			return (m_current == NULL);
		}

		virtual T operator*()
		{ return dynamic_cast<T>(m_current); }
		virtual T operator->()
		{ return dynamic_cast<T>(m_current); }

		// post-increment
		TreeIterator operator++(int)
		{
			TreeIterator current( *this );
			m_current = iterate_tree( m_current, m_parent );
			return current;
		}
		// pre-increment
		TreeIterator operator++()
		{
			m_current = iterate_tree( m_current, m_parent );
			return *this;
		}

		static inline TreeIterator getHead( NodeInterface *root )
		{
			while ( root && root->getParent() ) root = root->getParent();
			return TreeIterator( root );
		}

		int getNestingLevel() { return m_nested_level; }

	protected:

		virtual NodeInterface *iterate_tree( NodeInterface* current, NodeInterface* &parent )
		{
			if ( current == NULL ) {
				return NULL;
			}
			size_t max = current->getNumberOfChildren();
			size_t child_index = 0;
			while ( child_index >= max )
			{
				m_nested_level--;
				if ( parent  == NULL ||
					 current == parent )
				{
					return NULL;
				}
				child_index = (size_t)parent->getChildIndex( current ) + 1;
				current = parent;
				parent = parent->getParent();
				max = current->getNumberOfChildren();
			}
			m_nested_level++;
			parent = current;
			return current->getChild( (int)child_index );
		}

		NodeInterface *m_current, *m_parent;
		int            m_nested_level;

	};

} // namespace pei
#endif // __TREE_ITERATOR_H__
