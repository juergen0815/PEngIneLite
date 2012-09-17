/************************************************************************/
/* pei::Engine (©)Copyright 2010 Jürgen Schober                         */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "dec_video_png_image.h"

#include <video/blitter.h>
#include <video/render_tools.h>

#include <png.h>

#include <algorithm>
#include <string>
#include <locale>
#include <iostream>


namespace pei
{

    class PngPrivateContext
    {
        bios::mapped_file m_Mmap;
        unsigned long     m_Cursor;
    public:
        PngPrivateContext() : m_Cursor(0) {}

        virtual ~PngPrivateContext() { close(); }

        virtual bool open( const bfs::path& file_name );

        virtual void close();

        virtual bool isOpen();

        virtual void readBuffer( char* buffer, unsigned int size );
    };

    bool PngPrivateContext::open( const bfs::path& file_name )
    {
        if ( !m_Mmap.is_open() ) {
            m_Mmap.open( file_name, std::ios_base::binary | std::ios_base::in );
            if ( m_Mmap.is_open() && m_Mmap.size() >= 8 ) {
                const char* magic = m_Mmap.const_data();
                bool canLoad = png_sig_cmp((png_bytep)magic, 0, 8) == 0;
                if ( canLoad ) {
                    // skip header for subsequent reads
                    m_Cursor = 8;
                    return true;
                }
                m_Mmap.close();
            }
            return false;
        }
        m_Cursor = 0;
        return true;
    }

    void PngPrivateContext::close() { m_Mmap.close(); m_Cursor = 0; }

    bool PngPrivateContext::isOpen() { return m_Mmap.is_open(); }

    void PngPrivateContext::readBuffer( char* buffer, unsigned int size )
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

    static void png_read_data(png_structp ctx, png_bytep data, png_size_t size)
    {
        PngPrivateContext *o = (PngPrivateContext*)png_get_io_ptr(ctx);
        if ( o ) {
            o->readBuffer( (char*)data, size );
        }
    }

    PngImageAsset::PngImageAsset( const char* n )
        : VideoAsset( n )
        , m_AssetName(n)
        , m_FileExtension(".png")
        , m_IsOpen(false)
        , m_Surface( new pei::Surface ) // create dummy surface
        , m_Context( new PngPrivateContext )
    {
        bfs::path path_name( n );
        if ( bfs::exists( path_name ) && !bfs::is_directory( path_name ) ) {
            m_DirectoryName = path_name.parent_path().string();
            m_AssetName = path_name.filename();
        }
    }

    PngImageAsset::~PngImageAsset()
    {
        delete m_Context;
    	Close();
    }

    //////////////////////////////////////////////////////////////////////////
    // Asset Interface

    bool PngImageAsset::CanHandle(  )
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

    bool PngImageAsset::Open()
    {
    	if ( m_IsOpen == false ) {
			bfs::path file_name( m_AssetName.c_str() );
			file_name = operator/( m_DirectoryName.c_str(), file_name );
			if (!bfs::exists( file_name )) {
				return false;
			}
			bool canLoad = m_Context->open( file_name );
            if ( canLoad ) {
                //create png struct
                png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
                if (!png_ptr) {
                    m_Context->close();
                    return false;
                }

                //create png info struct
                png_infop info_ptr = png_create_info_struct(png_ptr);
                if (!info_ptr) {
                    png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
                    m_Context->close();
                    return false;
                }

                //create png info struct
                png_infop end_info = png_create_info_struct(png_ptr);
                if (!end_info) {
                    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
                    m_Context->close();
                    return false;
                }

                //setup error handling (required without using custom error handlers above)
                if (setjmp(png_jmpbuf(png_ptr))) {
                    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                    m_Context->close();
                    return false;
                }

                png_set_read_fn(png_ptr, (png_voidp)m_Context, png_read_data);

                //let libpng know you already read the first 8 bytes
                png_set_sig_bytes(png_ptr, 8);

                // read all the info up to the image data
                png_read_info(png_ptr, info_ptr);

                //variables to pass to get info
                int bit_depth, color_type;
                png_uint_32 image_width, image_height;

                // get info about png
                png_get_IHDR(png_ptr, info_ptr, &image_width, &image_height, &bit_depth, &color_type, NULL, NULL, NULL);

                // Update the png info struct.
                png_read_update_info(png_ptr, info_ptr);

                // Row size in bytes.
                int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

                pei::Format fmt;
                int pitch(rowbytes);
#ifdef MAKE_POW_2
                int iw(makePow2(image_width));
                int ih(makePow2(image_height));
#endif
                int iw(image_width);
                int ih(image_height);
                switch (color_type)
                {
                    case PNG_COLOR_TYPE_RGBA:
                    	pitch = iw*4;
                        fmt = pei::Format( iw, ih , pitch, 32, 4 );
                        break;
                    case PNG_COLOR_TYPE_RGB:
                    	pitch = iw*3;
                        fmt = pei::Format( iw, ih, pitch, 24, 3 );
                        fmt.m_ALoss = 8; // no alpha!
                        break;
                    case PNG_COLOR_TYPE_GRAY_ALPHA:
                        // LUMA+ALPHA
                    	pitch = iw*2;
                        fmt = pei::Format( iw, ih, pitch, 16, 2, pei::Format::TYPE_LUMINANCE_ALPHA );
                        break;
                    case PNG_COLOR_TYPE_GRAY:
                        // we treat gray as luma only
                    	pitch = iw;
                        fmt = pei::Format( iw,ih, pitch, 8, 1, pei::Format::TYPE_LUMINANCE );
                        break;
                    default:
                        if ( color_type & PNG_COLOR_MASK_PALETTE ) {
                            // Palettized images
                            if ( bit_depth == 1 ) {
                                pitch = iw >> 3;
                                fmt = pei::Format( iw,ih, pitch, 1, 0, pei::Format::TYPE_BITMAP );
                                break;
                            }
                        }
                        std::cerr << "Unsupported PNG color type " << (int)color_type << " for texture: " << file_name.string() << std::endl;
                        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                        m_Context->close();
                        return false;
                }
#ifdef MAKE_POW_2
                fmt.m_Width  = image_width;
                fmt.m_Height = image_height;
#endif
                // allocate a surface buffer
                pei::SurfacePtr surface( new pei::Surface(fmt) );
#ifdef MAKE_POW_2
                // clear it??
#endif
                // make pixel buffer available to png
                png_bytep image_data = (png_bytep)surface->GetPixels();

                //row_pointers is for pointing to image_data for reading the png with libpng
                png_bytepp row_pointers = new png_bytep[image_height];
                if (!row_pointers) {
                    //clean up memory and close stuff
                    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                    m_Context->close();
                    return false;
                }

                // set the individual row_pointers to point at the correct offsets of image_data
                for ( unsigned int i = 0; i < image_height; i++) {
                    // do not flip rows
                    row_pointers[i] = image_data + i * rowbytes;
                }

                //read the png into image_data through row_pointers
                png_read_image(png_ptr, row_pointers);

                //clean up memory and close stuff
                png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
                delete [] row_pointers;
                if ( fmt.GetType() == pei::Format::TYPE_BITMAP ) {
                    // expands this to 8 bit luma - but keep type BITMAP
                    m_Surface = pei::SurfacePtr( new pei::Surface(pei::Format( iw,ih, iw, 8, 1, pei::Format::TYPE_BITMAP )) );
                    pei::Blitter::Blit( surface, m_Surface, 0, 0, iw, ih, 0, 0,
                            pei::PixOpPtr( new PixOpExpandBitmap(m_Surface->GetFormat(), 0xff, 0x00 ) ) );
                } else {
                    m_Surface = surface;
                }
                m_IsOpen = true;
                m_Context->close();
            }
    	}
        return m_IsOpen;
    }

    void PngImageAsset::Close( )
    {
        m_IsOpen = false;
        m_Surface = pei::SurfacePtr( new pei::Surface );
    }

    int PngImageAsset::GetMemoryUsage()
    {
        return m_Surface->GetPitch() * m_Surface->GetHeight();
    }

    /** abstract format description */
    std::string PngImageAsset::GetFormatString() const
    {
        return m_FileExtension; //??
    }

    //////////////////////////////////////////////////////////////////////////
    // VisualAsset interface

    /** get the number of frames in the video asset. Single frame returns 1 */
    int PngImageAsset::GetNumberOfFrames() const
    {
        return 1;
    }

    /** animation/movie time in ms */
    double PngImageAsset::GetLenght() const
    {
        return m_Surface->GetFormat().m_FrameTime;
    }

    /** read current frames per second */
    double PngImageAsset::GetFps() const
    {
    	double frame_time = m_Surface->GetFormat().m_FrameTime;
        if ( frame_time > 0 ) {
            return 1000.0/frame_time;
        }
        return 0;
    }

    /** override fps for decoder */
    void PngImageAsset::SetFps( double fps )
    {
    	// nothing to set. One frame
    }

    /** get a format description for the given frame. 0 if single image */
    const pei::Format& PngImageAsset::GetFormat( ) const
    {
        return m_Surface->GetFormat();
    }

    /** decode a frame to the given format */
    pei::SurfacePtr PngImageAsset::Decode( pei::Format& dest_format, double time /*= 0*/ )
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
