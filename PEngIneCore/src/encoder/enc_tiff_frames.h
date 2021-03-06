/************************************************************************/
/* pei::EngineLite (�)Copyright 2009-12 J�rgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef TIFF_ENCODER_H
#define TIFF_ENCODER_H

#include "encoder.h"

namespace pei
{

    class TiffEncoder : public Encoder
    {
    private:
        class TiffEncPrivate *priv;

    public:
        TiffEncoder( const AssetPtr asset );

        virtual ~TiffEncoder();

        virtual bool Open( const char* file );

        virtual void Close();

        virtual int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 );

        virtual const char* GetFileExtension() const;

        void SetFormatString( const char* fmt );
    };

};

#endif // TIFF_ENCODER_H
