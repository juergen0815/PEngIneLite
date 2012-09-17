/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef PNG_ENCODER_H
#define PNG_ENCODER_H

#include "encoder.h"

namespace pei
{

    class PngEncoder : public Encoder
    {
    private:
        class PngEncPrivate *priv;

    public:
        PngEncoder( const AssetPtr asset );

        virtual ~PngEncoder();

        virtual bool Open( const char* file );

        virtual void Close();

        virtual int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 );

        virtual const char* GetFileExtension() const;

        void SetFormatString( const char* fmt );

    };

};

#endif // PNG_ENCODER_H
