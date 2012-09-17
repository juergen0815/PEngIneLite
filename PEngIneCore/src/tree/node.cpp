/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "node.h"
#include "tree_iterator.h"

#include <algorithm>

#include <string.h>

using namespace pei;

Comparator::Comparator( const char *n ) : name( n ) {}

Comparator::~Comparator() {}

const char* Comparator::getName() const { return name; }

bool Comparator::compare( NodeInterface* const a )
{
    return strcmp( a->getName(), getName() ) == 0;
}

bool Comparator::operator( ) ( NodeInterface* const a )
{
    return compare( a );
}


