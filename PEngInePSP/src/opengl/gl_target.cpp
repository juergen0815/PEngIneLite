/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#ifdef _HAS_OPENGL_

#include <opengl/gl_target.h>

#include <render/render_profile.h>

#include <render/render_surface.h>
#include <render/render_param.h>
#include <render/tex_surface.h>

#include <iostream>

// check: http://glew.sourceforge.net/install.html

#define MUST_HAVE_POW2

namespace pei
{
    namespace opengl {

        static inline int makePow2(int x)
        {
            int val = 1;
            while(val < x) val <<= 1;
            return val;
        }

        static void printEglError(const char *msg, int err, int line, const char* file ) {
//          static const char *errmsg[] = {
//              "function succeeded",
//              "GL is not initialized, or could not be initialized, for the specified display",
//              "cannot access a requested resource",
//              "failed to allocate resources for the requested operation",
//              "an unrecognized attribute or attribute value was passed in an attribute list",
//              "an EGLConfig argument does not name a valid EGLConfig",
//              "an EGLContext argument does not name a valid EGLContext",
//              "the current surface of the calling thread is no longer valid",
//              "an EGLDisplay argument does not name a valid EGLDisplay",
//              "arguments are inconsistent",
//              "an EGLNativePixmapType argument does not refer to a valid native pixmap",
//              "an EGLNativeWindowType argument does not refer to a valid native window",
//              "one or more argument values are invalid",
//              "an EGLSurface argument does not name a valid surface configured for rendering",
//              "a power management event has occurred",
//          };

//          fprintf(stderr, "%s: %s\n", msg, errmsg[err - GL_SUCCESS]);

          std::cerr << "OpenGL Target Error: '" << msg
                    << "' Code: "<< err << " '" << gluErrorString(err)
                    << "' @Line: " << line << " in '" << file << "'" << std::endl;
        }

        RenderTarget::RenderTarget()
        {
            // SetSurface( SurfacePtr( new Texture2DSurface() ) );
        }

        pei::SurfacePtr RenderTarget::SetSurface( const pei::SurfacePtr& surface, int idx /*= 0*/ )
        {
            if ( surface ) {

                Texture2DSurfacePtr texture( boost::dynamic_pointer_cast<Texture2DSurface>(surface) );
                // if the surface is already a texture, don't bother loading it again
                if ( texture ) {
                    // we already have a texture. No need to load any more. Just store it
                    BufferedRenderTarget::SetSurface( texture, idx );
                }
                else {
                    GLint pixFmt(0);
                    pei::Format fmt(surface->GetFormat());
                    if ( fmt.GetBytesPerPixel() == 3 ) {
                        // always RGB
                        pixFmt = GL_RGB;
                    } else if ( fmt.GetBytesPerPixel() == 4 ) {
                        // always RBGA
                        pixFmt = GL_RGBA;
                    } else if ( fmt.GetBytesPerPixel() == 1 ) {
                        if ( fmt.GetType() == pei::Format::TYPE_LUMINANCE ) {
                            pixFmt = GL_LUMINANCE; // grey scale
                        } else if ( fmt.GetType() == pei::Format::TYPE_ALPHA ) {
                            pixFmt = GL_ALPHA;
                        }
                    } else if ( fmt.GetBytesPerPixel() == 2 ) {
                        if ( fmt.GetType() == pei::Format::TYPE_LUMINANCE_ALPHA ) {
                            pixFmt = GL_LUMINANCE_ALPHA;
                        }
                    }
                    if ( pixFmt != 0 ) {
                        // safe image canvas sizes as real texutre size
                        float tw(fmt.m_CanvasWidth);
                        float th(fmt.m_CanvasHeight);

                        int wrapMode(GL_CLAMP_TO_EDGE);
                        int textureMode = GetRenderMode( ) & pei::EN_TEXTURE_MODE_MASK;
                        if ( textureMode & pei::EN_TEXTURE_CLAMP_F ) {
                            wrapMode = GL_CLAMP;
                        }
                        else if ( textureMode & pei::EN_TEXTURE_REPEAT_F ) {
                            wrapMode = GL_REPEAT;
                        }
                        else if ( textureMode & pei::EN_TEXTURE_CLAMP_EDGE_F ) {
                            wrapMode = GL_CLAMP_TO_EDGE;
                        }
                        else if ( textureMode & pei::EN_TEXTURE_CLAMP_BORDER_F ) {
                            wrapMode = GL_CLAMP_TO_BORDER;
                        }
                        // if current surface is not a texture, make one
                        texture = boost::dynamic_pointer_cast<Texture2DSurface>(GetSurface(idx));
                        if ( texture == NULL ) {
                            // this converts any 2D surface into a texture - I could cache the real surface...hm...fast reads...
                            texture = pei::Texture2DSurfacePtr( new pei::Texture2DSurface( ) );
                            // generates texture coordinates
#ifdef MUST_HAVE_POW2
                            // for pow2 textures we create a pow2 canvas and non-pow SubImage
                            if ( GetRenderMode() & pei::EN_TEXTURE_FORCE_POW2_F ) {
                                fmt.m_CanvasWidth  = makePow2( fmt.m_CanvasWidth );
                                fmt.m_CanvasHeight = makePow2( fmt.m_CanvasHeight );
                            }
#endif
                            // UV coords are calculated here!
                            texture->SetFormat( fmt );

                            texture->Bind();
                            // calc line alignment
                            int align = /* fmt.GetBytesPerPixel() == 1 ? 1 : */ (4 - (fmt.m_Pitch & 3));
                            glPixelStorei(GL_UNPACK_ALIGNMENT, align);
                            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

                            // TODO: OpenGL > 3.1 use glGenerateMipmap
                            // test this
                            if ( GetRenderMode() & pei::EN_CREATE_MIP_MAPS_F ) {
                                glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
                            }
                            glTexImage2D(GL_TEXTURE_2D, 0, pixFmt,
                                         fmt.m_CanvasWidth, fmt.m_CanvasHeight, 0,
                                         pixFmt , GL_UNSIGNED_BYTE, NULL);

                            surface->Lock();
                            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                            tw, th,
                                            pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                            surface->Unlock();
                            BufferedRenderTarget::SetSurface( texture, idx );

                        } else {

                            // Update existing texture with new pixels -- e.g. stream from system memory

                            if ( surface->GetWidth()  <= texture->GetFormat().GetCanvasWidth()  &&
                                 surface->GetHeight() <= texture->GetFormat().GetCanvasHeight() &&
                                 surface->GetBytesPerPixel() == texture->GetBytesPerPixel() )
                            {
                                texture->Bind();

                                int align = fmt.GetBytesPerPixel() == 1 ? 1 : (4 - (fmt.m_Pitch & 3));
                                glPixelStorei(GL_UNPACK_ALIGNMENT, align);
                                surface->Lock();
                                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                                tw, th,
                                                pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                                surface->Unlock();
                            } else {
                                // generates texture coordinates
#ifdef MUST_HAVE_POW2
                                // for pow2 textures we create a pow2 canvas and non-pow SubImage
                                if ( GetRenderMode() & pei::EN_TEXTURE_FORCE_POW2_F ) {
                                    fmt.m_CanvasWidth  = makePow2( fmt.m_CanvasWidth );
                                    fmt.m_CanvasHeight = makePow2( fmt.m_CanvasHeight );
                                }
#endif
                                texture->SetFormat( fmt );

                                texture->Bind();
                                int align = fmt.GetBytesPerPixel() == 1 ? 1 : (4 - (fmt.m_Pitch & 3));
                                glPixelStorei(GL_UNPACK_ALIGNMENT, align);
                                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

                                // TODO: OpenGL > 3.1 use glGenerateMipmap
                                // test this
                                if ( GetRenderMode() & pei::EN_CREATE_MIP_MAPS_F ) {
                                    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
                                }

                                glTexImage2D(GL_TEXTURE_2D, 0, pixFmt,
                                             fmt.m_CanvasWidth, fmt.m_CanvasHeight, 0,
                                             pixFmt , GL_UNSIGNED_BYTE, NULL);
                                surface->Lock();
                                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                                tw, th,
                                                pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                                surface->Unlock();
                            }
                        }
                    }
                }
                return texture;
            }

#if 0
            if ( surface ) {
                // if current surface is not a texture, make one
                Texture2DSurfacePtr texture = boost::dynamic_pointer_cast<Texture2DSurface>(surface);
                if ( texture ) {
                    BufferedRenderTarget::SetSurface( texture, idx );
                } else {

                    GLint pixFmt(0);
                    pei::Format fmt(surface->GetFormat());

                    if ( fmt.GetBytesPerPixel() == 3 ) {
                        // always RGB
                        pixFmt = GL_RGB;
                    } else if ( fmt.GetBytesPerPixel() == 4 ) {
                        // always RBGA
                        pixFmt = GL_RGBA;
                    } else if ( fmt.GetBytesPerPixel() == 1 ) {
                        if ( fmt.GetType() == pei::Format::TYPE_LUMINANCE ) {
                            pixFmt = GL_LUMINANCE; // grey scale
                        } else if ( fmt.GetType() == pei::Format::TYPE_ALPHA ) {
                            pixFmt = GL_ALPHA;
                        }
                    } else if ( fmt.GetBytesPerPixel() == 2 ) {
                        if ( fmt.GetType() == pei::Format::TYPE_LUMINANCE_ALPHA ) {
                            pixFmt = GL_LUMINANCE_ALPHA;
                        }
                    }
                    if ( pixFmt != 0 ) {
                        int wrapMode(GL_CLAMP_TO_EDGE);
                        int textureMode = GetRenderMode( ) & pei::EN_TEXTURE_MODE_MASK;
                        if ( textureMode & pei::EN_TEXTURE_CLAMP_F ) {
                            wrapMode = GL_CLAMP;
                        }
                        else if ( textureMode & pei::EN_TEXTURE_REPEAT_F ) {
                            wrapMode = GL_REPEAT;
                        }
                        else if ( textureMode & pei::EN_TEXTURE_CLAMP_EDGE_F ) {
                            wrapMode = GL_CLAMP_TO_EDGE;
                        }
                        else if ( textureMode & pei::EN_TEXTURE_CLAMP_BORDER_F ) {
                            wrapMode = GL_CLAMP_TO_BORDER;
                        }
                        // if current surface is not a texture, make one
                        texture = boost::dynamic_pointer_cast<Texture2DSurface>(GetSurface(idx));
                        if ( texture == NULL ) {
                            // this converts any 2D surface into a texture - I could cache the real surface...hm...fast reads...
                            texture = pei::Texture2DSurfacePtr( new pei::Texture2DSurface( ) );
#ifdef MUST_HAVE_POW2
                            fmt.m_CanvasWidth  = makePow2( fmt.m_CanvasWidth );
                            fmt.m_CanvasHeight = makePow2( fmt.m_CanvasHeight );
#endif
                            // generates texture coordinates
                            texture->SetFormat( fmt );
                            surface->Lock();

                            glBindTexture(GL_TEXTURE_2D, texture->GetId());
                            // calc line alignment
                            int align = 4 - (fmt.m_Pitch & 3);
                            glPixelStorei(GL_UNPACK_ALIGNMENT, align);
        #if 0
                            glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
                            glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
                            glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                            glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                            glPixelStorei(GL_UNPACK_ROW_LENGTH, m_Format.GetPitch()/m_Format.GetBytesPerPixel() );

                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
                            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        #endif
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode );
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode );

                            if ( GetRenderMode() & pei::EN_CREATE_MIP_MAPS_F ) {
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                                gluBuild2DMipmaps( GL_TEXTURE_2D, pixFmt, surface->GetWidth(), surface->GetHeight(), pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                            } else {
                                // no mip maps, e.g. for animated textures, etc or in 2D mode
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->GetFilter() );
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->GetFilter() );
                                glTexImage2D(GL_TEXTURE_2D, 0, pixFmt,
                                             fmt.m_CanvasWidth, fmt.m_CanvasHeight, 0,
                                             pixFmt , GL_UNSIGNED_BYTE, NULL);

                                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                                surface->GetWidth(), surface->GetHeight(),
                                                pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                            }
                            surface->Unlock();
                            BufferedRenderTarget::SetSurface( texture, idx );

                        } else {

                            if ( surface->GetWidth()  <= texture->GetFormat().GetCanvasWidth()  &&
                                 surface->GetHeight() <= texture->GetFormat().GetCanvasHeight() &&
                                 surface->GetBytesPerPixel() == texture->GetBytesPerPixel() )
                            {
                                glBindTexture(GL_TEXTURE_2D, texture->GetId());
                                surface->Lock();
                                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                                surface->GetWidth(), surface->GetHeight(),
                                                pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                                surface->Unlock();
                            } else {
                                Format fmt(surface->GetFormat());
#ifdef MUST_HAVE_POW2
                                fmt.m_CanvasWidth  = makePow2( fmt.m_CanvasWidth );
                                fmt.m_CanvasHeight = makePow2( fmt.m_CanvasHeight );
#endif
                                // generates texture coordinates
                                surface->SetFormat( fmt );

                                glBindTexture(GL_TEXTURE_2D, texture->GetId());
                                int align = 4 - (fmt.m_Pitch & 3);
                                glPixelStorei(GL_UNPACK_ALIGNMENT, align);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode );
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode );

                                surface->Lock();
                                if ( GetRenderMode() & pei::EN_CREATE_MIP_MAPS_F ) {
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
                                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                                    gluBuild2DMipmaps( GL_TEXTURE_2D, pixFmt, surface->GetWidth(), surface->GetHeight(), pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                                } else {
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->GetFilter() );
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->GetFilter() );
                                    glTexImage2D(GL_TEXTURE_2D, 0, pixFmt,
                                                 fmt.m_CanvasWidth, fmt.m_CanvasHeight, 0,
                                                 pixFmt , GL_UNSIGNED_BYTE, NULL);
                                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                                    surface->GetWidth(), surface->GetHeight(),
                                                    pixFmt, GL_UNSIGNED_BYTE, surface->GetPixels());
                                }
                                surface->Unlock();
                            }
                        }
                    }
                    return texture;
                }
            }
#endif
            return pei::SurfacePtr();
        }

        void RenderTarget::Render( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            int renderMode = GetRenderMode();

            int glErr(0);
            GLfloat x0 = -param.GetXOrigin();
            GLfloat y0 = -param.GetYOrigin();
            GLfloat  z = -param.GetZOrigin();
            GLfloat x1 = x0 + param.GetWidth();
            GLfloat y1 = y0 + param.GetHeight();
            GLfloat vertices[] =
            {
                x0, y0, z,
                x1, y0, z,
                x1, y1, z,
                x0, y1, z
            };
            const pei::Color& c = param.GetColor();
            float col[] = { c.R(), c.G(), c.B(), c.A() };

            int gl_blend_src, gl_blend_dst;
            glGetIntegerv(GL_BLEND_SRC, &gl_blend_src);
            glGetIntegerv(GL_BLEND_DST, &gl_blend_dst);
            int blend_enabled;
            glGetIntegerv(GL_BLEND, &blend_enabled);

            int vertex_array_enabled;
            glGetIntegerv(GL_VERTEX_ARRAY, &vertex_array_enabled);
            if (!vertex_array_enabled) {
                glEnableClientState(GL_VERTEX_ARRAY);
            }
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            if ( (glErr = glGetError()) != GL_NO_ERROR ) {
                printEglError( "glVertexPointer",glErr, __LINE__, __FILE__ );
            }
            int tex (0);
            pei::Texture2DSurfacePtr texture = boost::dynamic_pointer_cast<pei::Texture2DSurface>(GetSurface(tex));
            if ( texture != NULL && (renderMode & pei::EN_DISABLE_TEXTURE_F) == 0 ) {
                int texture_enabled;
                glGetIntegerv(GL_TEXTURE_2D, &texture_enabled);
                if (!texture_enabled) {
                    glEnable(GL_TEXTURE_2D);
                    if ( (glErr = glGetError()) != GL_NO_ERROR ) {
                        printEglError( "glEnable(GL_TEXTURE_2D)",glErr, __LINE__, __FILE__ );
                    }

                }
                // I might want to flip these upside down...to be in sync with 3D coords
                const TexSurface::TexCoords &coords = texture->GetCoords();
                GLfloat texCoords[] =
                {
                    coords.MinX, coords.MinY,
                    coords.MaxX, coords.MinY,
                    coords.MaxX, coords.MaxY,
                    coords.MinX, coords.MaxY,
                };
                int texture_array_enabled;
                glGetIntegerv(GL_TEXTURE_COORD_ARRAY, &texture_array_enabled);
                if (!texture_array_enabled) {
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    if ( (glErr = glGetError()) != GL_NO_ERROR ) {
                        printEglError( "glEnableClientState(GL_TEXTURE_COORD_ARRAY)",glErr, __LINE__, __FILE__ );
                    }
                }
                glTexCoordPointer(2, GL_FLOAT, 0, texCoords );
                if ( (glErr = glGetError()) != GL_NO_ERROR ) {
                    printEglError( "glTexCoordPointer", glErr, __LINE__, __FILE__ );
                }

                if ( (( renderMode & EN_ALPHA_F) != 0) ) {
                    if (!blend_enabled) glEnable( GL_BLEND );
                    glEnable( GL_ALPHA_TEST );
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    if ( ( renderMode & EN_BLEND_F ) != 0 )
                    {
                        glColor4f( col[0], col[1], col[2], col[3] );
                    }
                    else
                    {
                        // alpha only
                        glColor4f( 1.0, 1.0, 1.0, col[3] );
//                            glColorMask( false, false, false, true );
                    }
                    // not sure if we do color blending with textures...
                } else {
                    // no alpha but color blending only - or no blending at all
                    glDisable( GL_ALPHA_TEST );
                    if ( (renderMode & EN_BLEND_F) != 0 )
                    {
                        if (!blend_enabled) glEnable( GL_BLEND );
                        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                        glColor4f( col[0], col[1], col[2], 1.0 );
                    }
                    else
                    {
                        if (blend_enabled) glDisable( GL_BLEND );
                    }
                }

                texture->Bind();
                GLshort idx[] = { 0, 1, 3, 2 };
                glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, idx );
//                    glColorMask( true, true, true, true );

                if ( (glErr = glGetError()) != GL_NO_ERROR ) {
                    printEglError( "glDrawElements", glErr, __LINE__, __FILE__ );
                }
                if (!texture_array_enabled) {
                    glDisableClientState( GL_TEXTURE_COORD_ARRAY);
                }
                if (!texture_enabled) {
                    glDisable(GL_TEXTURE_2D);
                }
            } else {
                int texture_enabled;
                glGetIntegerv(GL_TEXTURE_2D, &texture_enabled);
                if (texture_enabled) {
                    glDisable(GL_TEXTURE_2D);
                }
                int texture_array_enabled;
                glGetIntegerv(GL_TEXTURE_COORD_ARRAY, &texture_array_enabled);
                if (texture_array_enabled) {
                    glDisableClientState( GL_TEXTURE_COORD_ARRAY);
                }
                if ( (( renderMode & (EN_ALPHA_F|EN_BLEND_F)) != 0) ) {
                    if (!blend_enabled) glEnable( GL_BLEND );
                    glEnable( GL_ALPHA_TEST );
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor4f( col[0], col[1], col[2], col[3] );
                } else {
                    if (blend_enabled) glDisable( GL_BLEND );
                    glDisable( GL_ALPHA_TEST );
                    glColor4f( col[0], col[1], col[2], 1.0 );
                }
                GLshort idx[] = { 0, 1, 3, 2 };
                glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, idx );
            }
            if (!vertex_array_enabled) {
                glDisableClientState(GL_VERTEX_ARRAY);
            }

            // restore blend function
            glBlendFunc(gl_blend_src, gl_blend_dst);
            if (!blend_enabled) glDisable( GL_BLEND );

            if ( (renderMode & EN_WIREFRAME_F ) != 0 ) {
                bool skipRender(false);
                const pei::Color& c = param.GetColor();
                if ( (( renderMode & (EN_ALPHA_F|EN_BLEND_F)) != 0) && c.a < pei::Color::PIXF_OPAQUE ) {
                    if ( c.a == 0 ) {
                        skipRender = true;
                    } else {
                        glEnable(GL_BLEND);
                        glEnable( GL_ALPHA_TEST );
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glColor4f( c.R(), c.G(), c.B(), c.A());
                    }
                } else {
                    glColor3f( c.R(), c.G(), c.B() );
                }
                if (!skipRender) {
                    glPushAttrib( GL_POLYGON_BIT );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
                    double x0 = param.GetXOrigin();
                    double y0 = param.GetYOrigin();
                    double x1 = x0 + param.GetWidth();
                    double y1 = y0 + param.GetHeight();
                    glBegin(GL_QUADS);
                        glVertex2f(x0, y0 );
                        glVertex2f(x1, y0 );
                        glVertex2f(x1, y1 );
                        glVertex2f(x0, y1 );
                    glEnd();
                    glPopAttrib();
                }
            }

        }


        //////////////////////////////////////////////////////////////////////////
        // Vertex Arrays

        VertexArrayRenderTarget::VertexArrayRenderTarget()
            : m_IndexArray( NULL )
            , m_NumIncides(0)
            , m_IndexType( GL_UNSIGNED_BYTE )
            , m_IndexStride(1)
            , m_ShowOutline( false )
            , m_OutlineColor( 1.0, 1.0, 0.0, 0.75 )
        {
        }

        void VertexArrayRenderTarget::Render( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param )
        {
            if ( m_Vertices.size() && m_IndexArray != NULL ) {
                int renderMode = GetRenderMode();
                const pei::Color& c = param.GetColor();
                float col[] = { c.R(), c.G(), c.B(), c.A() };

                int gl_blend_src, gl_blend_dst;
                glGetIntegerv(GL_BLEND_SRC, &gl_blend_src);
                glGetIntegerv(GL_BLEND_DST, &gl_blend_dst);
                int blend_enabled;
                glGetIntegerv(GL_BLEND, &blend_enabled);

                pei::Texture2DSurfacePtr texture = boost::dynamic_pointer_cast<pei::Texture2DSurface>(GetSurface(0));
                if ( (renderMode & pei::EN_DISABLE_TEXTURE_F) == 0 &&
                     texture != NULL && m_TextureCoords.size() > 0 &&
                     texture->GetWidth() >  0 && texture->GetHeight() > 0
                   )
                {
                    if ( (( renderMode & EN_ALPHA_F) != 0) ) {
                        // always enable blend - either alpha or color blending
                        if (!blend_enabled) glEnable( GL_BLEND );
                        glEnable( GL_ALPHA_TEST );
                        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        if ( ( renderMode & EN_BLEND_F ) != 0 )
                        {
                            glColor4f( col[0], col[1], col[2], col[3] );
                        }
                        else
                        {
                            // alpha only!
                            glColor4f( 1.0, 1.0, 1.0, col[3] );
                        }
                        // not sure if we do color blending with textures...
                    } else {
                        // no alpha but color blending only - or no blending at all
                        glDisable( GL_ALPHA_TEST );
                        if ( (renderMode & EN_BLEND_F) != 0 )
                        {
                            if (!blend_enabled) glEnable( GL_BLEND );
                            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                            glColor4f( col[0], col[1], col[2], 1.0 );
                        }
                        else
                        {
                            if (blend_enabled) glDisable( GL_BLEND );
                        }
                    }

                    int texture_array_enabled;
                    glGetIntegerv(GL_TEXTURE_COORD_ARRAY, &texture_array_enabled);
                    if (!texture_array_enabled) {
                        glEnableClientState( GL_TEXTURE_COORD_ARRAY);
                    }
                    glTexCoordPointer(2, GL_FLOAT, 0, (const GLvoid*)(&m_TextureCoords.at(0)));
                    int texture_enabled;
                    glGetIntegerv(GL_TEXTURE_2D, &texture_enabled);
                    if (!texture_enabled) {
                        glEnable(GL_TEXTURE_2D);
                    }
                    texture->Bind();
                    DrawElements();
                    if (!texture_enabled) {
                        glDisable(GL_TEXTURE_2D);
                    }
                    if (!texture_array_enabled) {
                        glDisableClientState( GL_TEXTURE_COORD_ARRAY);
                    }
                } else {
                    int texture_enabled;
                    glGetIntegerv(GL_TEXTURE_2D, &texture_enabled);
                    if (texture_enabled) {
                        glDisable(GL_TEXTURE_2D);
                    }
                    int texture_array_enabled;
                    glGetIntegerv(GL_TEXTURE_COORD_ARRAY, &texture_array_enabled);
                    if (texture_array_enabled) {
                        glDisableClientState( GL_TEXTURE_COORD_ARRAY);
                    }

                    if ( (( renderMode & (EN_ALPHA_F|EN_BLEND_F)) != 0) && col[3] < 1.0 ) {
                        if (!blend_enabled) glEnable( GL_BLEND );
                        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glEnable( GL_ALPHA_TEST );
                        glColor4f( col[0], col[1], col[2], col[3] );
                    } else {
                        if (blend_enabled) glDisable( GL_BLEND );
                        glDisable( GL_ALPHA_TEST );
                        glColor4f( col[0], col[1], col[2], 1.0 );
                    }
                    DrawElements();
                }

                // restore blend function
                glBlendFunc(gl_blend_src, gl_blend_dst);
                if (!blend_enabled) glDisable( GL_BLEND );

                // this is expensive - disabled by default
                if ( m_ShowOutline || (renderMode & EN_WIREFRAME_F) != 0 ) {
                    glPushAttrib( GL_POLYGON_BIT );
                    glEnable( GL_BLEND );
                    glEnable( GL_ALPHA_TEST );
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor4f( m_OutlineColor.R(), m_OutlineColor.G(), m_OutlineColor.B(), m_OutlineColor.A() );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
                    DrawElements( );
                    glPopAttrib();
                }
            }
            // cannot render anything else than meshes
        }

        void VertexArrayRenderTarget::DrawElements( )
        {
            int vertex_array_enabled;
            glGetIntegerv(GL_VERTEX_ARRAY, &vertex_array_enabled);
            if (!vertex_array_enabled) {
                glEnableClientState( GL_VERTEX_ARRAY );
            }
            const GLvoid* va = (const GLvoid*)(&m_Vertices.at(0));
            glVertexPointer( 3, GL_FLOAT, 0, va );
            int normal_array_enabled;
            glGetIntegerv(GL_NORMAL_ARRAY, &normal_array_enabled);
            if ( m_Normals.size() > 0 ) {
                if (!normal_array_enabled) {
                    glEnableClientState( GL_NORMAL_ARRAY);
                }
                const GLvoid* na = (const GLvoid*)(&m_Normals.at(0));
                glNormalPointer( GL_FLOAT, 0, na );
            }
            if ( m_Polygons.size() ) {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

                std::vector<Polygon>::iterator pit = m_Polygons.begin();
                while ( pit != m_Polygons.end() ) {
                    const Polygon& p = *pit++;
                    int len   = p.GetNumIndices();
                    int start = p.GetOffset();
                    if ( (start+len) <= m_NumIncides) {
                        GLvoid *indexArray = (GLvoid *)(((char*)m_IndexArray) + start*m_IndexStride);
                        glDrawElements( p.GetType(), len, m_IndexType, indexArray );
                    }
                }
            }
            else // no polys. Assume all quads...dangerous...remove later
            {
                glDrawElements( GL_TRIANGLE_STRIP, m_NumIncides, m_IndexType, m_IndexArray );
            }
            if (!normal_array_enabled) {
                glDisableClientState( GL_NORMAL_ARRAY );
            }
            if (!vertex_array_enabled) {
                glDisableClientState( GL_VERTEX_ARRAY );
            }
        }

        void VertexArrayRenderTarget::SetVertices( int num, const float* vertices )
        {
            m_Vertices.assign( vertices, vertices+num );
        }

        void VertexArrayRenderTarget::SetNormals( int num, const float* normals )
        {
            m_Normals.assign( normals, normals+num );
        }

        void VertexArrayRenderTarget::SetTexCoords( int num, const float* texCoords )
        {
            m_TextureCoords.assign( texCoords, texCoords+num );
        }

        void VertexArrayRenderTarget::SetIndices( int num, const unsigned char* indices )
        {
            m_ByteIndices.assign( indices, indices+num );
            m_IndexType   = GL_UNSIGNED_BYTE;
            m_IndexStride = 1;
            m_IndexArray  = (void*)(&m_ByteIndices.at(0));
            m_NumIncides  = num;
        }

        void VertexArrayRenderTarget::SetIndices( int num, const short* indices )
        {
            m_ShortIndices.assign( indices, indices+num );
            m_IndexType   = GL_UNSIGNED_SHORT;
            m_IndexStride = 2;
            m_IndexArray  = (void*)(&m_ShortIndices.at(0));
            m_NumIncides  = num;
        }

        void VertexArrayRenderTarget::SetIndices( int num, const unsigned int* indices )
        {
            m_IntIndices.assign( indices, indices+num );
            m_IndexType   = GL_UNSIGNED_INT;
            m_IndexStride = 4;
            m_IndexArray  = (void*)(&m_IntIndices.at(0));
            m_NumIncides  = num;
        }

        void VertexArrayRenderTarget::AddPolygon( int type, int num, int offset )
        {
            if ( num == 0 ) {
                return ;
            }
            // optimize number of polygons/draw calls:
            // if type is the same, except all "STRIP" types,
            // merge the current poly into the last one.
            // Offsets are always absolute. No need to touch that.
            // Just increment number of indices for the previous polygon
            if ( m_Polygons.size() == 0 ||
                type == GL_TRIANGLE_STRIP ||
                type == GL_QUAD_STRIP ||
                type == GL_LINES ||
                type == GL_LINE_STRIP )
            {
                m_Polygons.push_back( Polygon(type, num, offset ) );
            } else {
                Polygon &last = *(m_Polygons.rend()-1); // rend points at .end()
                if ( last.GetType() == type ) {
                    last.AddIndices(num); // simply increase number of indices
                } else {
                    m_Polygons.push_back( Polygon(type, num, offset ) );
                }
            }

            // m_NumFaces += num;
        }
    }
} /* namespace pei */

#endif // _HAS_OPENGL_
