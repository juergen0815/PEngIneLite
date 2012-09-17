/************************************************************************/
/* pei::Engine (�)Copyright 2010 J�rgen Schober                         */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "enc_png_frames.h"

#include <decoder/dec_video.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <stdio.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include <png.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // number of CPU
#else
#include <unistd.h>
#endif

namespace bfs = boost::filesystem;
namespace pei
{
    class PngEncPrivate : public Encoder
    {
        friend class PngEncoder;
    protected:
        VideoAssetPtr m_VideoAsset;

        std::string     m_Directory;
        std::string     m_FormatString;
    protected:
        PngEncPrivate( const AssetPtr asset )
            : Encoder( asset )
            , m_FormatString("frame_%04d")
        {
            m_VideoAsset = boost::dynamic_pointer_cast<VideoAsset>(asset);
        }

        virtual ~PngEncPrivate()
        {
        }

        bool Open( const char* file )
        {
            m_Directory = file;
            if ( m_VideoAsset ) {
                // always make dir. Will fail if exists, but we don't care.
                bfs::create_directory( file );
            }
            return true;
        }

        void Close()
        {
        }

        void SetFormatString( const char* fmt )
        {
            m_FormatString = fmt;
        }

        virtual const char* GetFileExtension() const { return "png"; }

        int WriteRange( int first, int last, int offset /*= 0*/ )
        {
            int frames_written = 0;

            double frame_time = m_VideoAsset->GetFormat().GetFrameTime();
            int num_frames = m_VideoAsset->GetNumberOfFrames();
            int frame = first;
            for ( ; frame <= last; frame++)
            {
                pei::Format fmt;
                pei::SurfacePtr surface = m_VideoAsset->Decode( fmt, (double)frame*frame_time );
                if ( surface == NULL )
                {
                    break;
                }
                char file[ 256 ];
                if ( frame == first && num_frames == 1 )
                {
                    sprintf( file, "%s", std::string(m_Directory + "/" + m_FormatString).c_str() );
                } else
                {
                    std::string fms = m_Directory + "/" + m_FormatString + "." + GetFileExtension();
                    sprintf( file, fms.c_str(), offset + frame );
                }

                FILE *outfile = fopen( file, "wb" );
                if ( outfile )
                {
                    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ); //  &_png_struct;
                    if (!png_ptr) {
                        fclose(outfile);
                        /* If we get here, we had a problem reading the file */
                        break;
                    }
                    png_infop info_ptr = png_create_info_struct(png_ptr); // &_png_info;
                    if (!info_ptr) {
                        png_destroy_write_struct(&png_ptr, NULL);
                        fclose(outfile);
                        /* If we get here, we had a problem reading the file */
                        break;
                    }

                    if (setjmp(png_ptr->jmpbuf))
                    {
                        png_destroy_write_struct(&png_ptr, &info_ptr);
                        fclose(outfile);
                        /* If we get here, we had a problem reading the file */
                        break;
                    }

                    int pitch = surface->GetPitch();

                    /* set up the output control */
                    png_init_io(png_ptr, outfile);

                    /* set the file information here */
                    info_ptr->width       = surface->GetWidth();
                    info_ptr->height      = surface->GetHeight();
                    info_ptr->pixel_depth = surface->GetDepth();
                    info_ptr->channels    = (info_ptr->pixel_depth > 8 ) ? 4 : 1;
                    info_ptr->bit_depth   = info_ptr->pixel_depth/info_ptr->channels;
                    info_ptr->color_type  = PNG_COLOR_TYPE_RGB_ALPHA;
                    info_ptr->compression_type = info_ptr->filter_type = info_ptr->interlace_type=0;
                    info_ptr->valid       = 0;
                    info_ptr->rowbytes    = pitch;

                    png_write_info(png_ptr, info_ptr);
                    png_set_swap(png_ptr);

                    const pei::Format& sFormat = surface->GetFormat();
                    size_t rMask  = sFormat.m_RMask;
                    size_t rShift = sFormat.m_RShift;
                    size_t gMask  = sFormat.m_GMask;
                    size_t gShift = sFormat.m_GShift;
                    size_t bMask  = sFormat.m_BMask;
                    size_t bShift = sFormat.m_BShift;
                    size_t aMask  = sFormat.m_AMask;
                    size_t aShift = sFormat.m_AShift;

                    surface->Lock();
                    unsigned char* row_buffer = new unsigned char[ pitch ];
                    /* this is a pointer to one row of image data */
                    unsigned int y(0), x(0);
                    for ( y = 0;
                        y < sFormat.GetHeight();
                        y++ )
                    {
                        const size_t* srcp = (const size_t*)(((const unsigned char*)surface->GetPixels()) +
                                             (y/*pspec->format->m_height-y-1*/) * sFormat.GetPitch());
                        for ( x = 0; x < sFormat.GetWidth(); x++ )
                        {
                            size_t sp = *srcp++;
                            row_buffer[4*x + 0] = (sp & rMask) >> rShift; // r
                            row_buffer[4*x + 1] = (sp & gMask) >> gShift; // g
                            row_buffer[4*x + 2] = (sp & bMask) >> bShift; // b
                            row_buffer[4*x + 3] = (sp & aMask) >> aShift; // a
                        }
                        png_write_row(png_ptr, row_buffer );
                    }
                    surface->Unlock();

                    png_write_end(png_ptr, info_ptr);
                    png_destroy_write_struct(&png_ptr, &info_ptr);

                    fclose( outfile  );
                    delete [] row_buffer;

                    frames_written++;
                }
            };
            return frames_written;
        }

        int WriteBuffers( int frame_from = 0, int frame_to = -1, int offset = 0 )
        {
            if ( !m_VideoAsset) {
                return 0;
            }
            int frames_written = 0;
            if ( frame_to == -1 || frame_to >= m_VideoAsset->GetNumberOfFrames() ) {
                frame_to = m_VideoAsset->GetNumberOfFrames() - 1;
            }

#define _MULTI_THREADED_
#ifdef _MULTI_THREADED_
#ifdef _WIN32
            SYSTEM_INFO info;
            GetSystemInfo(&info);
            int num_cores = info.dwNumberOfProcessors;
//             if ( num_cores > 4 ) num_cores = 4; // i7 workaround - need to change to read physical cores, not hyper threads
#else
            int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
#endif
            int number_of_frames = m_VideoAsset->GetNumberOfFrames();
            frames_written = number_of_frames; // who cares...

            int num_frames_per_thread = number_of_frames / num_cores;
            int num_rounding_frames = number_of_frames % num_cores;
            int num_threads = (num_cores <= (int)number_of_frames) ? num_cores : num_rounding_frames;
            try {
                std::vector< boost::shared_ptr<boost::thread> > workers;
                int start_frame = 0;
                // create and spawn worker threads
                while ( --num_threads >= 0 )
                {
                    int num_frames = num_frames_per_thread + ( num_rounding_frames && 1 );
                    boost::shared_ptr< boost::thread > worker( new boost::thread( &pei::PngEncPrivate::WriteRange, this, start_frame, (start_frame + num_frames - 1), offset ));
                    workers.push_back( worker );

                    start_frame += num_frames;
                    if ( num_rounding_frames > 0 ) num_rounding_frames--;
                }
                // wait for all threads to finish
                std::vector< boost::shared_ptr<boost::thread> >::iterator it = workers.begin();
                for ( ; it != workers.end(); it++ )
                {
                    (*it)->join();
                }
            }
            catch ( ... )
            {
                // some exception
            }
#else
            frames_written = WriteRange(frame_from, frame_to, offset );
#endif
            return frames_written;
        }
    };

    PngEncoder::PngEncoder( const AssetPtr asset )
        : Encoder( asset )
        , priv( new PngEncPrivate( asset ) )
    {

    }

    PngEncoder::~PngEncoder()
    {
        if ( priv ) {
            delete priv;
        }
    }

    bool PngEncoder::Open( const char* file )
    {
        return priv->Open( file );
    }

    void PngEncoder::Close()
    {
        priv->Close();
    }

    int PngEncoder::WriteBuffers( int frame_from /*= 0*/, int frame_to /*= -1*/, int offset /*= 0*/ )
    {
        return priv->WriteBuffers( frame_from, frame_to, offset );
    }

    void PngEncoder::SetFormatString( const char* fmt )
    {
        priv->SetFormatString( fmt );
    }

    const char* PngEncoder::GetFileExtension() const
    {
        return priv->GetFileExtension();
    }

}; // namespace pei

