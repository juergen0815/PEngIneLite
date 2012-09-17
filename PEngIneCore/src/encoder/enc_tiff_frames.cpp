/************************************************************************/
/* pei::Engine (©)Copyright 2010 Jürgen Schober                         */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "enc_tiff_frames.h"

#include <decoder/dec_video.h>
#include <render/color.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <tiffio.h>

#ifdef WIN32
#include <direct.h>
#endif
#include <string.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;
namespace pei
{
	class TiffEncPrivate : public Encoder
	{
	    friend class TiffEncoder;
	protected:
	    /** we do not own this asset!! */
	    VideoAssetPtr     m_VideoAsset;

	    std::string       m_directory;
	    std::string       m_format_string;
	protected:
	    TiffEncPrivate( const AssetPtr asset )
	        : Encoder( asset )
	        , m_format_string("frame_%04d")
	    {
            m_VideoAsset = boost::dynamic_pointer_cast<VideoAsset>(asset);
	    }

	    virtual ~TiffEncPrivate()
	    {
	    }

	    bool Open( const char* directory )
	    {
	        m_directory = directory;
	        if ( m_VideoAsset ) {
	            // always make dir. Will fail if exists, but we don't care.
	            try {
	                bfs::create_directory( directory );
	            }
	            catch ( std::exception &ex )
	            {

	            }
	            catch ( ... )
	            {

	            }
	        }
	        return true;
	    }

	    void Close()
	    {
	    }

	    virtual const char* GetFileExtension() const { return "tiff"; }

	    void SetFormatString( const char* fmt )
	    {
	        m_format_string = fmt;
	    }

	    int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 )
	    {
	        int frames_written(0);
	        if ( !m_VideoAsset) {
	            return frames_written;
	        }


	        int frame = frame_from;
	        if ( frame_to == -1 || frame_to >= m_VideoAsset->GetNumberOfFrames() ) {
	            frame_to = m_VideoAsset->GetNumberOfFrames() - 1;
	        }
	        double frame_time = m_VideoAsset->GetFormat().GetFrameTime();
	        while ( frame <= frame_to )
	        {
                pei::Format tFmt;
                pei::SurfacePtr surface = m_VideoAsset->Decode( tFmt, (double)frame*frame_time );
                if ( !surface )
                {
                    break;
                }
                char file[ 256 ];
                std::string fms = m_directory + "/" + m_format_string + "." + GetFileExtension();
                sprintf( file, fms.c_str(), offset + frame++ );
                TIFF *image = TIFFOpen( file, "w");
                if (image == NULL){
                    std::cerr << "Could not '" << file << "' for writing\n";
                    break;
                }
                surface->Lock();
                int spp = surface->GetBytesPerPixel();
                int bps = surface->GetDepth() / spp;
                 // We need to set some values for basic tags before we can add any data
                TIFFSetField(image, TIFFTAG_IMAGEWIDTH, surface->GetWidth());
                TIFFSetField(image, TIFFTAG_IMAGELENGTH, surface->GetHeight());
                TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, spp);
                TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, bps);

                TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT );
                TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
                TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

                // TODO: Give encoder options to set compression ratio, etc.
                TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
                TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);

                TIFFSetField(image, TIFFTAG_XRESOLUTION, 150.0);
                TIFFSetField(image, TIFFTAG_YRESOLUTION, 150.0);
                TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

                //TIFFScanlineSize( image, surface->GetPitch( ) );
                TIFFSetField( image, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(image, surface->GetPitch()));
                for ( int y = 0; y < (int)surface->GetHeight(); y++ ) {
                    void *buf = (void*)((char*)surface->GetPixels() + surface->GetPitch()*y);
                    if ( TIFFWriteScanline( image, buf, y, 0 ) < 0) break;
                }

                // Close the file
                TIFFClose(image);
                surface->Unlock();
                frames_written++;
	        };
	        return frames_written;
	    }
	};

	TiffEncoder::TiffEncoder( const AssetPtr asset )
	    : Encoder( asset )
	    , priv( new TiffEncPrivate( asset ) )
	{

	}

	TiffEncoder::~TiffEncoder()
	{
	    if ( priv ) {
	        delete priv;
	    }
	}

	bool TiffEncoder::Open( const char* file )
	{
	    return priv->Open( file );
	}

	void TiffEncoder::Close()
	{
	    priv->Close();
	}

	int TiffEncoder::WriteBuffers( int frame_from /*= 0*/, int frame_to /*= -1*/, int offset /*= 0*/ )
	{
	    return priv->WriteBuffers( frame_from, frame_to, offset );
	}

	void TiffEncoder::SetFormatString( const char* fmt )
	{
	    priv->SetFormatString( fmt );
	}

	const char* TiffEncoder::GetFileExtension() const
	{
	    return priv->GetFileExtension();
	}

}; // namespace pei
