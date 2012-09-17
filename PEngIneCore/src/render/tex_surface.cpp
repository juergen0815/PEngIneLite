/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "tex_surface.h"

namespace pei
{

#ifdef _HAS_OPENGL_
    static const GLuint texTypes[] = { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D };

    // TODO: This must move into the pei::Format as "TILE-MODE"
    const int TexSurface::REPEAT        = GL_REPEAT;
    const int TexSurface::CLAMP         = GL_CLAMP;
    const int TexSurface::CLAMP_EDGE    = GL_CLAMP_TO_EDGE;
    const int TexSurface::CLAMP_BORDER  = GL_CLAMP_TO_BORDER;
#else
    static const GLuint texTypes[] = { GL_TEXTURE_2D };

    const int TexSurface::REPEAT        = GL_REPEAT;
    const int TexSurface::CLAMP_EDGE    = GL_CLAMP_TO_EDGE;
#endif

	TexSurface::TexSurface( enTexType type )
		: m_TexType(type)
//		, m_WrapMode(GL_CLAMP_TO_EDGE)
	    , m_WrapMode(GL_REPEAT)
		, m_Filter(GL_LINEAR)
	{
		glGenTextures(1, (GLuint*)&m_TexId );
	}

	TexSurface::~TexSurface()
	{
		if ( m_Pixels ) {
			delete [] (char*)m_Pixels;
			m_Pixels = NULL;
		}
	}

    void TexSurface::Bind() {
        m_IsLoaded = true;
    }

	void TexSurface::Enable() {
	    Bind();
	}

	void TexSurface::Disable() {
		glDisable( texTypes[m_TexType] );
	}

    void TexSurface::Lock() const
    {
    	if ( !m_Pixels) {
    		// m_Pixels = new char[ 0 ]; // texture size!
    		// TODO: copy pixels from texture
    	}
    }

    void TexSurface::Unlock() const
    {
    	if ( m_Pixels ) {
    		//TODO: Update texture from pixels
    		delete [] (char*)m_Pixels;
    	}
    	m_Pixels = NULL;
    }

#ifdef _HAS_OPENGL_
    Texture1DSurface::Texture1DSurface( )
    	: TexSurface( EN_TEX_1D )
	{
	}

    Texture1DSurface::~Texture1DSurface()
    {
    }
#endif

    Texture2DSurface::Texture2DSurface(  )
        : TexSurface( EN_TEX_2D )
    {

    }

    Texture2DSurface::~Texture2DSurface()
    {
    }

    void Texture2DSurface::Bind() {
        glBindTexture( GL_TEXTURE_2D, m_TexId );
        TexSurface::Bind();
    }

	void Texture2DSurface::Enable() {
		glEnable( GL_TEXTURE_2D );
		TexSurface::Enable();
	}

	void Texture2DSurface::Disable() {
		glDisable( GL_TEXTURE_2D );
	}

    void Texture2DSurface::SetFormat( const pei::Format& format )
    {
        if ( m_TexId > 0  ) {
            m_Format = format;
            double cw = m_Format.GetCanvasWidth();
            double ch = m_Format.GetCanvasHeight();
            double x0 = m_Format.GetXOffset()/cw;
            double y0 = m_Format.GetYOffset()/ch;
            double x1 = (m_Format.GetXOffset()+m_Format.GetWidth())/cw;
            double y1 = (m_Format.GetYOffset()+m_Format.GetHeight())/ch;
            m_TexCoords = TexCoords(  x0, y0, x1, y1 );
        }
    }

    void Texture2DSurface::Lock() const {
        // do we need mutex here??
        m_PixelLock.lock();
#if 0
    	// load texture into system memory
#ifdef _HAS_OPENGL_ // -- This needs to move into profile or RT
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, m_TexId );
        GLfloat w,h;
        glGetTexLevelParameterfv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &w );
        glGetTexLevelParameterfv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h );
        GLfloat border;
        glGetTexLevelParameterfv( GL_TEXTURE_2D, 0, GL_TEXTURE_BORDER, &border );
        GLint pack;
        glGetIntegerv( GL_PACK_ALIGNMENT , &pack );
//         GLint binding;
//         glGetIntegerv( GL_PIXEL_PACK_BUFFER_BINDING, &binding );

        // TODO:: only delete if our pixels are too small to fit texture
        if ( m_Pixels ) {
            delete [] (char*)m_Pixels;
            m_Format.m_CanvasWidth  = w;
            m_Format.m_CanvasHeight = h;
        }
    	m_Pixels = (void*)(new char [ (int)(m_Format.GetPitch() * m_Format.GetCanvasHeight() ) ]);
        // this will be upside down, must flip - or copy line per line
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixels );
        glDisable( GL_TEXTURE_2D );
#endif
#endif

    }

    void Texture2DSurface::Unlock() const {
        if (m_Pixels) {
    		delete [] (char*)m_Pixels;
    		m_Pixels = NULL;
    	}
    	m_PixelLock.unlock();
    }

} /* namespace pei */
