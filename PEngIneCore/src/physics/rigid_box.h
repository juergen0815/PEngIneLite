/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RIGID_BOX_H
#define RIGID_BOX_H

#include <physics/rigid_body.h>

#include <geometry/mesh_target.h>

namespace pei {

    class RigidBox : public RigidBody
    {
        MeshRenderTargetPtr m_MeshTarget;
    public:
        RigidBox();

        ~RigidBox();

        /*!
         * Call to recalc bounds in case RenderNode updates RenderParam (usually in OnUpdateState()
         * when a RequestStateChange() has been initiated)
         */
        virtual void Update( const Bounds3D& bounds );

        /*!
         * Render outline of rigid body (based on Bounds3D)
         */
        virtual void Render( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );
    };

} // namespace pei

#endif // RIGID_BOX_H
