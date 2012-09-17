/************************************************************************/
/* pei::Engine (©)Copyright 2010 Jürgen Schober                         */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "enc_bmp_frames.h"

#include <decoder/dec_video.h> // THIS NEEDS TP GO!!
#include <render/color.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <stdio.h>
#ifdef WIN32
#include <direct.h>
#endif
#include <string.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;
namespace pei
{
	class BmpEncPrivate : public Encoder
	{
	    friend class BmpEncoder;
	protected:
	    /** we do not own this asset!! */
	    VideoAssetPtr m_VideoAsset;

	    std::string       m_directory;
	    std::string       m_format_string;
	protected:
	    BmpEncPrivate( const AssetPtr asset )
	        : Encoder( asset )
	        , m_format_string("frame_%04d")
	    {
            m_VideoAsset = boost::dynamic_pointer_cast<VideoAsset>(asset);
	    }

	    virtual ~BmpEncPrivate()
	    {
	    }

	    bool Open( const char* directory )
	    {
	        m_directory = directory;
	        if ( m_VideoAsset ) {
	            // always make dir. Will fail if exists, but we don't care.
	            bfs::create_directory( directory );
	        }
	        return true;
	    }

	    void Close()
	    {
	    }

	    virtual const char* GetFileExtension() const { return "bmp"; }

	    void SetFormatString( const char* fmt )
	    {
	        m_format_string = fmt;
	    }

	    int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 )
	    {
	        if ( !m_VideoAsset) {
	            return 0;
	        }
	        // must be packed, non aligned - we write the whole structure at once
	#pragma pack( push, 2 )
	        struct BmpHeader
	        {
	            // BitmapHeader:
	            unsigned short  magic;          // format magic, always 'BM'
	            unsigned int    filesize;       // total file size, inc. headers
	            unsigned int    reserved;       // for future use
	            unsigned int    data_offset;    // image data offset in the file

	            // BitmapInfoHeader:
	            unsigned int    bih_size;       // 2nd part's size
	            unsigned int    width;          // bitmap's dimensions
	            unsigned int    height;
	            unsigned short  planes;         // num of planes
	            unsigned short  bpp;            // bits per pixel
	            unsigned int    compression;    // compression method
	            unsigned int    size_of_bmp;    // size of the bitmap
	            unsigned int    h_res;
	            unsigned int    v_res;          // image resolution in dpi
	            unsigned int    num_clrs;       // number of colors used
	            unsigned int    num_signif_clrs;// number of significant colors
	        } header;
	#pragma pack( pop )

	        int frames_written = 0;
	        memset( &header, 0xaa, sizeof( header ) );

	        int frame = frame_from;
	        if ( frame_to == -1 || frame_to >= m_VideoAsset->GetNumberOfFrames() ) {
	            frame_to = m_VideoAsset->GetNumberOfFrames() - 1;
	        }
	        double frame_time = m_VideoAsset->GetFormat().GetFrameTime();
	        std::ofstream out_stream;
	        while ( frame <= frame_to )
	        {
	            pei::Format tFmt;
	            pei::SurfacePtr pspec = m_VideoAsset->Decode( tFmt, (double)frame*frame_time );
	            if ( !pspec )
	            {
	                break;
	            }

	            char file[ 256 ];
	            std::string fms = m_directory + "/" + m_format_string + "." + GetFileExtension();
	            sprintf( file, fms.c_str(), offset + frame++ );

	            out_stream.open( file, std::fstream::out | std::fstream::binary );
	            if (!out_stream.good() )
	            {
	                break;
	            }
	            pspec->Lock();
	            const pei::Format& fmt = pspec->GetFormat();
	            unsigned short bpp    = pspec->GetBytesPerPixel() * 8;
	            pei::PalettePtr palette = pspec->GetPalette();
	            int palette_size      = palette == NULL ? 0 : palette->GetNumberOfColors();
	            int row_width         = pspec->GetPitch();

	            unsigned int   hdr_size = 14/*BitmapHeader*/ + 40/*BitmapInfoHeader*/;

	            header.magic           = (unsigned short)'MB'; // 0x4D42 = 'BM'; swapped
	            header.filesize        = (hdr_size + palette_size*4 + row_width * pspec->GetHeight());
	            header.reserved        = 0;
	            header.data_offset     = hdr_size + palette_size*4;

	            header.bih_size        = hdr_size - 14;
	            header.width           = pspec->GetWidth();
	            header.height          = pspec->GetHeight();
	            header.planes          = 1; // always 1 plane
	            header.bpp             = bpp;
	            header.compression     = 0; // RGB uncompressed
	            header.size_of_bmp     = row_width * pspec->GetHeight();
	            header.h_res           = header.v_res = 72;  // 72dpi is standard
	            header.num_clrs        = palette_size; // # colors in colormap
	            header.num_signif_clrs = 0;     // all colors are significant

	            out_stream.write( (const char*)&header, sizeof( header ) );

	            size_t rMask  = fmt.m_RMask;
	            size_t rShift = fmt.m_RShift;
	            size_t gMask  = fmt.m_GMask;
	            size_t gShift = fmt.m_GShift;
	            size_t bMask  = fmt.m_BMask;
	            size_t bShift = fmt.m_BShift;
	            size_t aMask  = fmt.m_AMask;
	            size_t aShift = fmt.m_AShift;

	            unsigned char *row_buffer = new unsigned char[row_width];
	            unsigned int y, x;
	            for ( y = 0; y < pspec->GetHeight(); y++ ) {
	                const size_t* srcp = (const size_t*)(((const unsigned char*)pspec->GetPixels())
	                                   + (pspec->GetHeight()-y-1) * pspec->GetPitch());
	                for ( x = 0; x < pspec->GetWidth(); x++ )
	                {
	                    size_t sp = *srcp++;
	                    row_buffer[4*x + 0] = (sp & bMask) >> bShift; // b
	                    row_buffer[4*x + 1] = (sp & gMask) >> gShift; // g
	                    row_buffer[4*x + 2] = (sp & rMask) >> rShift; // r
	                    row_buffer[4*x + 3] = (sp & aMask) >> aShift; // a
	                }
	                out_stream.write( (const char*)row_buffer, row_width );
	            }
	            out_stream.close();

	            delete [] row_buffer;

	            frames_written++;
	        };
	        return frames_written;
	    }
	};

	BmpEncoder::BmpEncoder( const AssetPtr asset )
	    : Encoder( asset )
	    , priv( new BmpEncPrivate( asset ) )
	{

	}

	BmpEncoder::~BmpEncoder()
	{
	    if ( priv ) {
	        delete priv;
	    }
	}

	bool BmpEncoder::Open( const char* file )
	{
	    return priv->Open( file );
	}

	void BmpEncoder::Close()
	{
	    priv->Close();
	}

	int BmpEncoder::WriteBuffers( int frame_from /*= 0*/, int frame_to /*= -1*/, int offset /*= 0*/ )
	{
	    return priv->WriteBuffers( frame_from, frame_to, offset );
	}

	void BmpEncoder::SetFormatString( const char* fmt )
	{
	    priv->SetFormatString( fmt );
	}

	const char* BmpEncoder::GetFileExtension() const
	{
	    return priv->GetFileExtension();
	}

}; // namespace pei
