/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_BUFFERED_MESH_H
#define RENDER_BUFFERED_MESH_H

#include <render/color.h>
#include <geometry/mesh.h>

#include <math/vector4f.h>

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace pei
{
    class BufferedMesh;
    typedef boost::shared_ptr<BufferedMesh> BufferedMeshPtr;

    //////////////////////////////////////////////////////////////////////////

	class BufferedMesh : public Mesh
    {
    protected:

    protected:
        BufferedMeshPtr& LinkSmartPtr( BufferedMeshPtr& self ) {
            m_WeakSelf = self;
            return self;
        }

        pei::Polygon     m_CurrentPolygon;
        pei::Material    m_CurrentMaterial;
        int              m_Offset;
        std::vector<unsigned int> m_Indices;

        enum enState
        {
            IDLE = 0,
            VERTEX_MODE
        };
        int             m_State;
    public:
        BufferedMesh();

        virtual ~BufferedMesh() {}

        static BufferedMeshPtr Create() {
            pei::BufferedMeshPtr self( new BufferedMesh() );
            return self->LinkSmartPtr( self );
        }

        /** simulate a fixed function pipeline - ref as return type doesn't work too well with smart-ptr */

        BufferedMesh* Reset();

        BufferedMesh* Begin( int type );

        void End(); // must end. No chaining

        void Compile();

        BufferedMesh* Material( const pei::Material& material );

        BufferedMesh* Vertex( const pei::Vector4f& v );

        BufferedMesh* Vertex2f( double x, double y );

        BufferedMesh* Vertex3f( double x, double y, double z );

        BufferedMesh* Vertex2fv( double v[2] );

        BufferedMesh* Vertex3fv( double v[3] );

        BufferedMesh* TexCoord2f( double u, double v );

        BufferedMesh* TexCoord3f( double u, double v, double w );

        BufferedMesh* TexCoord2fv( double v[2] );

        BufferedMesh* TexCoord3fv( double v[3] );

        BufferedMesh* Normal3f( double nx, double ny, double nz );

        BufferedMesh* Normal3fv( double nv[3] );

        BufferedMesh* Normal( const pei::Vector4f& vec );

        BufferedMesh* Color3f( float r, float g, float b );

        BufferedMesh* Color4f( float r, float g, float b, float a );

        BufferedMesh* Color3fv( float v[3] );

        BufferedMesh* Color4fv( float v[4] );

        BufferedMesh* Color( const pei::Color& col );

        BufferedMesh* Color( const pei::Vector4f& col );

    protected:
        void VerifyPolygonBounds( int n );
    };
} /* namespace pei */
#endif /* RENDER_MESH_H */
