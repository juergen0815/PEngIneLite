/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_NODE_H
#define RENDER_NODE_H

#include <interfaces/render_node_interface.h>
#include <interfaces/render_target_interface.h>

#include <render/render_surface.h>
#include <render/render_profile.h>
#include <render/render_param.h>

#include <math/vector4f.h>

#include <vector>
#include <string>
#include <deque>
#include <list>

#include <boost/thread/locks.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

//#define USE_VECTOR

namespace pei
{
    class RigidBody;
    typedef boost::shared_ptr<RigidBody> RigidBodyPtr;

	class RenderNode;
	typedef boost::shared_ptr<RenderNode> RenderNodePtr;

    // TODO: Split RenderNode into Node and Entity. Node should not contain any data but tree functions only
    class RenderNode : public pei::RenderNodeInterface
    {
    protected:
#ifdef USE_VECTOR
    	typedef std::vector< RenderNodeInterfacePtr > RenderNodeList;
#else
    	typedef std::list< RenderNodeInterfacePtr > RenderNodeList;
#endif
    private:
        std::string m_Name;
    	// Create shared_ptr without getting deleted.
    	struct NullDeleter {
    		void operator()( void const* ) const
            {
            }
    	};
    protected:

    	// this is a manual deleter for "floating" nodes
    	struct ManualDeleter {
    	    // can only be used in the scope of the class, therefore must be of type RenderNode
            void operator()( pei::RenderNode *self ) const
            {
                if ( self->GetParent() ) {
                    self->GetParent()->RemoveChild( self->Get() );
                }
                // delete self; - we need to free memory here, huh???
            }
        };

    protected:
        boost::weak_ptr<RenderNodeInterface> m_WeakThis;

        pei::RenderParam 		 m_RenderParam;
        RenderTargetInterfacePtr m_RenderTarget;

        int                      m_Flags;
        double                   m_ZIndex; //!< zindex is an absolute z-order in the scene graph used to priorize events

        RenderNodeInterfacePtr   m_ParentNode;   //!< if this is a child, it has a m_ParentNode
        RenderNodeList           m_Children;     //!< m_Children to be drawn
        RenderNodeList           m_DelayRemoveList;     //!< children to be removed after draw cycle

        mutable RenderNodeList::const_iterator m_CurrentChild;

        // move into render manager - no object level locking needed (???)
        static boost::recursive_mutex  m_RenderLock;

        // a list of rigid bodies...usually one, more for subdivided meshes
        std::vector<RigidBodyPtr>      m_RigidBodies;
    public:
        #define LOCK_GUARD() boost::lock_guard< boost::recursive_mutex > lock( GetLock() );

        enum enFLAGS
        {
            EN_ACTIVE   = 1<<0,
            EN_VISIBLE  = 1<<1,
            EN_PAUSED   = 1<<2,
            EN_DELETE   = 1<<3, // object is ready to be deleted (remove from tree)
            EN_REDRAW   = 1<<4, // need to be redrawn
            EN_DELETE_CHILDREN = 1<<5, // If EN_DELETE is set, also delete all children
            EN_SKIP_CHILDREN= 1<<6,
            EN_UPDATE_STATE= 1<<7,
            EN_NO_AUTOSORT = 1<<8,
            EN_DRAW_BEFORE_CHILDREN = 1<<9,
            EN_DRAWABLE = ( EN_ACTIVE | EN_VISIBLE )
        };

        // clamp Z to +/-100 - use float numbers! (z is very exact - range of 100 per node is sufficient)
        enum enZRange {
            MAX_Z =  100,
            MIN_Z = -100,

            MAX_Z_RANGE = 200
        };

    public:
        // quick fix until we have named nodes
        virtual const char* GetName() const { return m_Name.c_str(); }

        virtual RenderNode& SetName( const char* name ) { m_Name = name; return *this; }

        virtual void LinkSmartPtr( RenderNodeInterfacePtr smart_this );

        virtual RenderNodeInterfacePtr Get();

        /** */
        RenderNode( );

        /** */
        RenderNode( const RenderParam& p, const SurfacePtr& s = SurfacePtr() );

        /** */
        RenderNode( const SurfacePtr& s );

        /** */
        virtual ~RenderNode();

        virtual void AddRigidBody( RigidBodyPtr rigidBody );

        virtual RigidBodyPtr GetRigidBody( unsigned int index = 0) const;

        //////////////////////////////////////////////////////////////////////////
        // render interface

        virtual void SetFlags( unsigned int flags ) { m_Flags |= flags; }

        virtual unsigned int GetFlags() const { return m_Flags; }

        /** activate: updated only */
        virtual RenderNode& Activate( bool active );

        /** visible: visible, drawn if active */
        virtual RenderNode& Show( bool visible );

        /** paused: active but not updated, still drawn */
        virtual RenderNode& Pause( bool pause );

        bool IsActive()  const { return (m_Flags & EN_ACTIVE)  != 0; }
        bool IsVisible() const { return (m_Flags & EN_VISIBLE) != 0; }
        bool IsPaused()  const { return (m_Flags & EN_PAUSED)  != 0; }

        virtual RenderNode& RequestStateChanged() { m_Flags |= EN_UPDATE_STATE; return *this; } ;

        virtual RenderNode& RequestStateChanged( bool doUpdate ) { m_Flags |= (doUpdate ? EN_UPDATE_STATE : 0); return *this; } ;

        virtual bool MustRefreshState() const { return (m_Flags & EN_UPDATE_STATE) != 0; }

        virtual RenderNode& RequestRedraw() { m_Flags |= EN_REDRAW; return *this; }

        virtual bool MustRedraw() const { return (m_Flags & EN_REDRAW) != 0; }

        virtual void AutoSortChildren( bool auto_sort = true ) { if (auto_sort ) m_Flags &= ~EN_NO_AUTOSORT; else m_Flags |= EN_NO_AUTOSORT; }

        virtual bool IsAutoSortEnabled() { return (m_Flags & EN_NO_AUTOSORT) == 0; };

        virtual void DrawBeforeChildren( bool before = true ) { m_Flags |= EN_DRAW_BEFORE_CHILDREN; }

        virtual bool DrawBeforeChildren() const { return (m_Flags & EN_DRAW_BEFORE_CHILDREN) != 0; }

        // default can render only render one pass - a standard geom/texture pass
        virtual bool CanRender( int renderPass ) { return (renderPass & m_RenderParam.GetRenderPass()) != 0; }

        virtual void EnableRenderPass( int renderPass ) { m_RenderParam.SetRenderPass( m_RenderParam.GetRenderPass() | renderPass ); }

        virtual void DisableRenderPass( int renderPass ) { m_RenderParam.SetRenderPass( m_RenderParam.GetRenderPass() & ~renderPass ); }

        virtual void EnableRenderMode( int renderMode ) { GetRenderTarget()->SetRenderMode( GetRenderTarget()->GetRenderMode() | renderMode ); }

        virtual void DisableRenderMode( int renderMode ) { GetRenderTarget()->SetRenderMode( GetRenderTarget()->GetRenderMode() & ~renderMode ); }

        ////////////////////////////////////////////////////////////////

        virtual SurfacePtr SetSurface( const SurfacePtr& s, int n = 0 );

        virtual SurfacePtr GetSurface( int n = 0) const;

    	virtual void SetRenderTarget( RenderTargetInterfacePtr rti );

    	virtual RenderTargetInterfacePtr GetRenderTarget() const { return m_RenderTarget; }

        virtual bool Init( RenderProfilePtr& profile ) ;

        /** non virtual methods! */
        virtual int Draw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );

        /** */
        virtual void Update( RenderProfilePtr& profile, double time, int parent_flags = 0, double zindex = 0 );

        virtual double GetZIndex( ) const { return m_ZIndex; };

        virtual void Lock();

        virtual bool TryLock();

        virtual void Unlock();

        boost::recursive_mutex& GetLock() const;

        /** */
        virtual RenderNode& SetPosition( double x, double y ) {
        	RequestStateChanged((m_RenderParam.GetX() != x) || (m_RenderParam.GetY() != y));
        	m_RenderParam.SetX(x).SetY(y);
            return *this;
        }

        virtual RenderNode& SetPosition( double x, double y, double z ) {
        	RequestStateChanged((m_RenderParam.GetX() != x) || (m_RenderParam.GetY() != y) || (m_RenderParam.GetZ() != z) );
            if ( z > MAX_Z )     z = MAX_Z;
            else if (z < MIN_Z ) z = MIN_Z;
        	m_RenderParam.SetX(x).SetY(y).SetZ(z);
            return *this;
        }

        /** */
        virtual RenderNode& SetXPosition( double x ) {
        	RequestStateChanged(m_RenderParam.GetX() != x);
        	m_RenderParam.SetX(x);
            return *this;
        }

        virtual RenderNode& SetYPosition( double y ) {
        	RequestStateChanged(m_RenderParam.GetY() != y);
        	m_RenderParam.SetY(y);
            return *this;
        }

        /** */
        virtual RenderNode& SetZPosition( double z, bool resort = true ) {
        	RequestStateChanged(m_RenderParam.GetZ() != z);
        	// clamp z range!
        	if ( z > MAX_Z )     z = MAX_Z;
        	else if (z < MIN_Z ) z = MIN_Z;
            m_RenderParam.SetZ( z );
            if ( resort && IsAutoSortEnabled() && m_ParentNode ) {
                m_ParentNode->ZSort();
            }
            return *this;
        }

        /** */
        double GetXPosition() const {
            return m_RenderParam.GetX();
        }
        /** */
        double GetYPosition() const {
            return m_RenderParam.GetY();
        }

        /** */
        double GetZPosition() const {
            return m_RenderParam.GetZ();
        }

        virtual RenderNode& SetSize( double w, double h );

        //! Convenience overload - no position!
        inline  RenderNode& SetSize( const pei::Rectangle& rect ) { return SetSize(rect.GetWidth(),rect.GetHeight()); }

        virtual RenderNode& SetWidth( double w );

        virtual RenderNode& SetHeight( double h );

        /** */
        virtual double GetWidth() const;

        /** */
        virtual double GetHeight() const;

        //! Convenience overload - no position!
        inline pei::Rectangle GetSize() const { return pei::Rectangle(GetWidth(),GetHeight()); }

        virtual RenderNode& SetScale( double sx, double sy, double sz = 1.0 );

        virtual RenderNode& SetXScale( double s ) { RequestStateChanged(m_RenderParam.GetXScale() != s); m_RenderParam.SetXScale( s ); return *this; }

        virtual RenderNode& SetYScale( double s ) { RequestStateChanged(m_RenderParam.GetYScale() != s); m_RenderParam.SetYScale( s ); return *this; }

        virtual RenderNode& SetZScale( double s ) { RequestStateChanged(m_RenderParam.GetZScale() != s); m_RenderParam.SetZScale( s ); return *this; }

        virtual double GetXScale() const { return m_RenderParam.GetXScale(); }

        virtual double GetYScale() const { return m_RenderParam.GetYScale(); }

        virtual double GetZScale() const { return m_RenderParam.GetZScale(); }

        virtual RenderNode& SetRotation( double xr, double yr, double zr );

        virtual RenderNode& SetXRotation( double a ) { RequestStateChanged(m_RenderParam.GetXRotation() != a); m_RenderParam.SetXRotation( a ); return *this; }

        virtual RenderNode& SetYRotation( double a ) { RequestStateChanged(m_RenderParam.GetYRotation() != a); m_RenderParam.SetYRotation( a ); return *this; }

        virtual RenderNode& SetZRotation( double a ) { RequestStateChanged(m_RenderParam.GetZRotation() != a); m_RenderParam.SetZRotation( a ); return *this; }

        virtual double GetXRotation( ) const { return m_RenderParam.GetXRotation(); }

        virtual double GetYRotation( ) const { return m_RenderParam.GetYRotation(); }

        virtual double GetZRotation( ) const { return m_RenderParam.GetZRotation(); }

        /** */
        virtual RenderNode& SetAlpha( double level );

        virtual double GetAlpha( ) const { return m_RenderParam.GetAlpha(); }

        virtual RenderNode& SetColor( const pei::Color& c ) { m_RenderParam.SetColor(c); return *this; }

        virtual const pei::Color& GetColor() const { return m_RenderParam.GetColor(); }

        virtual RenderNode& SetRenderParam( const pei::RenderParam& p );

        virtual const pei::RenderParam& GetRenderParam() const { return m_RenderParam; }

        virtual pei::RenderParam& GetRenderParam()  { return m_RenderParam; }

        virtual bool IsInside( double x, double y ) const;

        //////////////////////////////////////////////////////////////////////////
        // node interface - will need a complete node interface implementation!

        RenderNodeInterfacePtr GetRoot() const;

        RenderNodeInterfacePtr GetParent() const { return m_ParentNode; }

        /** */
        virtual RenderNodeInterfacePtr AddChild( RenderNodeInterfacePtr ro );

        /** */
        virtual RenderNodeInterfacePtr AddChild( RenderNodeInterfacePtr ro, double z );

        /*! Mark a child for deletion. Removal is performed after one update cycle.
         *
         *  @param ro the render node to remove from the tree
         *  @param remove_children set to true if a whole subtree should be removed
         *  @return void
         */
        virtual void Remove( bool remove_children = false );

        /** */
        virtual RenderNodeInterfacePtr RemoveChild( RenderNodeInterfacePtr ro );

        /** */
        virtual void RemoveAllChildren( );

        virtual RenderNodeInterfacePtr FindChild( RenderNodeInterfacePtr ro ) const;

        virtual RenderNodeInterfacePtr FindChild( const char* name = NULL ) const;

        virtual RenderNodeInterfacePtr FindNextChild( const char* name ) const;

        virtual size_t GetNumberOfChildren() const;

        /* increment/decrement ops are const because they don't really increment the node, but rather its children! */

        virtual RenderNodeInterfacePtr& operator++(int) const;

        virtual RenderNodeInterfacePtr& operator--(int) const;

        virtual RenderNodeInterfacePtr& operator++() const;

        virtual RenderNodeInterfacePtr& operator--() const;

        virtual RenderNodeInterfacePtr& begin() const;

        virtual RenderNodeInterfacePtr& end() const;

        virtual RenderNodeInterfacePtr NextChild() const;

        virtual RenderNodeInterfacePtr PreviousChild() const;

        virtual bool Up( RenderNodeInterfacePtr rni );

        virtual bool Down( RenderNodeInterfacePtr rni );

        virtual bool Front( RenderNodeInterfacePtr rni );

        virtual bool Back( RenderNodeInterfacePtr rni );

        virtual bool Swap( RenderNodeInterfacePtr& a, RenderNodeInterfacePtr& b );

#ifdef USE_VECTOR
        virtual RenderNodeInterfacePtr GetChild( int num ) const;
#endif
        /** */
        virtual void ZSort( bool sort_children = false );


    protected:
        virtual void SetParent( RenderNodeInterfacePtr parent );

        /** (optional) initialization - required for 3D engine */
        virtual bool OnInit( RenderProfilePtr& profile ) ;

        /** update internal states - (called even if paused) */
        virtual void OnUpdateState( RenderProfilePtr& profile );
        /** update time dependent animation states (not called if paused) */
        virtual void OnUpdateAnimation( RenderProfilePtr& profile, double time );

        virtual SurfacePtr OnPreDraw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam()  );

        /** Functional imp for RemoveChild() but no locking, e.g. within drawloop */
        virtual RenderNodeInterfacePtr OnRemoveChild( RenderNodeInterfacePtr ro );

        /** Functional imp for OnRemoveAllChildren but no locking, e.g. to clean up within drawloop*/
        virtual void OnRemoveAllChildren( );

        /** */
        virtual void OnDraw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam()  );

        virtual void Render( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );

    public: // this should posible not be a member function - ??
        virtual RenderNode& TranslateFromScreenCoords( double &x, double &y );

        virtual RenderNode& TranslateToScreenCoords( double &x, double &y, double &w, double &h );

        virtual pei::Vector4f TranslateFromScreenCoords( );

        virtual pei::Vector4f TranslateToScreenCoords( );

    };

}; // namespace pei;

#endif // RENDER_NODE_H
