/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include <geometry/buffered_mesh.h>

#include <render/render_profile.h>
#include <render/render_surface.h>
#include <render/render_param.h>
#include <render/tex_surface.h>

#include <iostream>

#include <string.h>


namespace pei
{
    //////////////////////////////////////////////////////////////////////////
    // Mesh

    BufferedMesh::BufferedMesh()
        : m_Offset(0)
        , m_State(IDLE)
    {

    }

    BufferedMesh* BufferedMesh::Reset()
    {
        m_Offset = 0;
        m_Indices.clear();
        m_Vertices.clear();
        m_Normals.clear();
        m_TextureCoords.clear();
        m_Polygons.clear();
        m_CurrentMaterial = pei::Material();
        return this;
    }

    BufferedMesh* BufferedMesh::Begin( int type )
    {
        // just in case someone forgets to close use
        End();
        // do not reset offsets
        m_CurrentPolygon.SetType(type);
        m_State = VERTEX_MODE;
        return this;
    }

    void BufferedMesh::End()
    {
        // checks for 0 indices
        if ( m_CurrentPolygon.GetNumIndices() > 0 ) {
            m_CurrentPolygon.SetMaterial( m_CurrentMaterial );
            AddPolygon( m_CurrentPolygon );
            m_CurrentPolygon.SetOffset( m_CurrentPolygon.GetNumIndices() + m_CurrentPolygon.GetOffset() );
            m_CurrentPolygon.SetIndices(0);
        }
        m_State = IDLE;
    }

    void BufferedMesh::Compile()
    {
        End();
        int num = (int)m_Indices.size();
        if ( num > 0 ) {
            SetIndices( num, (const unsigned int*)&m_Indices.at(0) );
        }
    }

    /** vertex list finishes intermittenly */
    void BufferedMesh::VerifyPolygonBounds( int n ) {
        // we reached poly bounds, split into new one (Mesh will merge back if needed)
        int s = m_CurrentPolygon.size();
        if ( (n > 0) && (s > 0) && ((n % s) == 0) ) {
            // do not call End() - will be called from Begin()
            Begin( m_CurrentPolygon.GetType() );
        }
    }

    BufferedMesh* BufferedMesh::Material( const pei::Material& material )
    {
        m_CurrentMaterial = material;
        // set the material so in case we split this poly we always have the last material attached
        m_CurrentPolygon.SetMaterial( m_CurrentMaterial );
        return this;
    }

    BufferedMesh* BufferedMesh::Vertex( const pei::Vector4f& v )
    {
        if ( m_State == VERTEX_MODE ){
            m_Vertices.assign( &v.asArray()[0], &v.asArray()[2] );
            m_Indices.push_back( m_Indices.size() );
            VerifyPolygonBounds(m_CurrentPolygon++);
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Vertex2f( double x, double y )
    {
        if ( m_State == VERTEX_MODE ){
            m_Vertices.push_back(x);
            m_Vertices.push_back(y);
            m_Vertices.push_back(0); // we use a const size of 3 per vertex
            m_Indices.push_back( m_Indices.size() );
            VerifyPolygonBounds(m_CurrentPolygon++);
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Vertex3f( double x, double y, double z )
    {
        if ( m_State == VERTEX_MODE ){
            m_Vertices.push_back(x);
            m_Vertices.push_back(y);
            m_Vertices.push_back(z);
            m_Indices.push_back( m_Indices.size() );
            VerifyPolygonBounds(m_CurrentPolygon++);
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Vertex2fv( double v[] )
    {
        if ( m_State == VERTEX_MODE ){
            m_Vertices.assign( &v[0], &v[1] );
            m_Vertices.push_back(0); // we use a const size of 3 per vertex
            m_CurrentPolygon++;
            VerifyPolygonBounds(m_CurrentPolygon++);
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Vertex3fv( double v[] )
    {
        if ( m_State == VERTEX_MODE ){
            m_Vertices.assign( &v[0], &v[2] );
            m_Indices.push_back( m_Indices.size() );
            VerifyPolygonBounds(m_CurrentPolygon++);
        }
        return this;
    }

    BufferedMesh* BufferedMesh::TexCoord2f( double u, double v )
    {
        if ( m_State == VERTEX_MODE ){
            m_TextureCoords.push_back( u );
            m_TextureCoords.push_back( v );
        }
        return this;
    }

    BufferedMesh* BufferedMesh::TexCoord3f( double u, double v, double w )
    {
        if ( m_State == VERTEX_MODE ){
            m_TextureCoords.push_back( u );
            m_TextureCoords.push_back( v );
            m_TextureCoords.push_back( w );
        }
        return this;
    }

    BufferedMesh* BufferedMesh::TexCoord2fv( double v[] )
    {
        if ( m_State == VERTEX_MODE ){
            m_TextureCoords.assign( &v[0], &v[1] );
        }
        return this;
    }

    BufferedMesh* BufferedMesh::TexCoord3fv( double v[] )
    {
        if ( m_State == VERTEX_MODE ){
            m_TextureCoords.assign( &v[0], &v[2] );
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Normal3f( double nx, double ny, double nz )
    {
        if ( m_State == VERTEX_MODE ){
            m_Normals.push_back(nx);
            m_Normals.push_back(ny);
            m_Normals.push_back(nz);
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Normal3fv( double nv[] )
    {
        if ( m_State == VERTEX_MODE ){
            m_Normals.assign( &nv[0], &nv[2] );
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Normal( const pei::Vector4f& vec )
    {
        if ( m_State == VERTEX_MODE ){
            m_Normals.assign( &vec.asArray()[0], &vec.asArray()[2] );
        }
        return this;
    }

    BufferedMesh* BufferedMesh::Color3f( float r, float g, float b )
    {
        m_CurrentMaterial.SetColor3f( r,g,b );
        return this;
    }

    BufferedMesh* BufferedMesh::Color4f( float r, float g, float b, float a )
    {
        m_CurrentMaterial.SetColor4f( r,g,b,a );
        return this;
    }

    BufferedMesh* BufferedMesh::Color3fv( float v[3] )
    {
        m_CurrentMaterial.SetColor3fv( v );
        return this;
    }

    BufferedMesh* BufferedMesh::Color4fv( float v[4] )
    {
        m_CurrentMaterial.SetColor4fv( v );
        return this;
    }

    BufferedMesh* BufferedMesh::Color( const pei::Color& col )
    {
        m_CurrentMaterial.SetColor( col );
        return this;
    }

    BufferedMesh* BufferedMesh::Color( const pei::Vector4f& col )
    {
        m_CurrentMaterial.SetColor( col );
        return this;
    }

} /* namespace pei */
