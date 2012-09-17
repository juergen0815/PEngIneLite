/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <render/render_target.h>
#include <render/color.h>

#include <math/vector4f.h>

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

    //////////////////////////////////////////////////////////////////////////

	class Material
	{
	protected:
        pei::Vector4f m_Color;
        pei::Vector4f m_Ambient; // (col*0.3)
        pei::Vector4f m_Diffuse; // (col*0.7)
        pei::Vector4f m_Specular;// (col*0.2)
        float         m_Shininess; //0.5
        int           m_RenderMode;
        unsigned int  m_ParentRenderModeMask; // mask out parent render mode - global mode
	public:
        // TODO: Give me some attributes!

        virtual ~Material() {}

        Material( );

        Material( const Material& material );

        Material* SetColor( const pei::Vector4f& col );

        Material* SetColor( const pei::Color& col );

        Material* SetColor3f( float r, float g, float b );

        Material* SetColor4f( float r, float g, float b, float a );

        Material* SetColor3fv( const float v[3] );

        Material* SetColor4fv( const float v[4] );

        const pei::Vector4f& GetColor() const;

        /** always override parent mode */
        Material* SetRenderMode( int rm, unsigned int parentMask = 0 );

        /** default return local only */
        int GetRenderMode( int parent = 0 ) const;

        bool operator==( const Material& m ) {
            return m_Color == m.GetColor() &&
                   m_RenderMode == m.GetRenderMode(0);
        }
	};

	class IndexArray
	{
	protected:
        void *m_Indices;

        int   m_NumIncides;
        int   m_IndexType; // GL_UNSIGNED_BYTE
        int   m_IndexStride;

    public:
        IndexArray( int num, const unsigned char* indices );
        IndexArray( int num, const unsigned short* indices );
        IndexArray( int num, const unsigned int* indices );

        IndexArray( const IndexArray& );

        virtual ~IndexArray();

        inline const void* Get( int offset ) const
        {
            return (const void*)(((const char*)m_Indices) + offset*m_IndexStride);
        }

        inline int GetSize()   { return m_NumIncides; }

        inline int GetType()   { return m_IndexType; }

        inline int GetStride() { return m_IndexStride; }
	};
	typedef boost::shared_ptr<IndexArray> IndexArrayPtr;

    class Polygon
    {
    protected:
        int         m_Type;         // e.g. GL.GL_TRIANGLES, GL.GL_QUADS
        int         m_NumIndices;   // num vertices in face (total - spawning multiple quads, for example)
        int         m_Offset;       // offset from from start of array

        Material   *m_Material;     // Material for this polygon or NULL - is a pointer because can be NULL checked
    public:
        Polygon( const Polygon& p)
        : m_Type(p.m_Type), m_NumIndices(p.m_NumIndices), m_Offset(p.m_Offset), m_Material(p.m_Material ? new Material(*p.m_Material) : NULL ) {}

        Polygon( int t = 0, int n = 0, int o = 0 )
        : m_Type(t), m_NumIndices(n), m_Offset(o), m_Material(NULL) {}

        Polygon( int t, int n, int o, const Material& m)
        : m_Type(t), m_NumIndices(n), m_Offset(o), m_Material( new Material(m) ) {}

        virtual ~Polygon() { if (m_Material) delete m_Material; }

        void SetType( int t ) { m_Type = t; }

        int GetType() const { return m_Type; }

        void SetIndices( int n = 0 ) { m_NumIndices = n; }

        int GetNumIndices() const { return m_NumIndices; }

        void SetOffset( int n ) { m_Offset = n; }

        int GetOffset() const { return m_Offset; }

        void SetMaterial( const Material& m ) {
            if ( m_Material ) delete m_Material;
            m_Material = new Material(m);
        }

        Material *GetMaterial() const { return m_Material; }

        bool HasMaterial( const Material& material )
        { return m_Material == NULL ? false : (*m_Material == material); }

        void AddIndices( int n ) { m_NumIndices += n; }

        int operator++( int ) { m_NumIndices++; return m_NumIndices; }

        int size();
    };

	class Mesh
    {
    protected:

    protected:
        boost::weak_ptr<Mesh>      m_WeakSelf;

        std::vector<Polygon>       m_Polygons;
        std::vector<float>         m_Vertices;
        std::vector<float>         m_Normals;
        std::vector<float>         m_TextureCoords;

        IndexArrayPtr              m_Indices;

        MeshPtr& LinkSmartPtr( MeshPtr& self ) {
            m_WeakSelf = self;
            return self;
        }
        Mesh();
    public:
        virtual ~Mesh() {}

        static MeshPtr Create() {
            pei::MeshPtr self( new Mesh() );
            return self->LinkSmartPtr( self );
        }

        virtual int GetSize() const;

        virtual void SetVertices( int num, const float* vertices );

        virtual const std::vector<float>& GetVertices( ) const;

        virtual void SetNormals( int num, const float* normals );

        virtual const std::vector<float>& GetNormals() const;

        virtual void SetTexCoords( int num, const float* texCoords );

        virtual const std::vector<float>& GetTexCoords() const;

        virtual void SetIndices( int num, const unsigned char* indices );

        virtual void SetIndices( int num, const unsigned short* indices );

        virtual void SetIndices( int num, const unsigned int* indices );

        // short lifetime access!! e.g. by renderer
        virtual const IndexArrayPtr& GetIndices( ) const;

        virtual bool AddPolygon( const Polygon& poly );

        virtual bool AddPolygon( int type, int num, int offset );

        virtual bool AddPolygon( int type, int num, int offset, const Material& material );

        virtual const std::vector<Polygon>& GetPolygons() const;

    protected:
        virtual bool AddPolygon( int type, int num, int offset, const Material* material );
    };
} /* namespace pei */
#endif /* RENDER_MESH_H */
