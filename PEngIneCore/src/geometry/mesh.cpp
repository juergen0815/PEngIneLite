/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include <geometry/mesh.h>

#include <render/render_profile.h>
#include <render/render_surface.h>
#include <render/render_param.h>
#include <render/tex_surface.h>

#include <iostream>

#include <string.h>

// check: http://glew.sourceforge.net/install.html

namespace pei
{
    //////////////////////////////////////////////////////////////////////////
    // Mesh

    Material::Material( )
        : m_Color(1.0f,1.0f,1.0f,1.0f)
        , m_RenderMode(0)
        , m_ParentRenderModeMask(~0L)
    {
    }

    Material::Material( const Material& material )
        : m_Color(material.m_Color)
        , m_RenderMode(material.m_RenderMode)
        , m_ParentRenderModeMask(material.m_ParentRenderModeMask)
    {
    }

    Material* Material::SetColor( const pei::Vector4f& col )
    {
        m_Color = col;
        return this;
    }

    Material* Material::SetColor( const pei::Color& col )
    {
        m_Color.x(col.R());
        m_Color.y(col.G());
        m_Color.z(col.B());
        m_Color.w(col.A());
        return this;
    }

    Material* Material::SetColor3f( float r, float g, float b )
    {
        m_Color.x(r);
        m_Color.y(g);
        m_Color.z(b);
        return this;
    }

    Material* Material::SetColor4f( float r, float g, float b, float a )
    {
        m_Color.x(r);
        m_Color.y(g);
        m_Color.z(b);
        m_Color.w(a);
        return this;
    }

    Material* Material::SetColor3fv( const float v[3] )
    {
        m_Color.Set3f(v);
        return this;
    }

    Material* Material::SetColor4fv( const float v[4] )
    {
        m_Color.Set4f(v);
        return this;
    }

    const pei::Vector4f& Material::GetColor() const
    {
        return m_Color;
    }

    Material* Material::SetRenderMode( int rm, unsigned int parentMask  ) {
        m_RenderMode = rm;
        m_ParentRenderModeMask = parentMask;
        return this;
    }

    int Material::GetRenderMode( int parent ) const {
        return (m_RenderMode | ( parent & m_ParentRenderModeMask));
    }

    //////////////////////////////////////////////////////////////////////////

    IndexArray::IndexArray( int num, const unsigned char* indices )
    {
        m_IndexType   = GL_UNSIGNED_BYTE;
        m_IndexStride = 1;
        m_NumIncides  = num;
        m_Indices = (void*)new unsigned char[ num ];
        memcpy( m_Indices, indices, num );
    }
    IndexArray::IndexArray( int num, const unsigned short* indices )
    {
        m_IndexType   = GL_UNSIGNED_SHORT;
        m_IndexStride = 2;
        m_NumIncides  = num;
        m_Indices = (void*)new unsigned short[ num ];
        memcpy( m_Indices, indices, num*sizeof(unsigned short) );
    }
    IndexArray::IndexArray( int num, const unsigned int* indices )
    {
#if defined _HAS_OPENGL_
        m_IndexType   = GL_UNSIGNED_INT;
        m_IndexStride = 4;
        m_NumIncides  = num;
        m_Indices = (void*)new unsigned int[ num ];
        memcpy( m_Indices, indices, num*sizeof(unsigned int) );
#endif
    }

    IndexArray::~IndexArray()
    {
        // just a block of memory...
        if (m_Indices) delete [] (char*)m_Indices;
    }

    //////////////////////////////////////////////////////////////////////////

    int Polygon::size() {
        switch ( GetType( ) )
        {
            case GL_POINTS:         return 1;
            case GL_LINES:          return 2;
            case GL_LINE_LOOP:      return 2;
            case GL_LINE_STRIP:     return 2;
            case GL_TRIANGLES:      return 3;
            case GL_TRIANGLE_STRIP: return -1;
            case GL_TRIANGLE_FAN:   return -1; // can be unlimited
#ifdef _HAS_OPENGL_
            case GL_QUADS:          return 4;
            case GL_QUAD_STRIP:     return -1;
            case GL_POLYGON:        return -1; // unlimited
#endif
        }
        return 0; // invalid
    }

    //////////////////////////////////////////////////////////////////////////


    Mesh::Mesh()
    {
    }

    int Mesh::GetSize() const {
        return (m_Indices != NULL) ? m_Vertices.size() : 0;
    }

    void Mesh::SetVertices( int num, const float* vertices )
    {
        m_Vertices.assign( vertices, vertices+num );
    }

    const std::vector<float>& Mesh::GetVertices( ) const {
        return m_Vertices;
    }

    void Mesh::SetNormals( int num, const float* normals )
    {
        m_Normals.assign( normals, normals+num );
    }

    const std::vector<float>& Mesh::GetNormals() const {
        return m_Normals;
    }

    void Mesh::SetTexCoords( int num, const float* texCoords )
    {
        m_TextureCoords.assign( texCoords, texCoords+num );
    }

    const std::vector<float>& Mesh::GetTexCoords() const {
        return m_TextureCoords;
    }

    void Mesh::SetIndices( int num, const unsigned char* indices )
    {
        m_Indices = IndexArrayPtr( new IndexArray( num, indices ) );
    }

    void Mesh::SetIndices( int num, const unsigned short* indices )
    {
        m_Indices = IndexArrayPtr( new IndexArray( num, indices ) );
    }

    void Mesh::SetIndices( int num, const unsigned int* indices )
    {
        m_Indices = IndexArrayPtr( new IndexArray( num, indices ) );
    }

    // short lifetime access!!
    const IndexArrayPtr& Mesh::GetIndices( ) const {
        return m_Indices;
    }


    bool Mesh::AddPolygon( int type, int num, int offset, const Material& material )
    {
        return AddPolygon( type, num, offset, &material );
    }

    bool Mesh::AddPolygon( int type, int num, int offset )
    {
        return AddPolygon( type, num, offset, NULL );
    }

    bool Mesh::AddPolygon( const Polygon& poly )
    {
        return AddPolygon( poly.GetType(), poly.GetNumIndices(), poly.GetOffset(), poly.GetMaterial() );
    }

    bool Mesh::AddPolygon( int type, int num, int offset, const Material* material )
    {
        if ( num == 0 ) {
            return false;
        }
        // optimize number of polygons/draw calls:
        // if type is the same, except all "STRIP" types,
        // merge the current poly into the last one.
        // Offsets are always absolute. No need to touch that.
        // Just increment number of indices for the previous polygon
        if ( m_Polygons.size() == 0 ||
            type == GL_TRIANGLE_STRIP ||
#ifdef _HAS_OPENGL_
            type == GL_QUAD_STRIP ||
#endif
            type == GL_LINES ||
            type == GL_LINE_STRIP )
        {
            if ( material ) {
                m_Polygons.push_back( Polygon(type, num, offset, *material ) );
            } else {
                m_Polygons.push_back( Polygon(type, num, offset ) );
            }
        } else {
            Polygon &last = *(m_Polygons.rend()-1); // rend points at .end()
            if ( last.GetType() == type && (material && last.HasMaterial( *material ))) {
                last.AddIndices(num); // simply increase number of indices
            } else {
                if ( material ) {
                    m_Polygons.push_back( Polygon(type, num, offset, *material ) );
                } else {
                    m_Polygons.push_back( Polygon(type, num, offset ) );
                }
            }
        }
        return true;
    }

    const std::vector<Polygon>& Mesh::GetPolygons() const {
        return m_Polygons;
    }

} /* namespace pei */
