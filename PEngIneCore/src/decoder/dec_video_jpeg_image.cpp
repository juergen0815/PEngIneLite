/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "dec_video_jpeg_image.h"

#include <video/blitter.h>
#include <video/render_tools.h>

#include <algorithm>
#include <string>
#include <locale>
#include <iostream>

namespace pei
{

    class JpegPrivateContext
    {
    public:
        JpegPrivateContext() {}

        virtual ~JpegPrivateContext() { close(); }

        virtual bool open( const bfs::path& file_name );

        virtual void close();

        virtual bool isOpen();

        virtual void readBuffer( char* buffer, unsigned int size );
    };

    bool JpegPrivateContext::open( const bfs::path& file_name )
    {
        return true;
    }

    void JpegPrivateContext::close()
    {
    }

    bool JpegPrivateContext::isOpen()
    {
        return false;
    }

    void JpegPrivateContext::readBuffer( char* buffer, unsigned int size )
    {
    }

    JpegImageAsset::JpegImageAsset( const char* n )
        : VideoAsset( n )
        , m_AssetName(n)
        , m_FileExtension(".jpg")
        , m_IsOpen(false)
        , m_Surface( new pei::Surface ) // create dummy surface
        , m_Context( new JpegPrivateContext )
    {
        bfs::path path_name( n );
        if ( bfs::exists( path_name ) && !bfs::is_directory( path_name ) ) {
            m_DirectoryName = path_name.parent_path().string();
            m_AssetName = path_name.filename();
        }
    }

    JpegImageAsset::~JpegImageAsset()
    {
        delete m_Context;
    	Close();
    }

    //////////////////////////////////////////////////////////////////////////
    // Asset Interface

    bool JpegImageAsset::CanHandle(  )
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

    bool JpegImageAsset::Open()
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

    void JpegImageAsset::Close( )
    {
        m_IsOpen = false;
        m_Surface = pei::SurfacePtr( new pei::Surface );
    }

    int JpegImageAsset::GetMemoryUsage()
    {
        return m_Surface->GetPitch() * m_Surface->GetHeight();
    }

    /** abstract format description */
    std::string JpegImageAsset::GetFormatString() const
    {
        return m_FileExtension; //??
    }

    //////////////////////////////////////////////////////////////////////////
    // VisualAsset interface

    /** get the number of frames in the video asset. Single frame returns 1 */
    int JpegImageAsset::GetNumberOfFrames() const
    {
        return 1;
    }

    /** animation/movie time in ms */
    double JpegImageAsset::GetLenght() const
    {
        return m_Surface->GetFormat().m_FrameTime;
    }

    /** read current frames per second */
    double JpegImageAsset::GetFps() const
    {
    	double frame_time = m_Surface->GetFormat().m_FrameTime;
        if ( frame_time > 0 ) {
            return 1000.0/frame_time;
        }
        return 0;
    }

    /** override fps for decoder */
    void JpegImageAsset::SetFps( double fps )
    {
    	// nothing to set. One frame
    }

    /** get a format description for the given frame. 0 if single image */
    const pei::Format& JpegImageAsset::GetFormat( ) const
    {
        return m_Surface->GetFormat();
    }

    /** decode a frame to the given format */
    pei::SurfacePtr JpegImageAsset::Decode( pei::Format& dest_format, double time /*= 0*/ )
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
