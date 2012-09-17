/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "blitter.h"

namespace pei
{
    typedef void (*copyPixFunc)( PixOpPtr& pop, const void*, void*);

//    template< class S, class D >
//    void composeT( PixOpPtr& pop, const S *s, D  *d )
//    {
//        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
//        *d = (D)pop->Compose( (uint32)(*s), (uint32)(*d) );
//    }

    // 8 -> 8
    static void compose0( PixOpPtr& pop, const uint8 *s, uint8 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 16 -> 8
    static void compose1( PixOpPtr& pop, const uint16 *s, uint8 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 24/32 -> 8
    static void compose2( PixOpPtr& pop, const uint32 *s, uint8 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 8 -> 16
    static void compose3( PixOpPtr& pop, const uint8 *s, uint16 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 16 -> 16
    static void compose4( PixOpPtr& pop, const uint16 *s, uint16 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 24/32 -> 16
    static void compose5( PixOpPtr& pop, const uint32 *s, uint16 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 8 -> 24/32
    static void compose6( PixOpPtr& pop, const uint8 *s, uint32 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 16 -> 24/32
    static void compose7( PixOpPtr& pop, const uint16 *s, uint32 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    // 24/32 -> 24/32
    static void compose8( PixOpPtr& pop, const uint32 *s, uint32 *d )
    {
        // this does not check for any byte ordering. e.g. byte in long word can be MSB or LSB! - works for intel/arm! (little endian)
        *d = (uint8)pop->Compose( (uint32)(*s), (uint32)(*d) );
    }

    //    static copyPixFunc compose[] = {
//            (copyPixFunc)composeT<uint8,uint8>,     // 0: 8  -> 8
//            (copyPixFunc)composeT<uint16,uint8>,    // 1: 16 -> 8
//            (copyPixFunc)composeT<uint32,uint8>,    // 2: 24/32 -> 8
//            (copyPixFunc)composeT<uint8,uint16>,    // 3: 8  -> 16
//            (copyPixFunc)composeT<uint16,uint16>,   // 4: 16 -> 16
//            (copyPixFunc)composeT<uint32,uint16>,   // 5: 24/32 -> 16
//            (copyPixFunc)composeT<uint8, uint32>,   // 6: 8 -> 32
//            (copyPixFunc)composeT<uint16,uint32>,   // 7: 16 -> 32
//            (copyPixFunc)composeT<uint32,uint32>,   // 8: 32/24 -> 32/24
//    };

    void Blitter::Scale( const SurfacePtr& src, SurfacePtr& dst,
                         sint32 x_pos, sint32 y_pos,
                         uint32 scale_width, uint32 scale_height,
                         PixOpPtr pixOp,
                         bool keep_aspect )
    {
        if ( src && dst ) {
            double scale_x = (double)scale_width / (double)src->GetWidth();
            double scale_y = keep_aspect ? scale_x : ((double)scale_height / (double)src->GetHeight());

            // render bounds is withing the destination bitmap
            Rectangle render_bounds( 0, 0, dst->GetWidth(), dst->GetHeight() );
            // that's the rect we want to blit into
            Rectangle dst_bounds( x_pos, y_pos, scale_width, src->GetHeight()*scale_y);

            // clip render bounds
            if ( pei::intersectRect( render_bounds, dst_bounds, dst_bounds ) )
            {
                if ( dst_bounds.w() == 0 || dst_bounds.h() == 0 )
                {
                    // off screen!
                    return;
                }
                //             // adjust src rectangle if clipped
                //             if ( px < 0 ) src_bounds.x = src_bounds.w - dst_bounds.w;
                //             if ( py < 0 ) src_bounds.y = src_bounds.h - dst_bounds.h;
                //             src_bounds.w = dst_bounds.w;
                //             src_bounds.h = dst_bounds.h;
            }
            Rectangle src_bounds( 0, 0, src->GetWidth(), src->GetHeight() );

            uint32 spitch = src->GetPitch();
            uint32 dpitch = dst->GetPitch();
            uint32 sbpp   = src->GetBytesPerPixel();
            uint32 dbpp   = dst->GetBytesPerPixel();

            src->Lock();
            dst->Lock();

            int minx(MIN_X(src_bounds));
            int maxx(MAX_X(src_bounds));
            int miny(MIN_Y(src_bounds));
            int maxy(MAX_Y(src_bounds));

            copyPixFunc composeFunc;
            uint16 tp = sbpp << 8 | dbpp;
            switch ( tp )
            {
            // dest = 8 bit luma
            case 0x0101: composeFunc = (copyPixFunc)compose0; break; // compose[0]; break;
            case 0x0201: composeFunc = (copyPixFunc)compose1; break;
            case 0x0301: composeFunc = (copyPixFunc)compose2; break; // ignore one src byte
            case 0x0401: composeFunc = (copyPixFunc)compose2; break;
            // dest = 16 bit true color
            case 0x0102: composeFunc = (copyPixFunc)compose3; break;
            case 0x0202: composeFunc = (copyPixFunc)compose4; break;
            case 0x0302: composeFunc = (copyPixFunc)compose5; break; // ignore one src byte
            case 0x0402: composeFunc = (copyPixFunc)compose5; break;
            // dest = 3 byte RGB
            case 0x0103: composeFunc = (copyPixFunc)compose6; break;
            case 0x0203: composeFunc = (copyPixFunc)compose7; break;
            case 0x0303: composeFunc = (copyPixFunc)compose8; break; // skip one byte
            case 0x0403: composeFunc = (copyPixFunc)compose8; break;
            // dest = 4 byte RGBA (same as RGB)
            case 0x0104: composeFunc = (copyPixFunc)compose6; break;
            case 0x0204: composeFunc = (copyPixFunc)compose7; break;
            case 0x0304: composeFunc = (copyPixFunc)compose8; break; // skip one src byte
            default:
            case 0x0404: composeFunc = (copyPixFunc)compose8; break;
            }
            // TODO: Add filters, optimize...etc
            // Note always reads 32 bit pixels, even for < 32bpp formats...but corrects pixel addresses..cannot scale < 8 bpp
            if ( src->GetDepth() < 8 || dst->GetDepth() < 8 ) {

                // Simple up scale. Adds some inner loops for pixel multiplier. No filter is applied.
                int ys = MIN_Y(src_bounds);
                double yd = (double)MIN_Y(dst_bounds);
                for ( ; ys <= MAX_Y(src_bounds) && (int)yd <= MAX_Y(dst_bounds); ys++, yd += scale_y ) {
                    double dy(0);
                    do {
                        int xs = MIN_X(src_bounds);
                        double xd = (double)MIN_X(dst_bounds);
                        unsigned char* pSrcData = ((unsigned char*)src->GetPixels() + (ys * spitch ) + xs * sbpp);
                        for ( ; xs <= MAX_X(src_bounds) && (int)xd <= MAX_X(dst_bounds); xs++, xd += scale_x )
                        {
                            double dx(0);
                            do {
                                unsigned char* pDstData = ((unsigned char*)dst->GetPixels() + ((int)(yd+dy) * dpitch ) + (int)(xd+dx) * dbpp);
                                composeFunc( pixOp, pSrcData, pDstData );
                            } while ( ++dx < scale_x );
                            pSrcData += sbpp;
                        }
                    } while ( ++dy < scale_y );
                }


            } else {
                if ( scale_y <= 1.0 && scale_x <= 1.0 ) {
                    double ix(1.0/scale_x);
                    double iy(1.0/scale_y);
                    // pure down scaling. No pixels are multiplied, skip source pixels
                    double ys = MIN_Y(src_bounds);
                    double yd = (double)MIN_Y(dst_bounds);
                    for ( ; ys <= MAX_Y(src_bounds) && (int)yd <= MAX_Y(dst_bounds); ys++, yd+=scale_y ) {
                        double xs = MIN_X(src_bounds);
                        double xd = (double)MIN_X(dst_bounds);
                        unsigned char* pSrcData = ((unsigned char*)src->GetPixels() + (int)(ys * spitch ) + (int)xs*sbpp);
                        for ( ; xs <= MAX_X(src_bounds) && (int)xd <= MAX_X(dst_bounds); xs++, xd+=scale_x )
                        {
                            unsigned char* pDstData = ((unsigned char*)dst->GetPixels() + ((int)yd * dpitch ) + (int)xd * dbpp);
                            composeFunc( pixOp, pSrcData, pDstData );
                            pSrcData += sbpp;
                        }
                    }
                } else {
                    // Simple up scale. Adds some inner loops for pixel multiplier. No filter is applied.
                    int ys = MIN_Y(src_bounds);
                    double yd = (double)MIN_Y(dst_bounds);
                    for ( ; ys <= MAX_Y(src_bounds) && (int)yd <= MAX_Y(dst_bounds); ys++, yd += scale_y ) {
                        double dy(0);
                        do {
                            int xs = MIN_X(src_bounds);
                            double xd = (double)MIN_X(dst_bounds);
                            unsigned char* pSrcData = ((unsigned char*)src->GetPixels() + (ys * spitch ) + xs * sbpp);
                            for ( ; xs <= MAX_X(src_bounds) && (int)xd <= MAX_X(dst_bounds); xs++, xd += scale_x )
                            {
                                double dx(0);
                                do {
                                    unsigned char* pDstData = ((unsigned char*)dst->GetPixels() + ((int)(yd+dy) * dpitch ) + (int)(xd+dx) * dbpp);
                                    composeFunc( pixOp, pSrcData, pDstData );
                                } while ( ++dx < scale_x );
                                pSrcData += sbpp;
                            }
                        } while ( ++dy < scale_y );
                    }
                }
            }
            dst->Unlock();
            src->Unlock();
        }
    }

    pei::SurfacePtr Blitter::Dither( pei::SurfacePtr& dest )
    {
        /* Matrix: 4x4
         *          |1   9  3 11|
         *   1      |13  5 15  7|
         *  ---  *  |4  12  2 10|
         *   17     |16  8 14  6|
         *
         *   8x8: ...
         */

        // http://en.wikipedia.org/wiki/Ordered_dithering, 4x4 matrix - use 8x8 some day...
        // all consts, compiler should resolve these - TODO: make static int table
        static const float md[] =
        {
            1.0f/17.0f*255.0f,   9.0f/17.0f*255.0f,  3.0f/17.0f*255.0f, 11.0f/17.0f*255.0f,
           13.0f/17.0f*255.0f,   5.0f/17.0f*255.0f, 15.0f/17.0f*255.0f,  7.0f/17.0f*255.0f,
            4.0f/17.0f*255.0f,  12.0f/17.0f*255.0f,  2.0f/17.0f*255.0f, 10.0f/17.0f*255.0f,
           16.0f/17.0f*255.0f,   8.0f/17.0f*255.0f, 14.0f/17.0f*255.0f,  6.0f/17.0f*255.0f,
        };

        uint32 pitch = dest->GetPitch();
//         uint32 bpp   = dest->GetBytesPerPixel();

        dest->Lock();
        if ( dest->GetDepth() == 32 ) {
            uint32 aMask,  rMask,  gMask,  bMask;
            uint32 aShift, rShift, gShift, bShift;
            uint32 aLoss,  rLoss,  gLoss,  bLoss;

            const pei::Format &sf = dest->GetFormat();
            aMask  = sf.m_AMask; rMask  = sf.m_RMask; gMask  = sf.m_GMask; bMask  = sf.m_BMask;
            aShift = sf.m_AShift;rShift = sf.m_RShift;gShift = sf.m_GShift;bShift = sf.m_BShift;
            aLoss  = sf.m_ALoss; rLoss  = sf.m_RLoss; gLoss  = sf.m_GLoss; bLoss  = sf.m_BLoss;

            // render loop
            for ( unsigned int y = 0; y < dest->GetHeight(); y++ ) {

                uint32 *pPix = (uint32*)((char*)dest->GetPixels() + (y * pitch ));
                for ( unsigned int x = 0; x < dest->GetWidth(); x++ )
                {
                    uint32 sp = *pPix;
                    uint32 sa = ((sa & aMask) >> aShift) << aLoss;
                    if ( sa != pei::Color::PIXF_TRANPARENT ) {
                        // split pix img 1 to rgb components
                        float sr = (float)(((sp & rMask) >> rShift) << rLoss);
                        float sg = (float)(((sp & gMask) >> gShift) << gLoss);
                        float sb = (float)(((sp & bMask) >> bShift) << bLoss);

                        // extract luma
                        uint32 Y = (uint32)(0.2126f * sr + 0.7152f * sg + 0.0722f * sb);

                        uint32 hue = (uint32)(md[(x%4) + (y%4*4)]);
                        uint32 d = ( Y > hue ) ? 255 : 0;

                        // preserve alpha if we have one??
                        uint32 dp = ( (((d >> rLoss ) << rShift ) & rMask ) |
                                      (((d >> gLoss ) << gShift ) & gMask ) |
                                      (((d >> bLoss ) << bShift ) & bMask ) |
                                      aMask // always opaque
                                    );

                        *pPix++ = dp;
                    } else {
                        // fill white
                        *pPix++ = (rMask | gMask | bMask | aMask );
                    }
                }
            }
        }
        else if ( dest->GetDepth() == 8 ) {
            // render loop
            for ( unsigned int y = 0; y < dest->GetHeight(); y++ ) {
                uint8 *pPix = (uint8*)dest->GetPixels() + (y*pitch);
                for ( unsigned int x = 0; x < dest->GetWidth(); x++ )
                {
                    uint32 sp  = (uint32)*pPix;
                    uint32 hue = (uint32)(md[(x%4) + (y%4*4)]);
                    *pPix++ = sp > hue ? 255 : 0;
                }
            }
        } else {
            // not supported < 8 bit depth. Do not dither
        }
        dest->Unlock();

        return dest;
    }

} /* namespace sdl */
