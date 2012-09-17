/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "dec_video_tiff_image.h"

#include <video/blitter.h>
#include <video/render_tools.h>

#include <tiffio.h>


namespace pei
{

    class TiffPrivateContext
    {
        TIFF *m_TiffImage;

    public:
        TiffPrivateContext() : m_TiffImage(NULL) {}

        ~TiffPrivateContext() { close(); }

        inline bool open( const bfs::path& file_name );

        inline void close();

        inline bool isOpen();

        inline TIFF *getHandle() const { return m_TiffImage; }
    };

    bool TiffPrivateContext::open( const bfs::path& file_name )
    {
        std::string name(file_name.string());
        TIFFSetWarningHandler(NULL);
        m_TiffImage = TIFFOpen( name.c_str(), "r");
        return m_TiffImage != NULL;
    }

    void TiffPrivateContext::close()
    {
        if ( m_TiffImage ) TIFFClose( m_TiffImage );
        m_TiffImage = NULL;
    }

    bool TiffPrivateContext::isOpen()
    {
        return m_TiffImage != NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    void* TiffImageAsset::GetTiffHandle() const
    {
        return m_Context->getHandle();
    }

    TiffImageAsset::TiffImageAsset( const char* n )
        : ImageSequenceAsset( n )
        , m_Context( new TiffPrivateContext )
        , m_DpiX(0)
        , m_DpiY(0)
    {
        // by default, this is not treated as an animation but one frame per index
        m_Format.m_FrameTime = 1.0;

        m_Extension = ".tiff";
        bfs::path path_name( n );
        if ( bfs::exists( path_name ) && !bfs::is_directory( path_name ) ) {
            m_DirectoryName = path_name.parent_path().string();
            m_ResourceName = path_name.filename();
        }
    }

    TiffImageAsset::~TiffImageAsset()
    {
        Close();
        delete m_Context;
    }

    //////////////////////////////////////////////////////////////////////////
    // Asset Interface

    bool TiffImageAsset::CanHandle(  )
    {
        if ( !m_IsOpen ) {
            bfs::path file_name( m_ResourceName.c_str() );
            file_name = operator/( m_DirectoryName.c_str(), file_name );
            if (!bfs::exists( file_name )) {
                return m_IsOpen;
            }
            return m_Context->open( file_name );
        }
        return m_IsOpen;
    }

    bool TiffImageAsset::Open()
    {
    	if ( m_IsOpen == false ) {
			bfs::path file_name( m_ResourceName.c_str() );
			file_name = operator/( m_DirectoryName.c_str(), file_name );
			if (!bfs::exists( file_name )) {
				return false;
			}
			bool canLoad = m_Context->open( file_name );
            if ( canLoad ) {
                uint32 width, height;
                TIFF* image = m_Context->getHandle();
                if(TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &width) == 0) {
                    //
                    return false;
                }
                if(TIFFGetField(image, TIFFTAG_IMAGELENGTH, &height) == 0){
                    return false;
                }
                // Store resolution
                TIFFGetField(image, TIFFTAG_XRESOLUTION, &m_DpiX);
                TIFFGetField(image, TIFFTAG_YRESOLUTION, &m_DpiY);

                // TODO: Add other formats than RGBA - natively
                m_Format.SetWidth(width);
                m_Format.SetHeight(height);
                do {
                    // always read RGBA data
                    pei::SurfacePtr surface( new pei::Surface(m_Format) );
                    if (!TIFFReadRGBAImage(image, width, height, (unsigned int*)surface->GetPixels(), 0) )
                    {
                        return false;
                    }

                    /* libtiff loads the image upside-down, flip it back - stole this from SDL_Image/tiff loader. Sorry Sam;) */
                    uint32 half = height / 2;
                    for( uint32 y = 0; y < half; y++)
                    {
                        uint32 *top = (uint32*)surface->GetPixels() + y * surface->GetPitch()/4;
                        uint32 *bot = (uint32*)surface->GetPixels() + (height - y - 1) * surface->GetPitch()/4;
                        for( uint32 x = 0; x < width; x++)
                        {
                            uint32 tmp = top[x];
                            top[x] = bot[x];
                            bot[x] = tmp;
                        }
                    }
                    m_FrameCache.push_back( surface );

                } while ( TIFFReadDirectory( image ) );

                m_IsOpen = true;
            }
    	}
        return m_IsOpen;
    }

    void TiffImageAsset::Close( )
    {
        ImageSequenceAsset::Close();
        m_Context->close();
    }

    /** abstract format description */
    std::string TiffImageAsset::GetFormatString() const
    {
        return m_Extension;
    }

    int TiffImageAsset::GetNumberOfFrames() const
    {
        return (int)m_FrameCache.size();
    }

    pei::SurfacePtr TiffImageAsset::DecodeFrame( int frame_num )
    {
        if ( (int)m_FrameCache.size() > frame_num ) {
            return m_FrameCache[ frame_num ];
        }
        return pei::SurfacePtr( );
    }

}; // namespace pei
