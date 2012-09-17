/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <render/render_param.h>
#include <render/color.h>

#include <math/vector4f.h>
#include <math/bounds3d.h>

#include <boost/shared_ptr.hpp>

namespace pei {
    class RenderProfile;
    typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

    class Surface;
    typedef boost::shared_ptr<Surface> SurfacePtr;

    class RigidBody;
    typedef boost::shared_ptr<RigidBody> RigidBodyPtr;

    class RigidBody
    {
    protected:
        enum Type {
            BOX,
            SPHERE
        };
        RenderParam* m_RenderParamRef;  //!< shared with RenderNode!
        Bounds3D     m_Bounds;          //!< Object bounds as a cube

        //! Some render flags to allow rigid bodies to be drawn
        int      m_Type;                //!< Either a box or a sphere, default = BOX;
        Color    m_RenderColor;         //!< Outline color to render with
        bool     m_ShowBounds;          //!< flag to enable/disable drawing

        float    m_MinRadius;           //!< cache to "radius" that is dist in bounds from (vertex-min,vertex-max)/2

        bool     m_HasChanged;          //!< flag to notify a change - e.g. RigidBody was manipulated but triggers a state change in the attached object
    public:
        /*!
         *
         */
        RigidBody();

        /*!
         *
         */
        virtual ~RigidBody();

        /*!
         * Init the rigid body. Set the render parameter and calc bounds
         * @param param a render param to attach to this rigid body
         */
        virtual void SetRenderParam( RenderParam* param );

        /*!
         * Report if a rigid body has changed - e.g. position changed with calling the attached RenderNode
         * @return bool if the position/rotation/scale has changed
         */
        virtual bool HasChanged() const;

        /*!
         * Set the position - this sets the position of the attached RenderNode through the linked RenderParam
         * @param pos
         */
        virtual void SetPosition( const Vector4f& pos );

        /*!
         * Read the position from the attached render param
         * @return a vector containing position data
         */
        virtual Vector4f GetPosition() const;

        /*!
         * Rotate the object through rotation data in the attached RenderParam
         * @param rot 3 component x,y,z rotation angles (in degrees)
         */
        virtual void Rotate( const Vector4f& rot );

        /*!
         * Return the rotation vector as a 3 component angle vector
         * @return Vector containing x,y,z rotation angles (in degree)
         */
        virtual Vector4f GetRotation() const;

        /*!
         * Scale the object through its attached RenderParam
         * @param scale a 3 component vector containing x,y,z scales (0.0 ... 1.0)
         */
        virtual void Scale( const Vector4f& scale );

        /*!
         * Return the scale of this object as a 3 component vector (x,y,z)
         * @return Vector with scale data (x,y,z (0.0...1.0)
         */
        virtual Vector4f GetScale() const;

        /*!
         * Bounds of the rigid body as a cuboid object
         * @return Bounds of the object
         */
        virtual const Bounds3D& GetBounds() const;

        /*!
         * Radius of the object. This might differ from the Bounds::Radius in the sense these is a real vertex radius,
         * Bounds is always calculated from center of cube
         * @return Radius of object (that is, longest distance from center (RenderParam::m_Origin) to the point farthest away).
         */
        virtual float GetRadius() const;

        virtual void ShowBounds( bool showBounds );

        virtual bool ShowBounds() const;

        virtual void SetColor( const Color& col );

        /*!
         * Call to recalc bounds in case RenderNode updates RenderParam (usually in OnUpdateState()
         * when a RequestStateChange() has been initiated)
         */
        virtual void Update( const Bounds3D& bounds );

        /*!
         * Render outline of rigid body (based on Bounds3D)
         */
        virtual void Render( RenderProfilePtr profile, SurfacePtr buffer, const RenderParam& param );
    };

} // namespace pei

#endif
