/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef TIFF_IMAGE_DECODER_H
#define TIFF_IMAGE_DECODER_H

#include "dec_video_image_sequence.h"

#include <string>
#include <memory.h>

#include <boost/weak_ptr.hpp>
#include <boost/smart_ptr.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs  = boost::filesystem;

#include <boost/iostreams/device/mapped_file.hpp>
namespace bios = boost::iostreams;

namespace pei
{
	class TiffImageAsset;
	typedef boost::shared_ptr<TiffImageAsset> TiffImageAssetPtr;

	class TiffPrivateContext;

	class TiffImageAsset : public ImageSequenceAsset
    {
    protected:
    	boost::weak_ptr<TiffImageAsset> m_WeakThis;

        TiffPrivateContext *m_Context;
        float               m_DpiX,
                            m_DpiY;
    private:
        TiffImageAsset( const TiffImageAsset& other );

    protected:
        TiffImageAsset( const char* name );

        pei::VideoAssetPtr LinkSmartPtr( boost::shared_ptr<pei::TiffImageAsset> self ) {
        	m_WeakThis = self;
        	return self;
        }
    public:
        static pei::VideoAssetPtr Create( const char* name ) {
            TiffImageAsset *pngAsset(new TiffImageAsset( name ));
            return pngAsset->LinkSmartPtr( boost::shared_ptr<pei::TiffImageAsset>(pngAsset) );
        }

        virtual ~TiffImageAsset();

        virtual bool CanHandle();

        virtual bool Open();

        virtual void Close( );

        virtual std::string GetFormatString() const;

        /** get the number of frames in the video asset. Single frame returns 1 */
        virtual int GetNumberOfFrames() const;

        inline void GetDpi( unsigned int& dpix, unsigned int& dpiy ) {
            dpix = (unsigned int)m_DpiX; dpiy = (unsigned int)m_DpiY;
        }

    protected:
        /** not used */
        virtual pei::SurfacePtr ReadFrame( const char* file_name ) { return pei::SurfacePtr(); }

        virtual pei::SurfacePtr DecodeFrame( int i );

        // This is a (non virtual) way to retrieve the tiff handle for derived decoders without exposing the libTIFF to the header
        void* GetTiffHandle() const;
    };

}; // namespace pei;

#endif // TIFF_IMAGE_DECODER_H
