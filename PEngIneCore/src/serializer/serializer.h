/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <iostream>

namespace pei {

	class Serializer
	{
	public:
		Serializer();
		virtual ~Serializer();

        virtual std::ostream& operator<<( std::ostream& stream ) const = 0;

        virtual std::istream& operator>>( std::istream& stream ) = 0;

	};

    std::ostream& operator<<( std::ostream& stream, const pei::Serializer& ser )
    {
        return ser.operator <<( stream );
    }

    std::istream& operator>>( std::istream& stream, pei::Serializer& ser )
    {
        return ser.operator >>( stream );
    }

} /* namespace pei */
#endif /* SERIALIZER_H_ */
