/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <render/rectangle.h>

#include <boost/shared_ptr.hpp>
namespace pei
{
    class Rectangle;

    class Surface;
    typedef boost::shared_ptr<Surface> SurfacePtr;

    class Format;
    typedef boost::shared_ptr<Format> FormatPtr;

    class Palette;
    typedef boost::shared_ptr<Palette> PalettePtr;


    class Format
    {
    public:
        enum enTYPE
        {
            // unknown type (e.g. compressed, etc)
            TYPE_UNKNOWN,

            // palletized color pixel format uses palette
            TYPE_INDEXED,
            // all direct mapped pixel formats (RGB1, RGB565, RGB888, RGBA8888, etc)
            TYPE_DIRECT_COLOR,
            // RLE compressed direct color bitmap - not supported ATM
            TYPE_RLE_DIRECT_COLOR, // not used
            // YUV encoded (overlay)
            TYPE_YUV, // not used
            // YUV encoded (overlay) with additional alpha
            TYPE_YUVA, // not used
            // Grey scale, 1 byte
            TYPE_LUMINANCE,
            // Grey scale with alpha, 2 bytes
            TYPE_LUMINANCE_ALPHA,
            // Alpha mask, 1 byte
            TYPE_ALPHA,

            // 8 or 1 bit bitmap type - 1 bit cannot be used as texture!
            TYPE_BITMAP,

            // all audio types
            TYPE_AUDIO = 0x1000,
            // single stream 16 bit
            TYPE_MONO,
            // stereo 16 bit
            TYPE_STEREO
            // TODO: 5.1,7.1 AUDIO support

        };
        enTYPE m_Type;

        // a sound surface might encode a "virtual" bitmap to allow a visual blit of sound data - always default RGBA format. type remains TYPE_AUDIO!
        double m_Width;
        double m_Height;
        unsigned int m_Pitch;
        unsigned int m_Depth;
        unsigned int m_BytesPerPixel;
        union {
            // Video
            struct {
                unsigned int m_RMask,  m_GMask,  m_BMask,  m_AMask;
                unsigned int m_RShift, m_GShift, m_BShift, m_AShift;
                unsigned int m_RLoss,  m_GLoss,  m_BLoss,  m_ALoss;  // for pixelsizes < 32 bit

                /** bitmap size != canvas size. Offsets within pixel data to define bitmap */
                double m_XOffset;
                double m_YOffset;

                double m_CanvasWidth;
                double m_CanvasHeight;

                /** virtual size */
                double m_XScale;
                double m_YScale;
            };
            // Audio
            struct {
                unsigned int m_NumChannels;
                unsigned int m_NumSamples;
                unsigned int m_SampleBits;
                unsigned int m_SampleRate;
                int          m_NumBuffers;
                int          m_BlockSize;
                bool         m_Streaming;
                double       m_Volume;
                int          m_NumberOfLoops;
            };
        };
        /** a surface might have a frame time - time resolution. in ms */
        double m_FrameTime;
        double m_StreamPosition; // position in stream in ms - if any. 0 default
    public:
        Format(
            double width = 0.0, double height = 0.0,
            int pitch = 0,
            int depth = 32, int bpp = 4,
            enTYPE type = TYPE_DIRECT_COLOR,
            unsigned int rmask  = 0x000000ff, unsigned int gmask = 0x0000ff00, unsigned int bmask = 0x00ff0000, unsigned int amask = 0xff000000,
            unsigned int rshift = 0, unsigned int gshift = 8, unsigned int blue_shift = 16, unsigned int ashift = 24,
            unsigned int rloss  = 0,  unsigned int gloss  = 0, unsigned int bloss = 0, unsigned int aloss = 0,
            double frame_time = 0, double ox = 0, double oy = 0,
            double scale_x = 1.0, double scale_y = 1.0,
            double cw = 0.0, double ch = 0.0
            );

        Format( const Format& other );

        Format( enTYPE type, unsigned int channels = 0, unsigned int numSamples = 0,
                unsigned int sampleBits = 0, unsigned int sampleRate = 0,
                int numBuffers = 1, int blockSize = -1,
                bool streaming = false, double volume = 1.0 );

        inline pei::Format::enTYPE GetType() const          { return m_Type;                    }
        inline pei::Format& SetType(pei::Format::enTYPE t)  { m_Type = t; return *this;         }
        inline double GetWidth() const                      { return m_Width;                   }
        inline pei::Format& SetWidth( double w)             { m_Width = w; return *this;        }
        inline double GetHeight() const                     { return m_Height;                  }
        inline pei::Format& SetHeight( double h)            { m_Height = h; return *this;       }
        inline unsigned int GetPitch() const                { return m_Pitch;                   }
        inline pei::Format& SetPitch(unsigned int p)        { m_Pitch = p; return *this;        }
        inline unsigned int GetDepth() const                { return m_Depth;                   }
        inline pei::Format& SetDepth(unsigned int d)        { m_Depth = d; return *this;        }
        inline unsigned int GetBytesPerPixel() const        { return m_BytesPerPixel;           }
        inline pei::Format& SetBytesPerPixel(unsigned int bpp) { m_BytesPerPixel = bpp; return *this; }
        inline double GetXOffset() const                    { return m_XOffset;                 }
        inline pei::Format& SetXOffset(double x)            { m_XOffset = x; return *this;      }
        inline double GetYOffset() const                    { return m_YOffset;                 }
        inline pei::Format& SetYOffset(double y)            { m_YOffset = y; return *this;      }
        inline double GetCanvasWidth() const                { return m_CanvasWidth;             }
        inline pei::Format& SetCanvasWidth(double w)        { m_CanvasWidth = w; return *this;  }
        inline double GetCanvasHeight() const               { return m_CanvasHeight;            }
        inline pei::Format& SetCanvasHeight(double h)       { m_CanvasHeight = h; return *this; }
        inline double GetXScale() const                     { return m_XScale;                  }
        inline pei::Format& SetXScale(double x)             { m_XScale = x; return *this;       }
        inline double GetYScale() const                     { return m_YScale;                  }
        inline pei::Format& SetYScale(double y)             { m_YScale = y; return *this;       }

        inline double GetStreamPosition() const             { return m_StreamPosition;          }
        inline pei::Format& SetStreamPosition( double pos ) { m_StreamPosition = pos; return *this; }

        inline double GetFrameTime() const                  { return m_FrameTime;               }
        inline pei::Format& SetFrameTime(double t )         { m_FrameTime = t; return *this;    }

        inline unsigned int GetNumChannels() const          { return m_NumChannels;             }
        inline pei::Format& SetNumChannels( unsigned int n ){ m_NumChannels = n; return *this;  }
        inline unsigned int GetNumSamples() const           { return m_NumSamples;              }
        inline pei::Format& SetNumSamples( unsigned int n ) { m_NumSamples = n; return *this;   }
        inline unsigned int GetSampleBits() const           { return m_SampleBits;              }
        inline pei::Format& SetSampleBits(unsigned int n)   { m_SampleBits = n; return *this;   }
        inline unsigned int GetSampleRate() const           { return m_SampleRate;              }
        inline pei::Format& SetSampleRate(unsigned int n)   { m_SampleRate = n; return *this;   }
        inline int  GetNumBuffers() const                   { return m_NumBuffers;              }
        inline pei::Format& SetNumBuffers(int n)            { m_NumBuffers = n; return *this;   }
        inline int GetBlockSize() const                     { return m_BlockSize;               }
        inline pei::Format& SetBlockSize(int n)             { m_BlockSize = n; return *this;    }
        inline bool IsStreaming() const                     { return m_Streaming;               }
        inline pei::Format& SetStreaming(bool n)            { m_Streaming = n; return *this;    }
        inline double GetVolume() const                     { return m_Volume;                  }
        inline pei::Format& SetVolume(double n)             { m_Volume = n; return *this;       }
        inline int GetNumberOfLoops() const                 { return m_NumberOfLoops;           }
        inline pei::Format& SetNumberOfLoops(int  n)        { m_NumberOfLoops = n; return *this;}


    };


    // low level binary RGB/RGBA data. Pixel data are static usually linked with the application
    struct Brush {
        unsigned int   m_Width;
        unsigned int   m_Height;
        unsigned int   m_BytesPerPixel; /* 3:RGB, 4:RGBA */
        const char	  *m_Pixels;

        operator pei::SurfacePtr ();
    };

    class SurfaceBase
    {
    private:
        // no copy c'tor!
        SurfaceBase( const SurfaceBase& );
    public:
        /** frame/pixel format description */
        Format m_Format;

    protected:
        /** create empty default surface - cannot exist!*/
        SurfaceBase() {}

    public:
        virtual ~SurfaceBase() {}

        virtual const void* GetExtDescription() const { return "SurfaceBase"; }

        virtual const Format& GetFormat() const { return m_Format; }

        virtual Format& GetFormat() { return m_Format; }

        virtual void SetFormat( const Format& format ) { m_Format = format; }

        /*!
         * Lock a surface - can be used to validate the pixel data. Default GetPixels() returns 0;
         * Static surfaces (with static pixel data) would implement this empty, dynamic surfaces
         * will set cache some memory to allow access to pixel data until Unlock() is called.
         */
        virtual void Lock() const {}

        /*!
         * Unlock surface and release pixel data, if possible unless surface has static pixel data.
         */
        virtual void Unlock() const {}

        /*
         * Must be implemented by specialized class. Default surface has no pixel data.
         */
        virtual void* GetPixels() const = 0;

        // most format shortcuts
        inline unsigned int GetWidth() const { return m_Format.m_Width; }

        inline unsigned int GetHeight() const { return m_Format.m_Height; }

        inline int GetXOffset() const { return (int)m_Format.m_XOffset; }

        inline int GetYOffset() const { return (int)m_Format.m_YOffset; }

        inline unsigned int GetPitch() const { return m_Format.m_Pitch; }

        inline unsigned int GetBytesPerPixel() const { return m_Format.m_BytesPerPixel; }

        inline unsigned int GetDepth() const { return m_Format.m_Depth; }
    };

    // allocates pixel data - and frees them
    class Surface : public SurfaceBase
    {
    private:
        // no copy c'tor!
        Surface( const Surface& );
    protected:
        /** color look up table, if indexed, else NULL */
        PalettePtr        m_Palette;
        /** actual pixel data */
        mutable void     *m_Pixels;

    public:
        Surface();
        Surface( const pei::Format& format );
        Surface( size_t w, size_t h, size_t depth = 32 );
        virtual ~Surface();

        virtual const void* GetExtDescription() const { return "Surface"; }

        PalettePtr GetPalette() const { return m_Palette; }

        virtual void* GetPixels() const { return m_Pixels; }

        inline pei::Rectangle operator()( const pei::SurfacePtr& a) {
            return pei::Rectangle( a->GetXOffset(), a->GetYOffset(), a->GetWidth(), a->GetHeight() );
        }
    };


    inline bool operator<( const pei::SurfacePtr& a, const pei::SurfacePtr& b ) {
        return (a->GetWidth() < b->GetWidth()) || (a->GetHeight() < b->GetHeight() );
    }

    inline bool operator<( const pei::SurfacePtr& a, const pei::Rectangle& b ) {
        return (a->GetWidth() < b.GetWidth()) || (a->GetHeight() < b.GetHeight() );
    }

    inline bool operator>( const pei::SurfacePtr& a, const pei::SurfacePtr& b ) {
        return (a->GetWidth() > b->GetWidth()) || (a->GetHeight() > b->GetHeight() );
    }

    inline bool operator>( const pei::SurfacePtr& a, const pei::Rectangle& b ) {
        return (a->GetWidth() > b.GetWidth()) || (a->GetHeight() > b.GetHeight() );
    }

    // that might need some work - check canvas size and pos offsets...
    inline bool operator==( const pei::SurfacePtr& a, const pei::SurfacePtr& b ) {
        return (a->GetWidth() == b->GetWidth()) && (a->GetHeight() == b->GetHeight() );
    }

    inline bool operator==( const pei::SurfacePtr& a, const pei::Rectangle& b ) {
        return (a->GetWidth() == b.GetWidth()) || (a->GetHeight() == b.GetHeight() );
    }

    //! this class does not free pixels data! They might belong to someone else
    class SurfaceRef : public Surface
    {
    private:
        // no copy c'tor!
        SurfaceRef( const SurfaceRef& );

        // cannot be crated from format!
        SurfaceRef( const pei::Format& format );
        SurfaceRef( size_t w, size_t h, size_t depth = 32 );
    protected:
        // ref to surface owning pixels
        SurfacePtr m_SurfaceRef;

        SurfaceRef() {}
    public:
        SurfaceRef( SurfacePtr ref );
        // canvas within existing surface
        SurfaceRef( SurfacePtr ref, const Rectangle& bounds );

        SurfaceRef( const Brush& brush );

        virtual ~SurfaceRef();
    };

    // this one expects foreign pixel data and will free those
    #if 0
    class SurfaceWrapper : public Surface
    {
    private:
        // no copy c'tor!
        SurfaceWrapper( const SurfaceWrapper& );

        // cannot be crated from format!
        SurfaceWrapper( const pei::Format& format );
        SurfaceWrapper( size_t w, size_t h, size_t depth = 32 );
    protected:
        SurfaceWrapper( const Brush& brush );

        virtual ~SurfaceWrapper() {}
    };
    #endif

    // engine profiles a static linkage into its logo data
    extern const struct Brush LogoBrush;

} /* namespace pei */

#endif /* RENDER_SURFACE_H_ */
