/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef AUDIO_STREAM_INTERFACE_H
#define AUDIO_STREAM_INTERFACE_H

#include <boost/shared_ptr.hpp>

namespace pei
{
    class AudioAsset;
    typedef boost::shared_ptr<AudioAsset> AudioAssetPtr;

    class AudioStreamInterface;
    typedef boost::shared_ptr<AudioStreamInterface> AudioStreamInterfacePtr;

    class AudioStreamInterface
    {
    protected:
        bool kill;

        AudioStreamInterface() : kill(false) {}
    public:
        virtual void Kill() { kill = true; }
        virtual bool MustKill() { return kill; }

        virtual void LinkSmartPtr( AudioStreamInterfacePtr self ) = 0;

        virtual AudioStreamInterfacePtr Get() = 0;

        AudioStreamInterface( AudioStreamInterface& o ) : kill(false) {}

        AudioStreamInterface( AudioAssetPtr asset ) : kill(false) {}

        virtual ~AudioStreamInterface() {}

        virtual AudioAssetPtr GetAsset() = 0;

        virtual const AudioAssetPtr& GetAsset() const = 0;

        virtual bool Open() = 0;

        virtual void Close() = 0;

        virtual bool IsOpen() const = 0;

        virtual AudioStreamInterfacePtr Clone() const = 0;

        virtual int  FetchData( char* buffer, int len, int offset ) = 0;

        virtual AudioStreamInterfacePtr Rewind( ) = 0;

        virtual AudioStreamInterfacePtr Seek( int pos ) = 0;

        virtual bool IsEof( ) const = 0;
    };

}; // namespace pei

#endif // AV_INTERFACE_H
