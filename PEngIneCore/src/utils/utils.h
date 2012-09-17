/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

namespace pei
{
    std::string trimWhiteSpaces( const char* c, int len );

    std::string trimWhiteSpaces( const char* c );

    std::vector<std::string> breakLine( const char* text, size_t max_len = ~0UL, bool no_trim_ws = false );

    std::vector<std::string> split( const char* text, const char* pat, bool no_trim_ws = false );

};


#endif // __UTILS_H__
