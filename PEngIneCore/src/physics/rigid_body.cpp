/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include "rigid_body.h"

namespace pei {

    /*!
     *
     */
    RigidBody::RigidBody()
    {
    }

    /*!
     *
     */
    RigidBody::~RigidBody()
    {
    }

    /*!
     * Init the rigid body. Set the render parameter and calc bounds
     * @param param a render param to attach to this rigid body
     */
    void RigidBody::SetRenderParam( RenderParam* param )
    {
        m_RenderParamRef = param;
        if ( param ) {
            // default bounds is a plane
            Vector4f min( 0, 0, 0 );
            Vector4f max( param->GetWidth(), param->GetHeight(), param->GetDepth() );
            m_Bounds = Bounds3D( min, max );
            // hm...not necessarily. This is the center of gravity
            // m_Bounds.SetCenter( param->GetOrigin() );
        }
        m_HasChanged = true;
    }

    /*!
     * Report if a rigid body has changed - e.g. position changed with calling the attached RenderNode
     * @return bool if the position/rotation/scale has changed
     */
    bool RigidBody::HasChanged() const
    {
        return m_HasChanged;
    }

    /*!
     * Set the position - this sets the position of the attached RenderNode through the linked RenderParam
     * @param pos
     */
    void RigidBody::SetPosition( const Vector4f& pos )
    {
        if ( m_RenderParamRef ) {
            m_HasChanged = true;
            m_RenderParamRef->SetPosition( pos );
        }
    }

    /*!
     * Read the position from the attached render param
     * @return a vector containing position data
     */
    Vector4f RigidBody::GetPosition() const
    {
        if ( m_RenderParamRef ) {
            return m_RenderParamRef->GetPosition();
        }
        return Vector4f();
    }

    /*!
     * Rotate the object through rotation data in the attached RenderParam
     * @param rot 3 component x,y,z rotation angles (in degrees)
     */
    void RigidBody::Rotate(  const Vector4f& rot )
    {
        if ( m_RenderParamRef ) {
            m_HasChanged = true;
            m_RenderParamRef->SetRotation( rot );
        }
    }

    /*!
     * Return the rotation vector as a 3 component angle vector
     * @return Vector containing x,y,z rotation angles (in degree)
     */
    Vector4f RigidBody::GetRotation() const
    {
        if ( m_RenderParamRef ) {
            return m_RenderParamRef->GetRotation();
        }
        return Vector4f();
    }

    /*!
     * Scale the object through its attached RenderParam
     * @param scale a 3 component vector containing x,y,z scales (0.0 ... 1.0)
     */
    void RigidBody::Scale( const Vector4f& scale )
    {
        if ( m_RenderParamRef ) {
            m_HasChanged = true;
            m_RenderParamRef->SetScale( scale );
        }
    }

    /*!
     * Return the scale of this object as a 3 component vector (x,y,z)
     * @return Vector with scale data (x,y,z (0.0...1.0)
     */
    Vector4f RigidBody::GetScale() const
    {
        if ( m_RenderParamRef ) {
            return m_RenderParamRef->GetScale();
        }
        return Vector4f();
    }

    /*!
     * Bounds of the rigid body as a cuboid object
     * @return Bounds of the object
     */
    const Bounds3D& RigidBody::GetBounds() const
    {
        return m_Bounds;
    }

    /*!
     * Radius of the object. This might differ from the Bounds::Radius in the sense these is a real vertex radius,
     * Bounds is always calculated from center of cube
     * @return Radius of object (that is, longest distance from center (RenderParam::m_Origin) to the point farthest away).
     */
    float RigidBody::GetRadius() const
    {
        return 0;
    }

    void RigidBody::ShowBounds( bool showBounds )
    {
        m_ShowBounds = showBounds;
    }

    bool RigidBody::ShowBounds() const {
        return m_ShowBounds;
    }

    void RigidBody::SetColor( const Color& col )
    {
        m_RenderColor = col;
    }

    /*!
     * Call to recalc bounds in case RenderNode updates RenderParam (usually in OnUpdateState()
     * when a RequestStateChange() has been initiated)
     */
    void RigidBody::Update( const Bounds3D& bounds )
    {
        // this is something we have to store here. Note for later use to be able to do reverse projection for collision, etc.
//        GLdouble modelview[16];
//        GLdouble projection[16];
//        GLint    viewport[4]; // x,y,w,h
//
//        glGetIntegerv(GL_VIEWPORT, viewport);
//        glGetDoublev (GL_MODELVIEW_MATRIX, modelview);
//        glGetDoublev (GL_PROJECTION_MATRIX, projection);

        m_Bounds = bounds;
    }

    /*!
     * Render outline of rigid body (based on Bounds3D)
     */
    void RigidBody::Render( RenderProfilePtr profile, SurfacePtr buffer, const RenderParam& param )
    {
        // must be implemeted explicitly -RigidBody does not know how to render, can be box, sphere or any other abstract shape
    }

} // namespace pei
