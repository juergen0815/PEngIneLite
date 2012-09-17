/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_TARGET_INTERFACE_H
#define RENDER_TARGET_INTERFACE_H

#include <boost/shared_ptr.hpp>

namespace pei {
	class Surface;
	typedef boost::shared_ptr<Surface> SurfacePtr;

	class RenderProfile;
	typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

	class RenderParam;

	class RenderTargetInterface;
	typedef boost::shared_ptr<RenderTargetInterface> RenderTargetInterfacePtr;

    enum enRenderModeBits {
        EN_ALPHA_B = 0,
        EN_BLEND_B,
        EN_WIREFRAME_B,

        // If culling is enabled, control type of face to render
        EN_FRONT_FACE_B,
        EN_BACK_FACE_B,

        EN_CREATE_MIP_MAPS_B, // needs to be enabled explicitly - not e.g. for animations
        EN_DISABLE_TEXTURE_B,

        // not all targets support all texture edge modes
        EN_TEXTURE_CLAMP_B,
        EN_TEXTURE_REPEAT_B,
        EN_TEXTURE_CLAMP_EDGE_B,
        EN_TEXTURE_CLAMP_BORDER_B,

        EN_TEXTURE_FORCE_POW2_B,    // force texture creation to pow2 sizes

        EN_CUSTOM_B = 20    // 12 custom render modes for your render target
    };
    enum enRenderMode {
        EN_ALPHA_F      = 1 << EN_ALPHA_B,
        EN_BLEND_F      = 1 << EN_BLEND_B,
        EN_WIREFRAME_F  = 1 << EN_WIREFRAME_B,

        // if culling is enabled, define which face to render
        EN_FRONT_FACE_F      = 1 << EN_FRONT_FACE_B,
        EN_BACK_FACE_F       = 1 << EN_BACK_FACE_B,

        EN_CREATE_MIP_MAPS_F = 1 << EN_CREATE_MIP_MAPS_B,
        EN_DISABLE_TEXTURE_F = 1 << EN_DISABLE_TEXTURE_B,

        // not all targets support all texture edge modes
        EN_TEXTURE_CLAMP_F        = 1 << EN_TEXTURE_CLAMP_B,
        EN_TEXTURE_REPEAT_F       = 1 << EN_TEXTURE_REPEAT_B,
        EN_TEXTURE_CLAMP_EDGE_F   = 1 << EN_TEXTURE_CLAMP_EDGE_B,
        EN_TEXTURE_CLAMP_BORDER_F = 1 << EN_TEXTURE_CLAMP_BORDER_B,

        EN_TEXTURE_MODE_MASK = EN_TEXTURE_CLAMP_F|EN_TEXTURE_REPEAT_F|EN_TEXTURE_CLAMP_EDGE_F|EN_TEXTURE_CLAMP_BORDER_F,

        EN_TEXTURE_FORCE_POW2_F = 1 << EN_TEXTURE_FORCE_POW2_B,    // force texture creation to pow2 sizes
    };

	class RenderTargetInterface
	{
	public:
		virtual ~RenderTargetInterface() {}

		virtual pei::RenderTargetInterfacePtr Create() const = 0;

		virtual pei::SurfacePtr SetSurface( const pei::SurfacePtr& s, int idx = 0 ) = 0;

		virtual pei::SurfacePtr GetSurface( int idx = 0 ) const = 0;

		virtual void SetRenderMode( int renderMode ) = 0;

		virtual int GetRenderMode() const = 0;

        /** this renders into a given buffer - in 2D this is the screen surface, or any other
            local buffer a node allocates. In 3D this is either the screen, or an offscreen buffer,
            or a clipping area (e.g. siccor/stencil test only. */
		virtual void Render( pei::RenderProfilePtr& profile, pei::SurfacePtr& buffer, const pei::RenderParam& param ) = 0;

		/*!
		 * Pre-Render pass. E.g. set up shaders, etc
		 * @param profile
		 */
        virtual void PreRender( pei::RenderProfilePtr& profile, const pei::RenderParam& param ) = 0;

        /*!
         *  Clear up render target, e.g. disable shader program
         * @param profile
         */
        virtual void PostRender( pei::RenderProfilePtr& profile, const pei::RenderParam& param ) = 0;

	};
} // namespace pei

#endif /* RENDER_TARGET_INTERFACE_H */
