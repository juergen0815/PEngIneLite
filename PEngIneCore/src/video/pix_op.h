/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef PIX_OP_H
#define PIX_OP_H

#include <render/color.h>
#include <render/render_surface.h>

#include <map>
#include <vector>

namespace pei
{

    class PixOp;
    typedef boost::shared_ptr<PixOp> PixOpPtr;

    typedef unsigned int   uint32;
    typedef          int   sint32;
    typedef unsigned short uint16;
    typedef          short sint16;
    typedef unsigned char  uint8;
    typedef          char  sint8;

    static inline int makePow2(int x)
    {
        int val = 1;
        while(val < x) val <<= 1;
        return val;
    }

    // raster operators (per pixel ops - could be a shader, huh ?)
    class PixOp
    {
    public:
        PixOp() {}
        virtual ~PixOp() {}

        virtual uint32 Compose( const uint32 s, uint32 d ) = 0;
    };

    class PixOpVanillaCopy : public PixOp
    {
    public:
        PixOpVanillaCopy() {}
        PixOpVanillaCopy( const pei::Format &sf, const pei::Format &df ) {}

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            return sp;
        }
    };

    class PixOpCopy : public PixOp
    {
        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,   m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;

        uint32 m_Alpha;
    public:
        PixOpCopy( const pei::Format &sf, const pei::Format &df )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;

            m_Alpha =  pei::Color::PIXF_OPAQUE >> m_DstALoss << m_DstAShift & m_DstAMask;
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
            uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
            uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

            // copy src straight into dst
            dp = ( (((sr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                   (((sg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                   (((sb >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                   m_Alpha
                );
            return dp;
        }
    };


    class PixOpCopySrcAlpha : public PixOp
    {
        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,  m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;
    public:
        PixOpCopySrcAlpha( const pei::Format &sf, const pei::Format &df )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
            uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
            uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;
            uint32 sa = ((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss;
    //        uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : (((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss);

            // copy src straight into dst
            dp = ( (((sr >> m_DstRLoss) << m_DstRShift) & m_DstRMask ) |
                   (((sg >> m_DstGLoss) << m_DstGShift) & m_DstGMask ) |
                   (((sb >> m_DstBLoss) << m_DstBShift) & m_DstBMask ) |
                   (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask )
                );
            return dp;
        }
    };

    class PixOpAlphaBlit : public PixOp
    {
    protected:
        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,   m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;

        uint32 m_AlphaBlend;
    public:
        PixOpAlphaBlit( const pei::Format &sf, const pei::Format &df, double blend = 1.0 )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;

            m_AlphaBlend = ((uint32)((double)pei::Color::PIXF_OPAQUE * blend));
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            // extract alpha channel (pre-multiply with blend)
            uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : ((((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss) * m_AlphaBlend) >> 8;
            if ( sa == 0xff ) {
                // split pix img 1 to rgb components
                uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
                uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
                uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

                // copy src straight into dst
                dp = ( (((sr >> m_DstRLoss ) << m_DstRShift ) & m_DstRMask ) |
                       (((sg >> m_DstGLoss ) << m_DstGShift ) & m_DstGMask ) |
                       (((sb >> m_DstBLoss ) << m_DstBShift ) & m_DstBMask ) |
                       (((sa >> m_DstALoss ) << m_DstAShift ) & m_DstAMask )
                    );
            } else if ( sa == 0 ) {
            } else {
                // split pix img 1 to rgb components
                uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
                uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
                uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

                uint32 dr = ((dp & m_DstRMask) >> m_DstRShift) << m_DstRLoss;
                uint32 dg = ((dp & m_DstGMask) >> m_DstGShift) << m_DstGLoss;
                uint32 db = ((dp & m_DstBMask) >> m_DstBShift) << m_DstBLoss;

                // alpha src + dst
                dr = dr + (( sr - dr ) * sa >> 8);
                dg = dg + (( sg - dg ) * sa >> 8);
                db = db + (( sb - db ) * sa >> 8);

                // write back dst
                dp = ( (((dr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((dg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((db >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                     );
            }
            return dp;
        }
    };

    class PixOpColorBlend : public PixOp
    {
    protected:
        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,  m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;

        pei::Color m_BlendColor;
    public:
        PixOpColorBlend( const pei::Format &sf, const pei::Format &df, const pei::Color& blendColor )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;

            m_BlendColor = blendColor;
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            // extract alpha channel (pre-multiply with blend)
            uint32 sa = (m_SrcAMask == 0) ?
                        pei::Color::PIXF_OPAQUE :
                        ((((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss) * m_BlendColor.a ) >> 8;
            if ( sa == 0xff ) {
                // split pix img 1 to rgb components
                uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
                uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
                uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

                sr = ((sr * m_BlendColor.r) >> 8) & 0xff;
                sg = ((sg * m_BlendColor.g) >> 8) & 0xff;
                sb = ((sb * m_BlendColor.b) >> 8) & 0xff;

                // copy src straight into dst
                dp = ( (((sr >> m_DstRLoss ) << m_DstRShift ) & m_DstRMask ) |
                       (((sg >> m_DstGLoss ) << m_DstGShift ) & m_DstGMask ) |
                       (((sb >> m_DstBLoss ) << m_DstBShift ) & m_DstBMask ) |
                       (((sa >> m_DstALoss ) << m_DstAShift ) & m_DstAMask )
                    );
            } else if ( sa == 0 ) {
            } else {
                // split pix img 1 to rgb components
                uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
                uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
                uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

                sr = ((sr * m_BlendColor.r) >> 8) & 0xff;
                sg = ((sg * m_BlendColor.g) >> 8) & 0xff;
                sb = ((sb * m_BlendColor.b) >> 8) & 0xff;

                uint32 dr = ((dp & m_DstRMask) >> m_DstRShift) << m_DstRLoss;
                uint32 dg = ((dp & m_DstGMask) >> m_DstGShift) << m_DstGLoss;
                uint32 db = ((dp & m_DstBMask) >> m_DstBShift) << m_DstBLoss;

                // alpha src + dst
                dr = dr + (( sr - dr ) * sa >> 8);
                dg = dg + (( sg - dg ) * sa >> 8);
                db = db + (( sb - db ) * sa >> 8);

                // write back dst
                dp = ( (((dr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((dg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((db >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                     );
            }
            return dp;
        }
    };

    class PixOpAlphaBlitSrcAlpha : public PixOpAlphaBlit {
    public:
        PixOpAlphaBlitSrcAlpha( const pei::Format &sf, const pei::Format &df, double blend = 1.0 )
            : PixOpAlphaBlit(sf,df,blend)
        {
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            // extract alpha channel (pre-multiply with blend)
            uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : (((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss);
            if ( m_AlphaBlend != 0xff ) {
                sa = (sa * m_AlphaBlend) >> 8;
            }
            if ( sa == 0xff ) {
                // split pix img 1 to rgb components
                uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
                uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
                uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

                // copy src straight into dst
                dp = ( (((sr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((sg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((sb >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((pei::Color::PIXF_OPAQUE >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                    );
            } else if ( sa > 0 ) {
                // split pix img 1 to rgb components
                uint32 sr = ((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss;
                uint32 sg = ((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss;
                uint32 sb = ((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss;

                uint32 dr = ((dp & m_DstRMask) >> m_DstRShift) << m_DstRLoss;
                uint32 dg = ((dp & m_DstGMask) >> m_DstGShift) << m_DstGLoss;
                uint32 db = ((dp & m_DstBMask) >> m_DstBShift) << m_DstBLoss;

                // alpha src + dst
                dr = dr + (( sr - dr ) * sa >> 8);
                dg = dg + (( sg - dg ) * sa >> 8);
                db = db + (( sb - db ) * sa >> 8);

                // write back dst
                dp = ( (((dr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((dg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((db >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((pei::Color::PIXF_OPAQUE >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                    );
            }
            return dp;
        }
    };

    class PixOpAlphaBlitLuma : public PixOp
    {
        double m_Luma;

        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,   m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;

        uint32 m_Blend;
    public:
        PixOpAlphaBlitLuma( const pei::Format &sf, const pei::Format &df, double luma, double blend = 1.0 )
            : m_Luma( luma )
        {
            if ( m_Luma > 1.0 ) m_Luma = 1.0;
            else if ( m_Luma < 0.0 ) m_Luma = 0.0;

            m_Blend = (uint32)((double)pei::Color::PIXF_OPAQUE * blend );

            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            // extract alpha channel
            uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : ((((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss) * m_Blend) >> 8;
            if ( m_Blend != 0xff ) {
                sa = (sa * m_Blend) >> 8;
            }
            if ( sa == 0xff ) {
                // split pix img 1 to rgb components
                float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);

                // copy src straight into dst
                dp = ( ((((uint32)(sr*m_Luma) >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       ((((uint32)(sg*m_Luma) >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       ((((uint32)(sb*m_Luma) >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (m_DstAMask) // sa == 0xff!
                    );
            } else if ( sa == 0 ) {
                // transparent. Nothing to render
            } else {
                // split pix img 1 to rgb components
                float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);


                uint32 dr = ((dp & m_DstRMask) >> m_DstRShift) << m_DstRLoss;
                uint32 dg = ((dp & m_DstGMask) >> m_DstGShift) << m_DstGLoss;
                uint32 db = ((dp & m_DstBMask) >> m_DstBShift) << m_DstBLoss;

                // alpha src + dst
                dr = dr + (( (uint32)(sr*m_Luma) - dr ) * sa >> 8);
                dg = dg + (( (uint32)(sg*m_Luma) - dg ) * sa >> 8);
                db = db + (( (uint32)(sb*m_Luma) - db ) * sa >> 8);

                // write back dst
                dp = ( (((dr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((dg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((db >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                    );
            }
            return dp;
        }
    };

    class PixOpAlphaBlitGray : public PixOp
    {
        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,   m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;

        uint32 m_Blend;
        pei::Format::enTYPE m_DestColorType; // TODO: Do blit into 8 bit luma

    public:
        PixOpAlphaBlitGray( const pei::Format &sf, const pei::Format &df, double blend = 1.0 )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;

            m_Blend = (uint32)((double)pei::Color::PIXF_OPAQUE * blend);
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            // extract alpha channel
            uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : (((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss);
            if ( m_Blend != 0xff ) {
                sa = (sa * m_Blend) >> 8;
            }
            if ( sa == 0xff ) {
                // split pix img 1 to rgb components
                float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);

                // extract luma
                uint32 Y = (uint32)(0.2126 * sr + 0.7152 * sg + 0.0722 * sb);

                // copy src straight into dst
                dp = ( (((Y  >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((Y  >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((Y  >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                    );
            } else if ( sa != 0 ) {
                // split pix img 1 to rgb components
                float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);

                // extract luma
                uint32 Y = (uint32)(0.2126 * sr + 0.7152 * sg + 0.0722 * sb);

                uint32 dr = ((dp & m_DstRMask) >> m_DstRShift) << m_DstRLoss;
                uint32 dg = ((dp & m_DstGMask) >> m_DstGShift) << m_DstGLoss;
                uint32 db = ((dp & m_DstBMask) >> m_DstBShift) << m_DstBLoss;

                // alpha src + dst
                dr = dr + (( Y - dr ) * sa >> 8);
                dg = dg + (( Y - dg ) * sa >> 8);
                db = db + (( Y - db ) * sa >> 8);

                // write back dst
                dp = ( (((dr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                       (((dg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                       (((db >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                       (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                    );
            }
            return dp;
        }
    };

    // dest must be 8 bit luma!
    class PixOpConvertToGray : public PixOp
    {
        uint32 m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32 m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32 m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;

        uint8 m_AlphaThreshold;
    public:
        PixOpConvertToGray( const pei::Format &sf, uint8 alphaThreshold = pei::Color::PIXF_TRANPARENT )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;

            m_AlphaThreshold = alphaThreshold;
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : (((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss);
            if ( sa > m_AlphaThreshold ) {
                // split pix img 1 to rgb components
                float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);

                // extract luma
                return (uint8)(0.2126f * sr + 0.7152f * sg + 0.0722f * sb);
            }
            return pei::Color::PIXF_OPAQUE;
        }
    };

    // dest must be 8 bit luma!
    class PixOpConvertToMono : public PixOp
    {
        float   m_Threshold;
        uint8   m_White;
        uint8   m_Black;

        uint32  m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32  m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32  m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;
        int     m_SrcDepth;
    public:
        PixOpConvertToMono( const pei::Format &sf, double threshold = 0.5, uint8 white = 255, uint8 black = 0  )
            : m_Threshold(threshold * 255.0)
            , m_White(white)
            , m_Black(black)
            , m_SrcDepth( sf.GetDepth() )
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            if ( m_SrcDepth == 8 ) {
                dp = ( sp > m_Threshold ) ? m_White : m_Black;
            } else if ( m_SrcDepth > 8 ) {
                uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : (((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss);
                if ( sa > pei::Color::PIXF_TRANPARENT ) {
                    // split pix img 1 to rgb components
                    float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                    float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                    float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);

                    // extract luma
                    float Y = (0.2126f * sr + 0.7152f * sg + 0.0722f * sb);
                    dp = ( Y > m_Threshold ) ? m_White : m_Black;
                } else {
                    dp = m_White;
                }
            } else {
                // ??? < 8 bit depth...not supported
                dp = m_Black;
            }
            return dp;
        }
    };

    // dest must be 8 bit luma!
    class PixOpLumaHistogram : public PixOp
    {
        uint32  m_SrcAMask,  m_SrcRMask,  m_SrcGMask,  m_SrcBMask;
        uint32  m_SrcAShift, m_SrcRShift, m_SrcGShift, m_SrcBShift;
        uint32  m_SrcALoss,  m_SrcRLoss,  m_SrcGLoss,  m_SrcBLoss;
        int     m_SrcDepth;

        std::map< int, int > m_Histogram;
        uint32               m_Min, m_Max;
    public:
        PixOpLumaHistogram( const pei::Format &sf )
            : m_SrcDepth( sf.GetDepth() )
            , m_Min(255)
            , m_Max(0)
        {
            m_SrcAMask  = sf.m_AMask; m_SrcRMask  = sf.m_RMask; m_SrcGMask  = sf.m_GMask; m_SrcBMask  = sf.m_BMask;
            m_SrcAShift = sf.m_AShift;m_SrcRShift = sf.m_RShift;m_SrcGShift = sf.m_GShift;m_SrcBShift = sf.m_BShift;
            m_SrcALoss  = sf.m_ALoss; m_SrcRLoss  = sf.m_RLoss; m_SrcGLoss  = sf.m_GLoss; m_SrcBLoss  = sf.m_BLoss;
        }

        inline int GetNumColors() const {
            return m_Histogram.size();
        }

        std::vector< int > GetHistogram() const
        {
            std::vector< int > lumas;
            std::map< int, int >::const_iterator it = m_Histogram.begin();
            while ( it != m_Histogram.end() ) {
                lumas.push_back( it->second ); it++;
            }
            return lumas;
        }

        inline int GetMin() const { return m_Min; }

        inline int GetMax() const { return m_Max; }

        // crap
        inline int GetAvarage() const { return (m_Max - m_Min)/GetNumColors()*255; }

        // crap
        inline int GetWeightedAverage() const
        {
            int n(0);
            std::map< int, int >::const_iterator it = m_Histogram.begin();
            while ( it != m_Histogram.end() ) {
                n += it->first / it->second;
            }
            return n / GetNumColors();
        }

        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            if ( m_SrcDepth == 8 ) {
                if ( sp < m_Min ) m_Min = sp;
                if ( sp > m_Max ) m_Max = sp;
                int l = m_Histogram[ (uint8)sp ] + 1;
                m_Histogram[ (uint8)sp ] = l;
            } else if ( m_SrcDepth > 8 ) {
                uint32 sa = (m_SrcAMask == 0) ? pei::Color::PIXF_OPAQUE : (((sp & m_SrcAMask) >> m_SrcAShift) << m_SrcALoss);
                if ( sa > pei::Color::PIXF_TRANPARENT ) {
                    // split pix img 1 to rgb components
                    float sr = (float)(((sp & m_SrcRMask) >> m_SrcRShift) << m_SrcRLoss);
                    float sg = (float)(((sp & m_SrcGMask) >> m_SrcGShift) << m_SrcGLoss);
                    float sb = (float)(((sp & m_SrcBMask) >> m_SrcBShift) << m_SrcBLoss);

                    // extract luma
                    uint8 Y = (uint8) (0.2126f * sr + 0.7152f * sg + 0.0722f * sb);
                    if ( Y < m_Min ) m_Min = Y;
                    if ( Y > m_Max ) m_Max = Y;
                    int l = m_Histogram[ Y ] + 1;
                    m_Histogram[ Y ] = l;
                }
            }
            return sp;
        }
    };

    // expand 1 bit bitmap into 8 bit luma or 16/32 bit true color
    class PixOpExpandBitmap : public PixOp
    {
        uint8  m_Black;
        uint8  m_White;
        uint32 m_Blend;

        uint32 m_DstAMask,  m_DstRMask,  m_DstGMask,   m_DstBMask;
        uint32 m_DstAShift, m_DstRShift, m_DstGShift, m_DstBShift;
        uint32 m_DstALoss,  m_DstRLoss,  m_DstGLoss,  m_DstBLoss;

        pei::Format::enTYPE m_ColorType;
        int                 m_SrcDepth;
    public:
        PixOpExpandBitmap( const pei::Format &df, uint8 black = 0, uint8 white = 255, double blend = 1.0 )
            : m_Black(black), m_White(white)
            , m_Blend((uint32)((double)pei::Color::PIXF_OPAQUE * blend))
        {
            m_DstAMask  = df.m_AMask; m_DstRMask  = df.m_RMask; m_DstGMask  = df.m_GMask; m_DstBMask  = df.m_BMask;
            m_DstAShift = df.m_AShift;m_DstRShift = df.m_RShift;m_DstGShift = df.m_GShift;m_DstBShift = df.m_BShift;
            m_DstALoss  = df.m_ALoss; m_DstRLoss  = df.m_RLoss; m_DstGLoss  = df.m_GLoss; m_DstBLoss  = df.m_BLoss;
            m_ColorType = df.GetType();
        }

        // we do not mask sp. Is either 0 or not -- will not work in big endian mode??
        virtual uint32 Compose( const uint32 sp, uint32 dp )
        {
            static const uint32 white = m_White;
            static const uint32 black = m_Black;
            uint32 c = (sp > 0) ? white : black;
            if ( m_ColorType == pei::Format::TYPE_LUMINANCE ||
                 m_ColorType == pei::Format::TYPE_BITMAP ) {
                // 8 bit luminance bitmap - no alpha
                dp = c;
            } else if ( m_ColorType == pei::Format::TYPE_DIRECT_COLOR ){

                // extract alpha channel
                uint32 sa(255.0);
                if ( m_Blend != 0xff ) {
                    sa = (sa * m_Blend) >> 8;
                }
                if ( sa == 0xff ) {
                    // split pix img 1 to rgb components

                    uint32 c = (sp > 0) ? white : black;
                    // copy src straight into dst
                    dp = ( (((c >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                           (((c >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                           (((c >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                           (((0xff >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                        );
                } else if ( sa != 0 ) {

                    uint32 dr = ((dp & m_DstRMask) >> m_DstRShift) << m_DstRLoss;
                    uint32 dg = ((dp & m_DstGMask) >> m_DstGShift) << m_DstGLoss;
                    uint32 db = ((dp & m_DstBMask) >> m_DstBShift) << m_DstBLoss;

                    // alpha src + dst
                    dr = dr + (( c - dr ) * sa >> 8);
                    dg = dg + (( c - dg ) * sa >> 8);
                    db = db + (( c - db ) * sa >> 8);

                    // write back dst
                    dp = ( (((dr >> m_DstRLoss) << m_DstRShift) & m_DstRMask) |
                           (((dg >> m_DstGLoss) << m_DstGShift) & m_DstGMask) |
                           (((db >> m_DstBLoss) << m_DstBShift) & m_DstBMask) |
                           (((sa >> m_DstALoss) << m_DstAShift) & m_DstAMask)
                        );
                } else {
                    dp = 0;
                }
            }
            return dp;
        }
    };

} /* namespace sdl */

#endif /* PIX_OP_H */
