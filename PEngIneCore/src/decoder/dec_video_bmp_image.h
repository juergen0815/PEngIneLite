/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef BMP_IMAGE_DECODER_H
#define BMP_IMAGE_DECODER_H

#include "dec_video.h"

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
	class BmpImageAsset;
	typedef boost::shared_ptr<BmpImageAsset> BmpImageAssetPtr;

	class BmpPrivateContext;

	class BmpImageAsset : public VideoAsset
    {
    protected:
    	boost::weak_ptr<BmpImageAsset> m_WeakThis;

        std::string 	m_DirectoryName;
        std::string 	m_AssetName;
        std::string     m_FileExtension;
        bool        	m_IsOpen;
        pei::SurfacePtr m_Surface;	// we keep one pixel descriptor

        BmpPrivateContext *m_Context;
    private:
        BmpImageAsset( const BmpImageAsset& other );

    protected:
        BmpImageAsset( const char* name );

        pei::VideoAssetPtr LinkSmartPtr( boost::shared_ptr<pei::BmpImageAsset> self ) {
        	m_WeakThis = self;
        	return self;
        }
    public:
        static pei::VideoAssetPtr Create( const char* name ) {
            BmpImageAsset *pngAsset(new BmpImageAsset( name ));
            return pngAsset->LinkSmartPtr( boost::shared_ptr<pei::BmpImageAsset>(pngAsset) );
        }

        virtual ~BmpImageAsset();

        //////////////////////////////////////////////////////////////////////////
        // Asset Interface

        virtual bool CanHandle();

        virtual bool Open(  );

        virtual void Close( );

        virtual int GetMemoryUsage();

        /** abstract format description */
        virtual std::string GetFormatString() const;

        virtual const char* GetResourceName() const {
            return m_AssetName.c_str();
        }

        virtual const char* GetResourceDirectory() const {
            return m_DirectoryName.c_str();
        }

        //////////////////////////////////////////////////////////////////////////
        // VisualAsset interface

        /** get the number of frames in the video asset. Single frame returns 1 */
        virtual int GetNumberOfFrames() const;

        /** animation/movie time in ms */
        virtual double GetLenght() const;

        /** read current frames per second */
        virtual double GetFps() const;

        /** override fps for decoder */
        virtual void SetFps( double _fps );

        /** get a format description for the given frame. 0 if single image */
        virtual const pei::Format& GetFormat( ) const;

        /** decode a frame to the given format */
        virtual pei::SurfacePtr Decode( pei::Format& dest_format, double time = 0);

    };

}; // namespace pei;

#endif // TIFF_IMAGE_DECODER_H
