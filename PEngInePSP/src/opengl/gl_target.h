/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef GL_RENDER_TARGET_H
#define GL_RENDER_TARGET_H

#include <profile_config.h>

#ifdef _HAS_OPENGL_

#include <render/render_target.h>
#include <render/color.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace pei
{
    class Surface;
    typedef boost::shared_ptr<Surface> SurfacePtr;

    class RenderParam;

    class RenderProfile;
    typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

    namespace opengl {

        class RenderTarget : public pei::BufferedRenderTarget
        {
        protected:
        public:
            RenderTarget();
            virtual ~RenderTarget() {}

            virtual pei::RenderTargetInterfacePtr Create() const {
                return pei::RenderTargetInterfacePtr( new RenderTarget() );
            }

            virtual pei::SurfacePtr SetSurface( const pei::SurfacePtr& s, int idx = 0 );

            virtual void Render( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param );
        };

        //////////////////////////////////////////////////////////////////////////

        class VertexArrayRenderTarget : public RenderTarget
        {
        protected:
            class Polygon
            {
            protected:
                int m_Type; 	    // e.g. GL.GL_TRIANGLES, GL.GL_QUADS
                int m_NumIndices;  	// num vertices in face (total - spawning multiple quads, for example)
                int m_Offset;

            public:
                Polygon( int t, int n = 4, int o = 0 ) : m_Type(t), m_NumIndices(n), m_Offset(o) {}

                int GetType() const { return m_Type; }

                int GetNumIndices() const { return m_NumIndices; }

                int GetOffset() const { return m_Offset; }

                void AddIndices( int n ) { m_NumIndices += n; }
            };

        protected:
            std::vector<Polygon>       m_Polygons;
            std::vector<float>         m_Vertices;
            std::vector<float>         m_Normals;
            std::vector<float>         m_TextureCoords;
            // I might use a dummy memory block here instead...
            std::vector<unsigned char> m_ByteIndices;
            std::vector<short>         m_ShortIndices;
            std::vector<unsigned int>  m_IntIndices;
            void                      *m_IndexArray;

            int                        m_NumIncides;
            int                        m_IndexType; // GL_UNSIGNED_BYTE
            int                        m_IndexStride;

            // fancy edges - if any
            bool                 m_ShowOutline;
            pei::Color           m_OutlineColor;
        public:
            VertexArrayRenderTarget();
            virtual ~VertexArrayRenderTarget() {}

            virtual pei::RenderTargetInterfacePtr Create() const {
                return pei::RenderTargetInterfacePtr( new VertexArrayRenderTarget() );
            }

            virtual void ShowOutline( bool showOutline ) { m_ShowOutline = showOutline; }

            virtual void SetOutlineColor( const pei::Color& c ) { m_OutlineColor = c; }

            virtual void SetVertices( int num, const float* vertices );

            virtual void SetNormals( int num, const float* normals );

            virtual void SetTexCoords( int num, const float* texCoords );

            virtual void SetIndices( int num, const unsigned char* indices );

            virtual void SetIndices( int num, const short* indices );

            virtual void SetIndices( int num, const unsigned int* indices );

            virtual void AddPolygon( int type, int num, int offset );

            virtual void Render( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param );

        protected:
            virtual void DrawElements( );

        };
    }
} /* namespace pei */

#endif // _HAS_OPENGL_

#endif /* GL_RENDER_TARGET_H */
