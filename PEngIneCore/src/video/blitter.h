/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef BLITTER_H
#define BLITTER_H

#include <video/pix_op.h>
#include <video/color_op.h>

#include <render/rectangle.h>
#include <render/render_surface.h>

#include <algorithm>

#include <string.h>

namespace pei
{

    class Blitter
    {
        PixOpPtr m_PixOp;

        Blitter( const Blitter& other ) {}
    public:
        Blitter( const PixOpPtr& pixop ) : m_PixOp(pixop) {}

        virtual ~Blitter() { }

        void SetPixOp( const PixOpPtr& pixop )  { m_PixOp = pixop; }

        virtual void Blit( const SurfacePtr& src, const Rectangle &sr,
                                 SurfacePtr& dst, const Rectangle &dr )
        {
            Blit( src, sr, dst, dr, m_PixOp );
        }

        virtual void Blit( const pei::SurfacePtr& src, pei::SurfacePtr& dst,
                           sint32 x_pos, sint32 y_pos,
                           uint32 width, uint32 height,
                           sint32 sx = 0, sint32 sy = 0)
        {
            Rectangle s_rc( sx, sy, width, height );
            Rectangle d_rc( x_pos, y_pos, width, height );
            Blit( src, s_rc, dst, d_rc, m_PixOp );
        }

        static inline void Blit( const pei::SurfacePtr& src, pei::SurfacePtr& dst,
                           sint32 x_pos, sint32 y_pos,
                           uint32 width, uint32 height,
                           sint32 sx, sint32 sy,
                           PixOpPtr pixop )
        {
            Rectangle s_rc( sx, sy, width, height );
            Rectangle d_rc( x_pos, y_pos, width, height );
            Blit( src, s_rc, dst, d_rc, pixop );
        }

        static inline void Blit( const SurfacePtr& src, const Rectangle &sr, SurfacePtr& dst, const Rectangle &dr, PixOpPtr pixop )
        {
            Rectangle src_bounds(sr);
            Rectangle dst_bounds(dr);

            if ( (src_bounds.x() + src_bounds.w()) > src->GetWidth()  ) src_bounds.w() = src->GetWidth()  - sr.x();
            if ( (src_bounds.y() + src_bounds.h()) > src->GetHeight() ) src_bounds.h() = src->GetHeight() - sr.y();

            // render bounds is withing the destination bitmap
            Rectangle render_bounds( 0, 0, dst ? dst->GetWidth() : src->GetWidth(), dst ? dst->GetHeight() : src->GetHeight() );

            if ( ClipBounds( render_bounds, src_bounds, dst_bounds ) ) {
                return; // clipped everything away
            }

    //             // adjust dest bounds - can never be bigger than src bounds
    //             dst_bounds.w = src_bounds.w;
    //             dst_bounds.h = src_bounds.h;
    //             // remember dest position - will be overridden by clipper
    //             int px = dst_bounds.x;
    //             int py = dst_bounds.y;
    //
    //             // clip render bounds
    //             if ( pei::intersectRect( render_bounds, dst_bounds, dst_bounds ) )
    //             {
    //                 if ( dst_bounds.w == 0 || dst_bounds.h == 0 )
    //                 {
    //                     // off screen!
    //                     return;
    //                 }
    //                 // adjust src rectangle if clipped
    //                 if ( px < 0 ) src_bounds.x = src_bounds.w - dst_bounds.w;
    //                 if ( py < 0 ) src_bounds.y = src_bounds.h - dst_bounds.h;
    //                 src_bounds.w = dst_bounds.w;
    //                 src_bounds.h = dst_bounds.h;
    //             }

            uint32 spitch = src->GetPitch();
            uint32 dpitch = dst ? dst->GetPitch() : spitch;
            uint32 sbpp   = src->GetBytesPerPixel();
            uint32 dbpp   = dst ? dst->GetBytesPerPixel() : sbpp;

            src->Lock();
            if ( dst ) {
                dst->Lock();
            }

            const void *sp = src->GetPixels();
            void *dp = dst ? dst->GetPixels() : NULL;

            uint16 tp = sbpp << 8 | dbpp;
            switch ( tp )
            {
                // 8 bit blits need palette!!
            case 0x0101:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint8,uint8>( sp, sbpp, spitch, src_bounds,
                                            dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0102:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint8,uint16>( sp, sbpp, spitch, src_bounds,
                                             dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0104:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint8,uint32>( sp, sbpp, spitch, src_bounds,
                                             dp, dbpp, dpitch, dst_bounds, pixop );
                break;

            case 0x0202:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint16,uint16>( sp, sbpp, spitch, src_bounds,
                                              dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0204:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint16,uint32>( sp, sbpp, spitch, src_bounds,
                                              dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0402:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint32,uint16>( sp, sbpp, spitch, src_bounds,
                                              dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0404:
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint32,uint32>( sp, sbpp, spitch, src_bounds,
                                              dp, dbpp, dpitch, dst_bounds, pixop );
                break;

                // gray scale conversion
            case 0x0401: // only works with some DST format of LUMA
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlit<uint32,uint8>( sp, sbpp, spitch, src_bounds,
                                             dp, dbpp, dpitch, dst_bounds, pixop );
                break;

                // gray scale conversion
            case 0x0301: // only works with some DST format of LUMA
                // TODO: Check if src == dest and upper blit is required - and implement upper blit
                InnerDownBlitExact<uint32,uint8>( sp, sbpp, spitch, src_bounds,
                                                  dp, dbpp, dpitch, dst_bounds, pixop );
                break;

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Bit blits - I should probably make this an explicit implementation --- wrong pixop will certainly crash here
            case 0x0001:
                // 1 bit to 8 bit gray
                InnerBitBlit<uint8,uint8>( sp, sbpp, spitch, src_bounds,
                                           dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0002:
                // 1 bit to 16 bit true color
                InnerBitBlit<uint16,uint16>( sp, sbpp, spitch, src_bounds,
                                             dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            case 0x0004:
                // 1 bit to 32 bit true color
                InnerBitBlit<uint32,uint32>( sp, sbpp, spitch, src_bounds,
                                             dp, dbpp, dpitch, dst_bounds, pixop );
                break;
            default:
                break;
            }

            if (dst) dst->Unlock();
            src->Unlock();
        }

        static void Scale( const SurfacePtr& src,
                                 SurfacePtr& dst,
                           sint32 x_pos, sint32 y_pos,
                           uint32 scale_width, uint32 scale_height,
                           PixOpPtr pixOp,
                           bool   keep_aspect = false);

        // Short cut for direct scale into dest
        static inline void Scale( const SurfacePtr& src, SurfacePtr& dst, PixOpPtr pixOp )
        {
            Scale( src, dst, 0, 0, (uint32)dst->GetWidth(), (uint32)dst->GetHeight(), pixOp, false );
        }

        virtual void ScaleBlit( const SurfacePtr& src,
                                      SurfacePtr& dst,
                                sint32 x_pos, sint32 y_pos,
                                uint32 scale_width, uint32 scale_height,
                                bool   keep_aspect = false )
        {
            Scale( src, dst, x_pos, y_pos, scale_width, scale_height, m_PixOp, keep_aspect );
        }

        static inline void Clear( SurfacePtr& surface )
        {
            surface->Lock();
            void* pix = surface->GetPixels();
            if ( pix ) {
                memset( pix, 0, surface->GetPitch()*surface->GetFormat().GetCanvasHeight());
            }
            surface->Unlock();
        }

        /**
         * Dither a given surface.
         *
         * @param surface The surface to dither
         * @param grow expand the surface and create a new one (fixed size 4x4)
         * @param inplace dither existing surface
         */
        static pei::SurfacePtr Dither( pei::SurfacePtr& surface );

    protected:

        template<class S, class D>
        static inline void InnerDownBlit( const void* sp, int sbpp, int spitch, const Rectangle& sr,
                                          void* dp, int dbpp, int dpitch, const Rectangle& dr,
                                          PixOpPtr& pix_op )
        {
            if ( sp && dp ) {
                // render loop
                int ys = MIN_Y(sr);
                int yd = MIN_Y(dr);
                for ( ; ys <= MAX_Y(sr) && yd <= MAX_Y(dr); ys++, yd++ ) {
                    int xs = MIN_X(sr);
                    int xd = MIN_X(dr);

                    S *pSrcData = (S*)((char*)sp + (ys * spitch ) + xs * sbpp);
                    D *pDstData = (D*)((char*)dp + (yd * dpitch ) + xd * dbpp);
                    while ( xs++ <= MAX_X(sr) && xd++ <= MAX_X(dr) )
                    {
                        *pDstData = pix_op->Compose( *pSrcData, *pDstData );
                        pDstData++; pSrcData++;
                    }
                }
            } else if ( sp ) {
                // render loop
                int ys = MIN_Y(sr);
                for ( ; ys <= MAX_Y(sr); ys++ ) {
                    int xs = MIN_X(sr);

                    S *pSrcData = (S*)((char*)sp + (ys * spitch ) + xs * sbpp);
                    while ( xs++ <= MAX_X(sr)  )
                    {
                        S p = *pSrcData++;
                        pix_op->Compose( p, p );
                    }
                }
            }
        }

        template<class S, class D>
        static inline void InnerDownBlitExact( const void* sp, int sbpp, int spitch, const Rectangle& sr,
                                               void* dp, int dbpp, int dpitch, const Rectangle& dr,
                                               PixOpPtr& pix_op )
        {
            if ( sp && dp ) {
                // render loop
                int ys = MIN_Y(sr);
                int yd = MIN_Y(dr);
                for ( ; ys <= MAX_Y(sr) && yd <= MAX_Y(dr); ys++, yd++ ) {
                    int xs = MIN_X(sr);
                    int xd = MIN_X(dr);

                    S *pSrcData = (S*)((char*)sp + (ys * spitch ) + xs * sbpp);
                    D *pDstData = (D*)((char*)dp + (yd * dpitch ) + xd * dbpp);
                    while ( xs++ <= MAX_X(sr) && xd++ <= MAX_X(dr) )
                    {
                        *pDstData = pix_op->Compose( *pSrcData, *pDstData );
                        pSrcData = (S*)(((char*)pSrcData) + sbpp);
                        pDstData = (D*)(((char*)pDstData) + dbpp);
                    }
                }
            }
            else if ( sp ) {
                // render loop
                int ys = MIN_Y(sr);
                for ( ; ys <= MAX_Y(sr); ys++ ) {
                    int xs = MIN_X(sr);

                    S *pSrcData = (S*)((char*)sp + (ys * spitch ) + xs * sbpp);
                    while ( xs++ <= MAX_X(sr) )
                    {
                        S p = *pSrcData; pSrcData = (S*)(((char*)pSrcData) + sbpp);
                        pix_op->Compose( p, p );
                    }
                }
            }
        }

        template<class S, class D>
        static inline void InnerBitBlit( const void* sp, int sbpp, int spitch, const Rectangle& sr,
                                               void* dp, int dbpp, int dpitch, const Rectangle& dr,
                                               PixOpPtr& pix_op )
        {
            // must have source and destination
            if ( sp && dp ) {
                // render loop
                int fs(sizeof(D)*8);
                int ys = MIN_Y(sr);
                int yd = MIN_Y(dr);
                for ( ; ys <= MAX_Y(sr) && yd <= MAX_Y(dr); ys++, yd++ ) {
                    int xs = MIN_X(sr); // read long word from 1 bit bitmap
                    int xd = MIN_X(dr);

                    S *pSrcData = (S*)((char*)sp + (ys * spitch ) + xs / fs);
                    D *pDstData = (D*)((char*)dp + (yd * dpitch ) + xd * dbpp);

                    int w = std::min(MAX_X(sr)-MIN_X(sr)+1, MAX_X(dr)-MIN_X(dr)+1);
                    while ( w > 0 )
                    {
                        // read as many pixels as dest is wide
                        int col(fs);
                        S src = *pSrcData++;
                        int mask = w % fs; w -= fs;
                        if ( mask == 0 ) mask = fs;
                        switch ( mask ) {
                        // loop unrolling, all fall through - max 32 bit word at once
                        case 32: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 31: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 30: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 29: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 28: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 27: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 26: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 25: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 24: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 23: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 22: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 21: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 20: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 19: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 18: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 17: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 16: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 15: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 14: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 13: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 12: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 11: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case 10: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  9: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  8: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  7: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  6: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  5: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  4: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  3: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  2: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        case  1: *pDstData = pix_op->Compose( src & (1<<--col), *pDstData ); pDstData++;
                        default : break;
                        }
                    }
                }
            }
        }
    };

} /* namespace pei */
#endif /* PIX_OP_H */
