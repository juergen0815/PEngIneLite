/************************************************************************/
/* pei::Engine (�)Copyright 2010 J�rgen Schober                         */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "dec_video_image_sequence.h"

#include <video/blitter.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>

#include <stdio.h>

#define FPS_BASE 60.0

namespace bfs = boost::filesystem;
namespace pei {

        ImageSequenceAsset::ImageSequenceAsset( const char* dir, bool load_to_memory )
            : VideoAsset( dir )
            , m_DirectoryName( dir )
            , m_MaxMemUsed( 0 )
            , m_CacheFrames(load_to_memory)
            , m_IsOpen(false)
        {
            m_Format.m_FrameTime = 1000.0/FPS_BASE;
            bfs::path anim_path( bfs::system_complete( bfs::path( dir ) ) );
            if ( bfs::exists( anim_path ) && bfs::is_directory( anim_path ) ) {

                // get the directory name and call the resource according to it
                m_ResourceName = anim_path.stem();

                bfs::directory_iterator dir_end;
                bfs::directory_iterator dir_it( anim_path );
                while ( dir_it != dir_end )
                {
                    try {
                        if ( bfs::is_regular( dir_it->status() )) {
                            std::string file_string = dir_it->path().file_string();
                            m_FileNames.push_back( file_string );
                        }
                    }
                    catch ( const std::exception& ex ) {
                        std::cerr << "Exception: " << ex.what() << std::endl;
                    }
                    dir_it++;
                }
                if ( m_FileNames.size() > 0 ) {
                    std::sort( m_FileNames.begin(), m_FileNames.end() );
                }
            }
        }

        ImageSequenceAsset::ImageSequenceAsset( const char* dir, const std::vector<std::string>& names, bool load_to_memory )
            : VideoAsset( dir )
            , m_DirectoryName( dir )
            , m_ResourceName( "" )
            , m_MaxMemUsed( 0 )
            , m_CacheFrames(load_to_memory)
            , m_IsOpen(false)
        {
            m_Format.m_FrameTime = 1000.0/FPS_BASE;
            bfs::path directory( bfs::system_complete( bfs::path( dir ) ) );
            if ( bfs::exists( directory ) && bfs::is_directory( directory ) ) {
                m_DirectoryName = directory.directory_string();
            }
            std::vector<std::string>::const_iterator it = names.begin();
            while (it != names.end())
            {
                std::string file_string( *it++ );
                m_FileNames.push_back( file_string );
            }
            if ( m_FileNames.size() > 0 ) {
                std::sort( m_FileNames.begin(), m_FileNames.end() );

                bfs::path path( m_FileNames[0] );
                m_ResourceName = path.filename( );
                size_t pos = m_ResourceName.rfind( path.extension());
                if ( pos != std::string::npos ) {
                    m_ResourceName = m_ResourceName.substr( 0, pos );
                }
                pos = m_ResourceName.rfind( "_" );
                if ( pos != std::string::npos ) {
                    m_ResourceName = m_ResourceName.substr( 0, pos );
                }
            }
        }

        ImageSequenceAsset::ImageSequenceAsset( const char* pattern, int from, int to, bool load_to_memory )
            : VideoAsset( pattern )
            , m_DirectoryName( pattern )
            , m_ResourceName( "" )
            , m_MaxMemUsed( 0 )
            , m_CacheFrames(load_to_memory)
            , m_IsOpen(false)
        {
            m_Format.m_FrameTime = 1000.0/FPS_BASE;

            char buffer[256];
            while ( from <= to ) {
                sprintf( buffer, pattern, from++ );
                bfs::path anim_path( bfs::system_complete( bfs::path( buffer ) ) );
                if ( bfs::exists( anim_path ) && !bfs::is_directory( anim_path ) ) {

                    // std::cout << "Found image @ " << buffer << std::endl;
                    // get the directory name and call the resource according to it
                    if ( m_ResourceName.empty() ) m_ResourceName = anim_path.stem();
                    std::string file_string = anim_path.file_string();
                    m_FileNames.push_back( file_string );
                }
            }
        }

        ImageSequenceAsset::~ImageSequenceAsset()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        // Asset Interface

        pei::SurfacePtr ImageSequenceAsset::DecodeFrame( int frame_num )
        {
            if ( (int)m_FrameCache.size() > frame_num ) {
                return m_FrameCache[ frame_num ];
            } else {
                bool decode_frame( (int)m_FileNames.size() > frame_num);
                if ( decode_frame ) {
                    return ReadFrame( m_FileNames[frame_num].c_str() );
                }
            }
            return pei::SurfacePtr( );
        }

        bool ImageSequenceAsset::CanHandle(  )
        {
            return m_FileNames.size() > 0 && DecodeFrame ( 0 ).get() != NULL;
        }

        bool ImageSequenceAsset::Open(  )
        {
            m_MaxMemUsed = 0;
            if ( !m_IsOpen ) {
                if ( m_CacheFrames ) {
                    std::vector<std::string>::iterator it = m_FileNames.begin();
                    while ( it != m_FileNames.end() ) {
                        std::string& fname = *it++;
                        // might not be an image file
                        pei::SurfacePtr s = ReadFrame( fname.c_str() );
                        if ( s ) {
                            m_FrameCache.push_back( s );
                            double w = s->GetWidth();
                            double h = s->GetHeight();

                            m_MaxMemUsed = (int)(m_MaxMemUsed + s->GetPitch() * h);
                            if ( m_Format.m_Width  < w ) m_Format.m_Width  = w;
                            if ( m_Format.m_Height < h ) m_Format.m_Height = h;
                            if ( m_Format.m_CanvasWidth  < w ) m_Format.m_CanvasWidth  = w;
                            if ( m_Format.m_CanvasHeight < h ) m_Format.m_CanvasHeight = h;
                        }
                    }
                    m_IsOpen = m_FrameCache.size() > 0;
                } else {
                    m_IsOpen = (m_FileNames.size() > 0 && DecodeFrame ( 0 ).get() != NULL);
                }
            }
            return m_IsOpen;
        }

        void ImageSequenceAsset::Close( )
        {
            m_Format.m_FrameTime = 1000.0/FPS_BASE;
            m_FrameCache.clear();
            m_MaxMemUsed = 0;
            m_Format = pei::Format();
            m_IsOpen = false;
        }

        int ImageSequenceAsset::GetMemoryUsage()
        {
            return m_MaxMemUsed;
        }

        //////////////////////////////////////////////////////////////////////////
        // VisualAsset interface

        /** get the number of frames in the video asset. Single frame returns 1 */
        int ImageSequenceAsset::GetNumberOfFrames() const
        {
            return (int)m_FileNames.size();
        }

        /** animation/movie time in ms */
        double ImageSequenceAsset::GetLenght() const
        {
            return m_Format.m_FrameTime * GetNumberOfFrames();
        }

        /** read current frames per second */
        double ImageSequenceAsset::GetFps() const
        {
            if ( m_Format.m_FrameTime > 0 ) {
                return 1000.0/m_Format.m_FrameTime;
            }
            return 0;
        }

        /** override fps for decoder */
        void ImageSequenceAsset::SetFps( double fps )
        {
            if ( fps > 0  ) {
                m_Format.m_FrameTime = 1000.0/fps;
            }
        }

        /** get a format description for the given frame. 0 if single image */
        const pei::Format& ImageSequenceAsset::GetFormat( ) const
        {
            return m_Format;
        }

        /** decode a frame to the given format */
        pei::SurfacePtr ImageSequenceAsset::Decode( pei::Format& dest_format, double time /*= 0*/ )
        {
            if ( m_FileNames.size() > 0 ) {
                int frame = m_Format.m_FrameTime > 0 ? (int)(time / m_Format.m_FrameTime) : 0;
                dest_format = m_Format;
                // cannot decode x/y offsets from single frames
                dest_format.m_XOffset = 0;
                dest_format.m_YOffset = 0;
                return DecodeFrame( frame % m_FileNames.size() );
            } else if ( m_FrameCache.size() > 0 ) {
                int frame = m_Format.m_FrameTime > 0 ? (int)(time / m_Format.m_FrameTime) : 0;
                dest_format = m_Format;
                // cannot decode x/y offsets from single frames
                dest_format.m_XOffset = 0;
                dest_format.m_YOffset = 0;
                return DecodeFrame( frame % m_FrameCache.size() );
            }
            return pei::SurfacePtr();
        }

}; // namespace pei


