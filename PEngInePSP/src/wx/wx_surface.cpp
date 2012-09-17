/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "wx_surface.h"

#include <render/color.h>

namespace pei {
    namespace wx
    {
        class Format : public pei::Format
        {
        public:
            inline int MaskToBitShift( int mask )
            {
                switch ( mask )
                {
                // do we need endian magic here??? hmmm....
                case 0xff000000: return 24;
                case 0x00ff0000: return 16;
                case 0x0000ff00: return 8;
                case 0x000000ff: return 0;
                default: break;
                }
                // invalid mask!!
                return 0;
            }

            Format( const wxImage& image )
            {
                m_Width  = m_CanvasWidth  = image.GetWidth();
                m_Height = m_CanvasHeight = image.GetWidth();
                m_Depth         = image.HasAlpha() ? 32 : 24;
                m_BytesPerPixel = image.HasAlpha() ? 4 : 3;
                m_Pitch  = m_Width * m_BytesPerPixel;

                m_RMask = image.GetMaskRed();
                m_GMask = image.GetMaskGreen();
                m_BMask = image.GetMaskBlue();
                m_AMask = ~(m_RMask|m_GMask|m_BMask);
                m_RShift= MaskToBitShift(m_RMask);
                m_GShift= MaskToBitShift(m_GMask);
                m_BShift= MaskToBitShift(m_BMask);
                m_AShift= MaskToBitShift(m_AMask);
                m_RLoss = 0;
                m_GLoss = 0;
                m_BLoss = 0;
                m_ALoss = (m_Depth == 3) ? 8 : 0;
            }
        };

        Surface::Surface( const wxImage& image )
        {
            SetPixels(image);
        }

        Surface::Surface( const wxBitmap& bitmap )
        {
            wxImage image(bitmap.ConvertToImage());
            SetPixels(image);
        }

        Surface::Surface( const pei::SurfacePtr& other )
        {
            m_Format = other->GetFormat();
            size_t numPixels = (size_t)m_Format.m_Pitch*m_Format.m_CanvasHeight;
            m_Pixels = (void*)new char[ numPixels ];
            other->Lock();
            memcpy( m_Pixels, other->GetPixels(), numPixels );
            if ( other->GetPalette() ) {
                m_Palette = pei::PalettePtr( new pei::Palette(*other->GetPalette().get()) );
            }
            other->Unlock();
        }

        wxImage Surface::asImage() const
        {
            pei::SurfacePtr sp( (pei::Surface*) this, NullDeleter() );
            return Surface::ConveToImage( sp );
        }

        void Surface::SetPixels( const wxImage& image )
        {
            m_Format = Format( image );
            size_t numPixels = (size_t)m_Format.m_Pitch*m_Format.m_CanvasHeight;
            if ( m_Format.GetDepth() == 24 ) {
                m_Pixels = (void*)new char[ numPixels ];
                memcpy( m_Pixels, image.GetData(), numPixels );
            } else if ( m_Format.GetDepth() == 32 ) {
                // wx uses a screwed up image format with a separate alpha plane. We need to convert into RGBA
                m_Pixels = (void*)new char[ numPixels ];
                unsigned int sPitch  = image.GetWidth()*3;
                unsigned char* pData = image.GetData();
                unsigned char* pAlpha= image.GetAlpha();
                unsigned int aMask = m_Format.m_AMask;
                unsigned int aShift= m_Format.m_AShift;
                int w = (int)m_Format.GetWidth();
                int h = (int)m_Format.GetHeight();
                for ( int y = 0; y < h; y++ ) {
                    unsigned int *pSrc  = (unsigned int*)(pData + y*sPitch);
                    unsigned int *pDst  = (unsigned int*)((unsigned char*)m_Pixels + y*m_Format.GetPitch());
                    for ( int x = 0; x < w; x++ ) {
                        *pDst++  = ((*pSrc) & ~aMask) | ((*pAlpha) << aShift);
                        pAlpha++; pSrc = (unsigned int *)(((unsigned char*)pSrc) + 3); // performance issues with odd addresses???
                    }
                }
            } else if ( m_Format.GetDepth() == 8 ) {
                // 8 bit gray scale - downscale to gray using luma

            } else if ( m_Format.GetDepth() == 1 ) {
                // 1 bit bitmap
            } else {
                // cannot compute
                m_Pixels = NULL;
            }
        }

        wxImage Surface::ConveToImage( const pei::SurfacePtr& surface )
        {
            surface->Lock();
            void* pixels = surface->GetPixels();
            if ( pixels ) {
                const pei::Format& fmt = surface->GetFormat();

                int w = (int)fmt.GetWidth();
                int h = (int)fmt.GetHeight();

                unsigned int rMask = fmt.m_RMask;
                unsigned int rShift= fmt.m_RShift;
                unsigned int gMask = fmt.m_GMask;
                unsigned int gShift= fmt.m_GShift;
                unsigned int bMask = fmt.m_BMask;
                unsigned int bShift= fmt.m_BShift;
                unsigned int aMask = fmt.m_AMask;
                unsigned int aShift= fmt.m_AShift;
                unsigned int pitch = surface->GetPitch();

                if ( fmt.m_Depth == 32 ) {
                    // use malloc - wxImage will free the data
                    unsigned char* pRGB= (unsigned char*)malloc( w*h*3 );
                    unsigned char* pAlpha = (unsigned char*)malloc( w*h );

                    for ( int y = 0; y < h; y++ ) {
                        unsigned int  *pSrc = (unsigned int*)((unsigned char*)pixels + y*pitch);
                        unsigned char *pDst = (unsigned char*)(pRGB + y*w*3);
                        unsigned char *pA   = (unsigned char*)(pAlpha + y*w);
                        for ( int x = 0; x < w; x++ ) {
                            unsigned int pix = *pSrc++;
                            *pDst++  = ( pix & rMask) >> rShift;
                            *pDst++  = ( pix & gMask) >> gShift;
                            *pDst++  = ( pix & bMask) >> bShift;
                            *pA++    = ( pix & aMask) >> aShift;
                        }
                    }
                    surface->Unlock();
                    return wxImage( w,h, pRGB, pAlpha, false );
                }
                else if ( fmt.m_Depth == 24 ) {
                    // use malloc - wxImage will free the data
                    unsigned char* pRGB= (unsigned char*)malloc( w*h*3 );

                    for ( int y = 0; y < h; y++ ) {
                        unsigned int  *pSrc = (unsigned int*)((unsigned char*)pixels + y*pitch);
                        unsigned char *pDst = (unsigned char*)(pRGB + y*w*3);
                        for ( int x = 0; x < w; x++ ) {
                            unsigned char pix = *pSrc++;
                            *pDst++  = pix; // ( pix & rMask) >> rShift;
                            *pDst++  = ( pix & gMask) >> gShift;
                            *pDst++  = ( pix & bMask) >> bShift;
                        }
                    }
                    surface->Unlock();
                    return wxImage( w,h, pRGB, false );
                }
                else if ( fmt.m_Depth == 8 ) {
                    // use malloc - wxImage will free the data
                    unsigned char* pRGB= (unsigned char*)malloc( w*h*3 );

                    for ( int y = 0; y < h; y++ ) {
                        unsigned char *pSrc = ((unsigned char*)pixels + y*pitch);
                        unsigned char *pDst = (unsigned char*)(pRGB + y*w*3);
                        for ( int x = 0; x < w; x++ ) {
                            unsigned char pix = *pSrc++;
                            *pDst++ = pix;
                            *pDst++ = pix;
                            *pDst++ = pix;
                        }
                    }
                    surface->Unlock();
                    return wxImage( w,h, pRGB, false );
                }
            }
            surface->Unlock();
            return NULL;
        }

        SurfaceRef::SurfaceRef( const pei::SurfacePtr& ref )
            : m_SurfaceRef(ref)
        {
            // hard link pixels and palette
            m_Pixels  = ref->GetPixels();
            m_Palette = ref->GetPalette();
            // copy format
            m_Format  = ref->GetFormat();
        }

        wxImage SurfaceRef::asImage() const {
            return Surface::ConveToImage( m_SurfaceRef );
        }

    }
}
