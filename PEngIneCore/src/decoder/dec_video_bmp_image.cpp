/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "dec_video_bmp_image.h"

#include <video/blitter.h>
#include <video/render_tools.h>

#include <algorithm>
#include <string>
#include <locale>
#include <iostream>

namespace pei
{

    class BmpPrivateContext
    {
        bios::mapped_file m_Mmap;
        unsigned long     m_Cursor;
    public:
        BmpPrivateContext() {}

        virtual ~BmpPrivateContext() { close(); }

        virtual bool open( const bfs::path& file_name );

        virtual void close();

        virtual bool isOpen();

        virtual void readBuffer( char* buffer, unsigned int size );
    };

    bool BmpPrivateContext::open( const bfs::path& file_name )
    {
        if ( !m_Mmap.is_open() ) {
            m_Mmap.open( file_name, std::ios_base::binary | std::ios_base::in );
            if ( m_Mmap.is_open() && m_Mmap.size() >= 8 ) {
                const char* magic = m_Mmap.const_data();
                bool canLoad = strncmp(magic, "BM", 2) == 0;
                if ( canLoad ) {
                    // skip header for subsequent reads
                    m_Cursor = 2;
                    return true;
                }
                m_Mmap.close();
            }
            return false;
        }
        m_Cursor = 0;
        return true;
    }

    void BmpPrivateContext::close()
    {
        m_Mmap.close(); m_Cursor = 0;
    }

    bool BmpPrivateContext::isOpen()
    {
        return m_Mmap.is_open();
    }

    void BmpPrivateContext::readBuffer( char* buffer, unsigned int size )
    {
        unsigned long fsize(m_Mmap.size());
        if ( fsize > 0 && m_Cursor < fsize ) {
            if ( (size+m_Cursor) > fsize ) size = fsize - m_Cursor;
            const char* data = m_Mmap.const_data();
            if ( data && buffer ) {
                memcpy( buffer, data+m_Cursor, size );
            }
            m_Cursor += size;
        }
    }

    BmpImageAsset::BmpImageAsset( const char* n )
        : VideoAsset( n )
        , m_AssetName(n)
        , m_FileExtension(".bmp")
        , m_IsOpen(false)
        , m_Surface( new pei::Surface ) // create dummy surface
        , m_Context( new BmpPrivateContext )
    {
        bfs::path path_name( n );
        if ( bfs::exists( path_name ) && !bfs::is_directory( path_name ) ) {
            m_DirectoryName = path_name.parent_path().string();
            m_AssetName = path_name.filename();
        }
    }

    BmpImageAsset::~BmpImageAsset()
    {
        delete m_Context;
    	Close();
    }

    //////////////////////////////////////////////////////////////////////////
    // Asset Interface

    bool BmpImageAsset::CanHandle(  )
    {
        if ( !m_IsOpen ) {
            bfs::path file_name( m_AssetName.c_str() );
            file_name = operator/( m_DirectoryName.c_str(), file_name );
            if (!bfs::exists( file_name )) {
                return m_IsOpen;
            }
            m_IsOpen = m_Context->open( file_name );
        }
        return m_IsOpen;
    }

    bool BmpImageAsset::Open()
    {
    	if ( m_IsOpen == false ) {
			bfs::path file_name( m_AssetName.c_str() );
			file_name = operator/( m_DirectoryName.c_str(), file_name );
			if (!bfs::exists( file_name )) {
				return false;
			}
			bool canLoad = m_Context->open( file_name );
            if ( canLoad ) {
                pei::Format fmt;
#ifdef MAKE_POW_2
                int iw(makePow2(image_width));
                int ih(makePow2(image_height));
#endif
#ifdef MAKE_POW_2
                fmt.m_Width  = image_width;
                fmt.m_Height = image_height;
#endif
                // allocate a surface buffer
                pei::SurfacePtr surface( new pei::Surface(fmt) );

                m_Surface = surface;
                m_IsOpen = true;
                m_Context->close();
            }
    	}
        return m_IsOpen;
    }

    void BmpImageAsset::Close( )
    {
        m_IsOpen = false;
        m_Surface = pei::SurfacePtr( new pei::Surface );
    }

    int BmpImageAsset::GetMemoryUsage()
    {
        return m_Surface->GetPitch() * m_Surface->GetHeight();
    }

    /** abstract format description */
    std::string BmpImageAsset::GetFormatString() const
    {
        return m_FileExtension; //??
    }

    //////////////////////////////////////////////////////////////////////////
    // VisualAsset interface

    /** get the number of frames in the video asset. Single frame returns 1 */
    int BmpImageAsset::GetNumberOfFrames() const
    {
        return 1;
    }

    /** animation/movie time in ms */
    double BmpImageAsset::GetLenght() const
    {
        return m_Surface->GetFormat().m_FrameTime;
    }

    /** read current frames per second */
    double BmpImageAsset::GetFps() const
    {
    	double frame_time = m_Surface->GetFormat().m_FrameTime;
        if ( frame_time > 0 ) {
            return 1000.0/frame_time;
        }
        return 0;
    }

    /** override fps for decoder */
    void BmpImageAsset::SetFps( double fps )
    {
    	// nothing to set. One frame
    }

    /** get a format description for the given frame. 0 if single image */
    const pei::Format& BmpImageAsset::GetFormat( ) const
    {
        return m_Surface->GetFormat();
    }

    /** decode a frame to the given format */
    pei::SurfacePtr BmpImageAsset::Decode( pei::Format& dest_format, double time /*= 0*/ )
    {
        // we got closed or released for some reason
        if ( !m_IsOpen ) {
            m_IsOpen = Open();
        }
        if ( m_IsOpen ) {
            // decoder does not scale! - renderer does, though - we still want the format from the surface to match the destination format
            double dx = dest_format.GetXScale();
            double dy = dest_format.GetYScale();
            dest_format = m_Surface->GetFormat();
            dest_format.SetXScale( dx );
            dest_format.SetYScale( dy );
        }
        return m_Surface;
    }

}; // namespace pei
