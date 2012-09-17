/************************************************************************/
/* pei::Engine (©)Copyright 2010 Jürgen Schober                         */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef BMP_ENCODER_H
#define BMP_ENCODER_H

#include "encoder.h"

namespace pei
{

    class BmpEncoder : public Encoder
    {
    private:
        class BmpEncPrivate *priv;

    public:
        BmpEncoder( const AssetPtr asset );

        virtual ~BmpEncoder();

        virtual bool Open( const char* file );

        virtual void Close();

        virtual int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 );

        virtual const char* GetFileExtension() const;

        void SetFormatString( const char* fmt );
    };

};

#endif // BMP_ENCODER_H
