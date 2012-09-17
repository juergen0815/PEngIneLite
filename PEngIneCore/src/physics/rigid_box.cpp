/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include "rigid_box.h"

#include <geometry/mesh.h>

namespace pei {

    static unsigned char indexArray[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1 };

    RigidBox::RigidBox()
        : m_MeshTarget( new MeshRenderTarget )
    {
        // wireframe with global color only
        m_MeshTarget->SetRenderMode( pei::EN_WIREFRAME_F | pei::EN_SKIP_MATERIAL_COLOR_F );
    }

    RigidBox::~RigidBox()
    {
        m_MeshTarget->Clear();
    }

    void RigidBox::Update( const Bounds3D& bounds )
    {
        RigidBody::Update( bounds );
        // calc mesh for box
        m_MeshTarget->Clear();
#ifdef _HAS_OPENGL_
        MeshPtr mesh = Mesh::Create();
        // TODO: Make trianlge strips...
        std::vector<float> va = bounds.asQuadStrips();
        mesh->SetVertices( va.size()/3, &va[0] );
        mesh->SetIndices( 10, indexArray );
        Material mat;
        mat.SetColor( m_RenderColor );
        mesh->AddPolygon( GL_QUAD_STRIP, 10, 0, mat );
        m_MeshTarget->AddMesh( mesh );
#endif
    }

    void RigidBox::Render( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
    {
        // show the bounds
        if ( m_ShowBounds ) {
            RenderParam p;
            p.SetColor( m_RenderColor );
            m_MeshTarget->Render( profile, buffer, p );
        }
    }

} // namespace pei
