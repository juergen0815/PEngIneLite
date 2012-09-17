/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <profile_config.h>

#include <geometry/mesh_target.h>
#include <geometry/mesh.h>

#include <render/render_profile.h>
#include <render/render_surface.h>
#include <render/render_param.h>
#include <render/tex_surface.h>

#include <iostream>
#include <algorithm>

#include <boost/bind.hpp>

namespace pei
{
    //////////////////////////////////////////////////////////////////////////
    // Vertex Arrays

    MeshRenderTarget::MeshRenderTarget()
    {
    }

    pei::SurfacePtr MeshRenderTarget::SetSurface( const SurfacePtr& surface, int idx /*= 0*/ )
    {
        // TODO: single texture ATM. Need a solution for multitexturing and bump maps, etc.
        //       Also, no cube maps, ...
        if ( surface ) {

            // see PSP implementation
        }
        return pei::SurfacePtr();
    }

    void MeshRenderTarget::AddMesh( const MeshPtr& mesh )
    {
        m_Meshes.push_back( mesh );
    }

    void MeshRenderTarget::Clear()
    {
        m_Meshes.clear();
    }

    void MeshRenderTarget::Render( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
    {
        std::for_each( m_Meshes.begin(), m_Meshes.end(),
                       boost::bind( &MeshRenderTarget::Render, this, profile, _1, buffer, param ));
    }

    // MeshTarget can either render its own list of meshes or an external mesh
    void MeshRenderTarget::Render( pei::RenderProfilePtr& profile, const MeshPtr& mesh, SurfacePtr& buffer, const RenderParam& param )
    {
#ifdef _HAS_OPENGL_
        // buffer must be VBO or NULL - TODO: support VBOs

        if ( mesh && mesh->GetSize() > 0 ) {
            const IndexArrayPtr& indexArray = mesh->GetIndices();
            const std::vector<float>& vertices = mesh->GetVertices();
            const std::vector<float>& normals  = mesh->GetNormals();

            bool skipRender(false);
            int renderMode = GetRenderMode();
            // default behaviour. Apply global color - material overrides
//            if ( ( renderMode & (EN_SKIP_MATERIAL_COLOR_F)) == 0) {
                const pei::Color& c = param.GetColor();
                if ( (( renderMode & (EN_ALPHA_F|EN_BLEND_F)) != 0)
                        && c.a < pei::Color::PIXF_OPAQUE ) {
                    if ( c.a == 0 ) {
                        skipRender = true;
                    } else {
                        glEnable(GL_BLEND);
                        glEnable( GL_ALPHA_TEST );
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        glColor4f( c.R(), c.G(), c.B(), c.A());
                    }
                } else {
                    // default color - will be overwritten by material, if any
                    if ((renderMode & EN_BLEND_F)!=0) glEnable(GL_BLEND);
                    glColor3f( c.R(), c.G(), c.B() );
                }
//            }
            if ( !skipRender ) {
                const std::vector<float>& texCoords = mesh->GetTexCoords();
                Texture2DSurfacePtr texture = boost::dynamic_pointer_cast<Texture2DSurface>(GetSurface(0));
                if ( texture != NULL && texCoords.size() > 0 /* &&
                     texture->GetWidth() >  0 && texture->GetHeight() > 0 */ )
                {
                    // texture & normals
                    texture->Enable();
                    DrawElements( indexArray, vertices, normals, texCoords, 2, mesh->GetPolygons() );
                    texture->Disable();
                } else if ( normals.size() ) {
                    // normals defined - but no texture
                    DrawElements( indexArray, vertices, normals, mesh->GetPolygons() );
                } else {
                    // no normals, no texture
                    DrawElements( indexArray, vertices, mesh->GetPolygons() );
                }
                // can be both, SOLID and WIRFRAME
                if ( (renderMode & EN_WIREFRAME_F) != 0 ) {
                    glPushAttrib( GL_POLYGON_BIT );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
                    DrawElements( indexArray, vertices, mesh->GetPolygons() );
                    glPopAttrib();
                }
            }
        }
#endif
    }

    void MeshRenderTarget::DrawElements( const IndexArrayPtr& indexArray,
                               const std::vector<float>& vertices,
                               const std::vector<float>& normals,
                               const std::vector<Polygon>& polys )
    {
#ifdef _HAS_OPENGL_
        glEnableClientState( GL_NORMAL_ARRAY);
        glNormalPointer( GL_FLOAT, 0, (const GLvoid*)(&normals.at(0)) );
        DrawElements( indexArray, vertices, polys );
        glDisableClientState( GL_NORMAL_ARRAY );
#endif
    }

    void MeshRenderTarget::DrawElements( const IndexArrayPtr& indexArray,
                                         const std::vector<float>& vertices,
                                         const std::vector<float>& normals,
                                         const std::vector<float>& texCoords,
                                         int textSize,
                                         const std::vector<Polygon>& polys )
    {
#ifdef _HAS_OPENGL_
        glEnableClientState( GL_TEXTURE_COORD_ARRAY);
        // TODO: vertex size is not static!
        glTexCoordPointer(textSize, GL_FLOAT, 0, (const GLvoid*)(&texCoords.at(0)));
        if ( normals.size() ) {
            DrawElements( indexArray, vertices, normals, polys );
        } else {
            DrawElements( indexArray, vertices, polys );
        }
        glDisableClientState( GL_TEXTURE_COORD_ARRAY);
#endif
    }

    void MeshRenderTarget::DrawElements( const IndexArrayPtr& indexArray,
                                         const std::vector<float>& vertices,
                                         const std::vector<Polygon>& polys )
    {
#ifdef _HAS_OPENGL_
        // no more checks here!!
        if ( polys.size() ) {
            bool skipRender(false);
            glEnableClientState( GL_VERTEX_ARRAY );
            // for 2D we won't need 3 components in the vertex
            glVertexPointer( 3, GL_FLOAT, 0, (const GLvoid*)(&vertices.at(0)) );
            std::vector<Polygon>::const_iterator pit = polys.begin();
            while ( pit != polys.end() ) {
                const Polygon& p = *pit++;
                // TODO: Apply Specular/Reflection
                Material* m = p.GetMaterial();
                int rm = (GetRenderMode() & ~EN_WIREFRAME_F);
                if ( m != NULL ) {
                    rm = m->GetRenderMode( rm );
                    if ( (rm & EN_SKIP_MATERIAL_COLOR_F) == 0 ) {
                        const pei::Vector4f& c = p.GetMaterial()->GetColor();
                        if ( (( rm & (EN_ALPHA_F|EN_BLEND_F)) != 0)
                                && c.w() < 1.0 ) {
                            if ( c.w() == 0 ) {
                                skipRender = true;
                            } else {
                                glEnable(GL_BLEND);
                                glEnable( GL_ALPHA_TEST );
                                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                                glColor4fv( c.asArray() );
                            }
                        } else {
                            // default color - will be overwritten by material, if any
                            glColor3fv( c.asArray() );
                        }
                    }
                }
                int len   = p.GetNumIndices();
                int start = p.GetOffset();
                if ( !skipRender && (start+len) <= indexArray->GetSize() ) {
                    GLvoid *indices = (GLvoid *)indexArray->Get(start);
                    // this is per material wireframe - not global
                    if ( (rm & EN_WIREFRAME_F) != 0 ) {
                        // push attrib is expensive!
                        glPushAttrib( GL_POLYGON_BIT );
                        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
                        glDrawElements( p.GetType(), len, indexArray->GetType(), indices );
                        glPopAttrib();
                    } else {
                        glDrawElements( p.GetType(), len, indexArray->GetType(), indices );
                    }
                }
            }
            glDisableClientState( GL_VERTEX_ARRAY );

        }
#endif
    }



} /* namespace pei */
