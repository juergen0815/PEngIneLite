/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef ENCODER_H
#define ENCODER_H

#include <boost/shared_ptr.hpp>

namespace pei
{
    class Asset;
    typedef boost::shared_ptr<Asset> AssetPtr;

    class Encoder
    {
    public:
        Encoder( const AssetPtr asset ) {};

        virtual ~Encoder() {}

        virtual bool Open( const char* file ) = 0;

        virtual void Close() = 0;

        virtual int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 ) = 0;

        virtual const char* GetFileExtension() const = 0;

        /** for some encoders we can set the string format for single frames */
        virtual void SetFormatString( const char* fmt ) {}
    };

};

#endif // EXPORTER_H
