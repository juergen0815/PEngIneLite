/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "render_surface.h"

#include <render/rectangle.h>

#include <string.h> // memset

namespace pei
{

Format::Format(
    double width, double height, int pitch, int depth, int bpp, enTYPE type,
    unsigned int rmask, unsigned int gmask, unsigned int bmask, unsigned int amask,
    unsigned int rshift, unsigned int gshift, unsigned int bshift, unsigned int ashift,
    unsigned int rloss,  unsigned int gloss, unsigned int bloss, unsigned int aloss,
    double frame_time, double ox, double oy,
    double scale_x, double scale_y,
    double cw, double ch )
: m_Type( type )

, m_Width(width)
, m_Height(height)
, m_Pitch(pitch)
, m_Depth(depth)
, m_BytesPerPixel(bpp)

, m_RMask (rmask),  m_GMask (gmask),  m_BMask (bmask),  m_AMask (amask)
, m_RShift(rshift), m_GShift(gshift), m_BShift(bshift), m_AShift(ashift)
, m_RLoss (rloss),  m_GLoss (gloss),  m_BLoss (bloss),  m_ALoss (aloss)

, m_XOffset(ox)
, m_YOffset(oy)

, m_CanvasWidth ( (cw < 0.1) ? width  : cw )
, m_CanvasHeight( (ch < 0.1) ? height : ch )

, m_XScale(scale_x)
, m_YScale(scale_y)

, m_FrameTime(frame_time)
, m_StreamPosition(0.0)
{
    // make sure we have a pitch if we have a width
    if ( m_Pitch == 0 && m_Width > 0 ) {
        m_Pitch = (unsigned int)m_Width * m_BytesPerPixel;
    }
}

Format::Format( const Format& o )
: m_Type( o.m_Type )

, m_Width( o.m_Width)
, m_Height(o.m_Height)
, m_Pitch( o.m_Pitch)
, m_Depth( o.m_Depth)
, m_BytesPerPixel( o.m_BytesPerPixel )

, m_RMask (o.m_RMask),  m_GMask (o.m_GMask),  m_BMask (o.m_BMask),  m_AMask (o.m_AMask)
, m_RShift(o.m_RShift), m_GShift(o.m_GShift), m_BShift(o.m_BShift), m_AShift(o.m_AShift)
, m_RLoss (o.m_RLoss),  m_GLoss (o.m_GLoss),  m_BLoss (o.m_BLoss),  m_ALoss (o.m_ALoss)

, m_XOffset(o.m_XOffset)
, m_YOffset(o.m_YOffset)

, m_CanvasWidth(o.m_CanvasWidth)
, m_CanvasHeight(o.m_CanvasHeight)

, m_XScale(o.m_XScale)
, m_YScale(o.m_YScale)

, m_FrameTime(o.m_FrameTime)
, m_StreamPosition(o.m_StreamPosition)
{
    if ( m_Type >= TYPE_AUDIO ) {
        m_NumChannels = o.m_NumChannels;
        m_NumSamples  = o.m_NumSamples;
        m_SampleBits  = o.m_SampleBits;
        m_SampleRate  = o.m_SampleRate;
        m_NumBuffers  = o.m_NumBuffers;
        m_BlockSize   = o.m_BlockSize;
        m_Streaming   = o.m_Streaming;
        m_Volume      = o.m_Volume;
        if ( m_Type == TYPE_AUDIO ) {
            m_Type = (m_NumChannels >= 2) ? TYPE_STEREO : TYPE_MONO;
        }
    }
}

Format::Format( enTYPE type, unsigned int channels /* = 0 */, unsigned int numSamples /* = 0 */,
                unsigned int sampleBits /* = 0 */, unsigned int sampleRate /* = 0 */,
                int numBuffers /*= 1*/, int blockSize /*= -1*/,
                bool streaming /*= false */,
                double volume /* = 1.0 */)
    : m_Type( type )
    , m_NumChannels(channels)
    , m_NumSamples(numSamples)
    , m_SampleBits(sampleBits)
    , m_SampleRate(sampleRate)
    , m_NumBuffers(numBuffers)
    , m_BlockSize(blockSize)
    , m_Streaming(false)
    , m_Volume(volume)
    , m_NumberOfLoops(-1)
{
    if ( type == TYPE_AUDIO ) {
        m_Type = (channels >= 2) ? TYPE_STEREO : TYPE_MONO;
    }
}

Surface::Surface() : m_Pixels(NULL)
{
}

Surface::Surface( const Format& format )
{
	m_Format = format;
	if ( m_Format.m_CanvasWidth  < (m_Format.m_Width+m_Format.m_XOffset)  ) m_Format.m_CanvasWidth = m_Format.m_Width+m_Format.m_XOffset;
    if ( m_Format.m_CanvasHeight < (m_Format.m_Height+m_Format.m_YOffset) ) m_Format.m_CanvasHeight = m_Format.m_Height+m_Format.m_YOffset;
    if ( m_Format.m_Pitch == 0  ) { m_Format.m_Pitch = (unsigned int)m_Format.m_CanvasWidth * m_Format.m_BytesPerPixel; }
    unsigned int numPixels(m_Format.m_Pitch*m_Format.m_Height);
	m_Pixels = (void*)new char[ numPixels ];
	memset( m_Pixels, 0, numPixels );
}

Surface::Surface( size_t w, size_t h, size_t depth /*= 32*/ )
{
	m_Format.m_Width  = m_Format.m_CanvasWidth  = w;
	m_Format.m_Height = m_Format.m_CanvasHeight = h;
	m_Format.m_Depth  = depth;
	m_Format.m_BytesPerPixel = depth >> 3;
	m_Format.m_Pitch = m_Format.m_CanvasWidth*m_Format.m_BytesPerPixel;
	unsigned int numPixels(m_Format.m_Pitch*m_Format.m_Height);
	m_Pixels = (void*)new char[ numPixels ];
	memset( m_Pixels, 0, numPixels );
}

Surface::~Surface()
{
	if ( m_Pixels ) {
		delete [] (char*)m_Pixels;
		m_Pixels = NULL;
	}
}

SurfaceRef::SurfaceRef( SurfacePtr ref )
{
	// keep ref to make sure other surface doesn't go out of scope
	m_SurfaceRef = ref;
	// hard link pixels and palette
	m_Pixels  = ref->GetPixels();
	m_Palette = ref->GetPalette();
	// copy format
	m_Format  = ref->GetFormat();
}

SurfaceRef::SurfaceRef( SurfacePtr ref, const Rectangle& bounds )
{
	m_SurfaceRef = ref;
	m_Pixels  = ref->GetPixels();
	m_Palette = ref->GetPalette();
	m_Format  = ref->GetFormat();
	// ref is a canvas to current surface. We are much smaller than original surface (e.g. stencil in atlas)...
	m_Format.m_XOffset = bounds.x();
	m_Format.m_YOffset = bounds.y();
	m_Format.m_Width   = bounds.w();
	m_Format.m_Height  = bounds.h();
    m_Format.m_CanvasWidth = ref->GetWidth();
    m_Format.m_CanvasHeight= ref->GetHeight();
}

SurfaceRef::SurfaceRef( const Brush& brush )
{
    m_Pixels = (void*)brush.m_Pixels;
    m_Format.m_Width  = m_Format.m_CanvasWidth  = brush.m_Width;
    m_Format.m_Height = m_Format.m_CanvasHeight = brush.m_Height;
    m_Format.m_BytesPerPixel = brush.m_BytesPerPixel;
    m_Format.m_Depth = brush.m_BytesPerPixel * 8;
    m_Format.m_Pitch = brush.m_Width * brush.m_BytesPerPixel;
    m_Format.m_RMask = 0x000000ff; m_Format.m_RShift = 0;
    m_Format.m_GMask = 0x0000ff00; m_Format.m_GShift = 8;
    m_Format.m_BMask = 0x00ff0000; m_Format.m_BShift = 16;
    if ( m_Format.m_Depth == 24 ) {
        m_Format.m_AMask = 0x00000000;
        m_Format.m_ALoss = 24;
    } else if ( m_Format.m_Depth == 32 ){
        m_Format.m_AMask  = 0xff000000;
        m_Format.m_AShift = 24;
    }
}


SurfaceRef::~SurfaceRef()
{
	// do not (and don't let super class) delete pixels!
	m_Pixels = NULL;
}

Brush::operator pei::SurfacePtr () {
    return pei::SurfacePtr( new SurfaceRef(*this) );
}

#if 0
// can create surface from brush but frees pixel data! Dangerous! Do not use with static pixel data!
SurfaceWrapper::SurfaceWrapper( const Brush& brush )
{
    m_Pixels = (void*)brush.m_Pixels;
    m_Format.m_width  = brush.m_Width;
    m_Format.m_height = brush.m_Height;
    m_Format.m_bytes_per_pixel = brush.m_BytesPerPixel;
    m_Format.m_depth = brush.m_BytesPerPixel * 8;
    m_Format.m_pitch = brush.m_Width * brush.m_BytesPerPixel;
    m_Format.m_rmask = 0x000000ff; m_Format.m_rshift = 0;
    m_Format.m_gmask = 0x0000ff00; m_Format.m_gshift = 8;
    m_Format.m_bmask = 0x00ff0000; m_Format.m_bshift = 16;
    if ( m_Format.m_depth == 24 ) {
        m_Format.m_amask = 0x00000000;
        m_Format.m_aloss = 24;
    } else if ( m_Format.m_depth == 32 ){
        m_Format.m_amask  = 0xff000000;
        m_Format.m_ashift = 24;
    }
}
#endif

} /* namespace pei */
