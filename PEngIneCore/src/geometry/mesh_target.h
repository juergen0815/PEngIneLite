/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef MESH_RENDER_TARGET_H
#define MESH_RENDER_TARGET_H

#include <render/render_target.h>
#include <render/color.h>

#include <geometry/mesh.h>

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace pei
{
	class Surface;
	typedef boost::shared_ptr<Surface> SurfacePtr;

	class RenderProfile;
	typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

	class RenderParam;

    class Mesh;
    typedef boost::shared_ptr<Mesh> MeshPtr;

    class MeshRenderTarget;
    typedef boost::shared_ptr<MeshRenderTarget> MeshRenderTargetPtr;

    //////////////////////////////////////////////////////////////////////////
    enum enCustomModeBits {
        EN_SKIP_MATERIAL_COLOR_B = EN_CUSTOM_B,
    };
    enum enCustomRenderMode {
        EN_SKIP_MATERIAL_COLOR_F = 1 << EN_SKIP_MATERIAL_COLOR_B,
    };

    // derive from VertexArrayTarget...??
    class MeshRenderTarget : public BufferedRenderTarget
    {
    protected:
        boost::weak_ptr<RenderTargetInterface> m_WeakSelf;

        std::vector< MeshPtr >  m_Meshes;
    public:
        MeshRenderTarget();
        virtual ~MeshRenderTarget() {}

        pei::RenderTargetInterfacePtr LinkSmartSelf( pei::RenderTargetInterfacePtr self ) {
            m_WeakSelf = self;
            return self;
        }

        virtual RenderTargetInterfacePtr Create() const {
            pei::RenderTargetInterfacePtr self( new MeshRenderTarget() );
            // need to get call proper type
            return static_cast<MeshRenderTarget*>(self.get())->LinkSmartSelf( self );
        }

        virtual unsigned int size() { return m_Meshes.size(); }

        virtual MeshPtr operator[]( unsigned int n ) { return ( n < size() ) ? m_Meshes[n] : MeshPtr(); }

        virtual void AddMesh( const MeshPtr& mesh );

        virtual void Clear();

        virtual pei::SurfacePtr SetSurface( const SurfacePtr& s, int idx = 0 );

        virtual void Render( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );

        // MeshTarget can either render its own list of meshes or an external mesh
        virtual void Render( pei::RenderProfilePtr& profile, const MeshPtr& mesh, SurfacePtr& buffer, const RenderParam& param );
    protected:

        virtual void DrawElements( const IndexArrayPtr& indexArray,
                                   const std::vector<float>& vertices,
                                   const std::vector<Polygon>& polys );

        virtual void DrawElements( const IndexArrayPtr& indexArray,
                                   const std::vector<float>& vertices,
                                   const std::vector<float>& normals,
                                   const std::vector<Polygon>& polys );

        virtual void DrawElements( const IndexArrayPtr& indexArray,
                                   const std::vector<float>& vertices,
                                   const std::vector<float>& normals,
                                   const std::vector<float>& texCoords,
                                   int textSize,
                                   const std::vector<Polygon>& polys );

    };
} /* namespace pei */
#endif /* MESH_RENDER_TARGET_H */
