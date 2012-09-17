/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef VISUAL_DECODER_H
#define VISUAL_DECODER_H

#include <interfaces/asset_interface.h>
#include <render/render_surface.h>

#include <boost/shared_ptr.hpp>

namespace pei
{
	class VideoAsset;
	typedef boost::shared_ptr<VideoAsset> VideoAssetPtr;

    class VideoAsset : public Asset
    {
    protected:
        VideoAsset( const char* name ) : Asset( name ) {}

    public:
        /** get the number of frames in the video asset. Single frame returns 1 */
        virtual int GetNumberOfFrames() const = 0;

        /** animation/movie time in ms */
        virtual double GetLenght() const = 0;

        /** read current frames per second */
        virtual double GetFps() const = 0;

        /** override fps for decoder */
        virtual void SetFps( double fps ) = 0;

        /** get a format description for the given frame. 0 if single image */
        virtual const Format& GetFormat( ) const = 0;

        /** decode a frame to the given format. A decoder can usually 
        mirror data very quickly. That's why these parameters are built
        into the interface. Rotation and scaling is not supported, though,
        nor is filtering of any kind. I might add those, though. */
        virtual pei::SurfacePtr Decode( pei::Format& dest_format, double time = 0 ) = 0;

        /** get the stored resource name */
        virtual const char* GetResourceName() const = 0;

        /** get the stored resource directory */
        virtual const char* GetResourceDirectory() const {
            return GetResourceName();
        }

        virtual void SetCanvasSize( int w, int h ) {}

        virtual void SetPositionOffset( int x, int y ) {}
    };

}; // namespace pei

#endif // VISUAL_DECODER_H
