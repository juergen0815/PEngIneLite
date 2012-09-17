/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <render/render_node.h>
#include <render/render_target.h>

#include <physics/rigid_body.h>

#include <algorithm>

#include <boost/bind.hpp>

using namespace pei;

boost::recursive_mutex RenderNode::m_RenderLock;

static bool CompareRenderNodePtrFunc ( const pei::RenderNodeInterfacePtr& a, const pei::RenderNodeInterfacePtr& b ) {
	return a->GetZPosition() < b->GetZPosition();
}

struct nameCompare
{
    const std::string name;
public:
    nameCompare( const char *n ) : name( n ) {}

    bool operator( ) ( pei::RenderNodeInterfacePtr const a )
    {
        return strcmp( name.c_str(), a->GetName() ) == 0;
    }
};

RenderNode::RenderNode( )
    : m_Name( "pei::RenderNode" )
    , m_Flags(EN_DRAWABLE)
{
    m_CurrentChild = m_Children.begin();
    // default render target uses backbuffer surface to draw - no 3D!!
    m_RenderTarget = RenderProfile::GetDefaultRenderTarget();
}

RenderNode::RenderNode( const RenderParam& p, const SurfacePtr& s /*= SurfacePtr()*/ )
    : m_Name( "pei::RenderNode" )
    , m_RenderParam(p)
    , m_Flags(EN_DRAWABLE)
{
    m_CurrentChild = m_Children.begin();
    // default render target uses backbuffer surface to draw - no 3D!!
    m_RenderTarget = RenderProfile::GetDefaultRenderTarget();
    if ( m_RenderTarget ) {
        m_RenderTarget->SetSurface( s );
    }
}

RenderNode::RenderNode( const SurfacePtr& s )
    : m_Name( "pei::RenderNode" )
    , m_Flags(EN_DRAWABLE)
{
    m_CurrentChild = m_Children.begin();
    // default render target uses backbuffer surface to draw - no 3D!!
    m_RenderTarget = RenderProfile::GetDefaultRenderTarget();
    if ( m_RenderTarget ) {
        m_RenderTarget->SetSurface( s );
    }
}

RenderNode::~RenderNode()
{
}

void RenderNode::LinkSmartPtr( RenderNodeInterfacePtr smart_this ) {
    // must be self!!
    assert ( smart_this.get() == this );
    m_WeakThis = smart_this;
}

pei::RenderNodeInterfacePtr RenderNode::Get()
{
    RenderNodeInterfacePtr smartThis;
    if ( m_WeakThis.expired() ) {
        // we create a temporary smart ptr. As soon as this is linked into the scene graph, this pointer will become valid
        smartThis = RenderNodeInterfacePtr( this, NullDeleter() );
        LinkSmartPtr( smartThis );
    } else {
        smartThis = RenderNodeInterfacePtr( m_WeakThis );
    }
    return smartThis;
}

void RenderNode::AddRigidBody( RigidBodyPtr rigidBody )
{
    // link to render param - note that all rigid bodies share same render param! They might differ in their bounds (sub section collision, for example)
    rigidBody->SetRenderParam( &m_RenderParam );
    m_RigidBodies.push_back( rigidBody );
}

RigidBodyPtr RenderNode::GetRigidBody( unsigned int index /* = 0 */) const
{
    if ( index < m_RigidBodies.size() ) {
        m_RigidBodies[index];
    }
    return RigidBodyPtr();
}

SurfacePtr RenderNode::SetSurface( const SurfacePtr& s, int n )
{
    if ( m_RenderTarget ) {
        boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
        return m_RenderTarget->SetSurface( s, n );
    }
    return s;
}

SurfacePtr RenderNode::GetSurface( int n ) const
{
    return m_RenderTarget  ? m_RenderTarget->GetSurface( n ) : SurfacePtr();
}

void RenderNode::SetRenderTarget( RenderTargetInterfacePtr rti )
{
    boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );

    // new render target inherits surface of previous rendertarget - e.g. can load surface as texture into vram
    if ( m_RenderTarget != NULL ) {
        int i = 0;
        do {
            SurfacePtr s = m_RenderTarget->GetSurface(i);
            if ( s == NULL ) break;
            rti->SetSurface( s, i++ );
        } while (1);
	}
	m_RenderTarget = rti;
}

void RenderNode::Lock()
{
    m_RenderLock.lock();
}

bool RenderNode::TryLock()
{
    return m_RenderLock.try_lock();
}

void RenderNode::Unlock()
{
    m_RenderLock.unlock();
}

boost::recursive_mutex& RenderNode::GetLock() const
{
    return m_RenderLock;
}

/** activate: updated only */
RenderNode& RenderNode::Activate( bool active )
{
    if ( active ) {
        m_Flags |=  EN_ACTIVE;
    } else {
        m_Flags &= ~EN_ACTIVE;
    }
    return *this;
}

/** visible: visible, drawn if active */
RenderNode& RenderNode::Show( bool visible )
{
    if ( visible ) {
        m_Flags |=  EN_VISIBLE;
    } else {
        m_Flags &= ~EN_VISIBLE;
    }
    return *this;
}

/** paused: active but not updated, still drawn */
RenderNode& RenderNode::Pause( bool pause )
{
    if ( pause ) {
        m_Flags |=  EN_PAUSED;
    } else {
        m_Flags &= ~EN_PAUSED;
    }
    return *this;
}

RenderNode& RenderNode::SetAlpha( double level )
{
	m_RenderParam.SetAlpha(level);
    return *this;
}

RenderNode& RenderNode::SetRenderParam( const pei::RenderParam& p ) {
	RequestStateChanged(m_RenderParam != p);
	m_RenderParam = p; return *this;
}

bool RenderNode::Init( RenderProfilePtr& profile )
{
	bool r = OnInit( profile );
    if ( m_Children.size() > 0 ) {
	    RenderNodeList::iterator it = m_Children.begin();
        while ( it != m_Children.end() ) {
            RenderNodeInterfacePtr ro = *it++;
            r &= ro->Init( profile );
        }
    }
	return r;
}

// update the objects context
void RenderNode::Update( RenderProfilePtr& profile, double time, int parent_flags /*= 0*/, double zindex )
{
    m_ZIndex = zindex;
    int local_flags = parent_flags | m_Flags;
    // dump render object never does an update
    if ( (local_flags & EN_ACTIVE) == EN_ACTIVE )
    {
        if ( m_Children.size() > 0 ) {
    	    RenderNodeList::iterator it = m_Children.begin();
            while ( it != m_Children.end() ) {
                zindex += 0.1;
                RenderNodeInterfacePtr& ro = *it++;
                ro->Update( profile, time, local_flags, zindex );
            }
        }
        if ( (local_flags & EN_PAUSED) == 0 ) {
            OnUpdateAnimation( profile, time );
        }
        OnUpdateState( profile );
        m_Flags &= ~EN_UPDATE_STATE; // clear update request
    }

    // remove all depreciated nodes - before the Draw() call!
    if ( m_DelayRemoveList.size() ) {
        std::for_each( m_DelayRemoveList.begin(), m_DelayRemoveList.end(), boost::bind( &RenderNode::OnRemoveChild, this, _1 ));
        m_DelayRemoveList.clear();
    }
}

// draw the raw data to destination
int RenderNode::Draw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
{
    int n(0);
    if ( (m_Flags & EN_DRAWABLE) == EN_DRAWABLE ||
         (m_Flags & EN_REDRAW) != 0 ) // force redraw
    {
        int renderPass( param.GetRenderPass( ) );
        // to do: I have to push the current param on the stack and translate with &param
        if ( CanRender(renderPass) ) {
            n = 1;
            profile->PushMatrix( param );                           // this is the parent matrix
            RenderParam p = profile->Translate( m_RenderParam );    // now multiply our local translation matrix; result is multiplied matrix
            // inherit render pass
            p.SetRenderPass( renderPass );
            // backbuffer, usually same as buffer
            SurfacePtr backBuffer = OnPreDraw( profile, buffer, p );
            if ( (m_Flags & EN_SKIP_CHILDREN) == 0 && (m_Children.size() > 0) ) {
                RenderNodeList::iterator it = m_Children.begin();
                while ( it != m_Children.end() ) {
                    RenderNodeInterfacePtr& ro = *it++;
                    n += ro->Draw( profile, backBuffer, p );
                }
            }
            // now render backbuffer into buffer (if there was any)
            OnDraw( profile, buffer, p );
            /*m_RenderParam = */profile->PopMatrix();
            m_Flags &= ~EN_REDRAW;
        }
    }

    return n;
}

bool RenderNode::OnInit( RenderProfilePtr& profile ) {
	return true;
}

// update the objects static states
void RenderNode::OnUpdateState( RenderProfilePtr& profile )
{
}

// update the objects animation context
void RenderNode::OnUpdateAnimation( RenderProfilePtr& profile, double time )
{
}

// draw the raw data to destination
void RenderNode::OnDraw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param  )
{
    // doesn't need to be fast - should never execute in production code - always drawn on top of object
    if ( m_RigidBodies.size( ) ) {
        std::for_each( m_RigidBodies.begin(), m_RigidBodies.end(), boost::bind( &RigidBody::Render, _1, profile, buffer, param ) );
    }
}

// draw the raw data to destination
SurfacePtr RenderNode::OnPreDraw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param  )
{
    return buffer;
}

void RenderNode::Render( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param )
{
}

RenderNodeInterfacePtr RenderNode::OnRemoveChild( RenderNodeInterfacePtr ro )
{
    if (m_Children.size() > 0 ) {
        RenderNodeList::iterator it = std::find( m_Children.begin(), m_Children.end(), ro );
        if ( it != m_Children.end() ) {
            m_Children.erase( it );
            // delete parent. We are not the parent of the node any more
            if ( ro->GetFlags() & EN_DELETE_CHILDREN ) ro->RemoveAllChildren();
            ro->SetParent(RenderNodeInterfacePtr());
            ro->SetFlags(0);
        } else {
            // find it somewhere in the tree
            RenderNodeList::iterator it = m_Children.begin();
            while ( it != m_Children.end() ) {
                RenderNodeInterfacePtr child = *it++;
                child->RemoveChild( ro );
            }
        }
    }
    return ro;
}

void RenderNode::OnRemoveAllChildren( )
{
    if ( m_Children.size() > 0 ) {
        RenderNodeList::iterator it = ( m_Children.begin() );
        while ( it != m_Children.end() ) {
            RenderNodeInterfacePtr ro = *it++;
            ro->SetFlags(0);
            ro->RemoveAllChildren( );
        }
    }
    m_Children.clear();
}

RenderNode& RenderNode::SetSize( double w, double h )
{
	RequestStateChanged((m_RenderParam.GetWidth() != w) || (m_RenderParam.GetHeight() != h));
	m_RenderParam.SetWidth(w).SetHeight(h);
    return *this;
}

RenderNode& RenderNode::SetWidth( double w )
{
	RequestStateChanged(m_RenderParam.GetWidth() != w);
	m_RenderParam.SetWidth(w); return *this;
}

RenderNode& RenderNode::SetHeight( double h )
{
	RequestStateChanged(m_RenderParam.GetHeight() != h);
    m_RenderParam.SetHeight(h); return *this;
}

double RenderNode::GetWidth() const
{
    return m_RenderParam.GetWidth();
}

double RenderNode::GetHeight() const
{
    return m_RenderParam.GetHeight();
}

#include <iostream>

bool RenderNode::IsInside( double x, double y ) const
{
    /* if this object got no size, we assume we are inside - revise that...removed check 1/13*/
    double xp = GetXPosition();
    double yp = GetYPosition();
    double xm = xp+GetWidth()*GetXScale();
    double ym = yp+GetHeight()*GetYScale();
    return (( x >= xp ) && ( x < ( xm )  ) &&
            ( y >= yp ) && ( y < ( ym ) ));
}

RenderNodeInterfacePtr RenderNode::AddChild( RenderNodeInterfacePtr ro )
{
    if ( ro ) {
        boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );

        // make sure we link ourself!
        ro->LinkSmartPtr( ro );
        ro->SetParent( Get() );
        m_Children.push_back( ro );
        if ( IsAutoSortEnabled() ) {
            ZSort();
        }
    }
    return ro;
}

RenderNodeInterfacePtr RenderNode::AddChild( RenderNodeInterfacePtr ro, double z )
{
    if ( ro ) {
        ro->SetZPosition( z );
        return AddChild( ro );
    }
    return ro;
}

void RenderNode::Remove( bool remove_children /*= false */)
{
    if ( remove_children ) SetFlags( EN_DELETE_CHILDREN );
    boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
    RenderNodePtr rn = m_ParentNode->as<RenderNode>();
    if ( rn ) {
        RenderNodeInterfacePtr ro = Get();
        rn->m_DelayRemoveList.push_back( ro );
    }
}

RenderNodeInterfacePtr RenderNode::RemoveChild( RenderNodeInterfacePtr ro )
{
    boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
    return OnRemoveChild( ro );
}

void RenderNode::RemoveAllChildren( )
{
    boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
    OnRemoveAllChildren();
}

RenderNodeInterfacePtr RenderNode::FindChild( RenderNodeInterfacePtr ro ) const
{
    if ( ro == NULL ) {
        m_CurrentChild = m_Children.begin();
        if ( m_CurrentChild != m_Children.end() ) {
            return *m_CurrentChild;
        }
    }
    else{
        RenderNodeList::const_iterator it = std::find( m_Children.begin(), m_Children.end(), ro );
        if ( it != m_Children.end() ) {
            m_CurrentChild = it;
            return *m_CurrentChild;
        }
    }
    return RenderNodeInterfacePtr();
}

RenderNodeInterfacePtr RenderNode::FindChild( const char* name /* = NULL */ ) const
{
	if ( name == NULL ) {
		m_CurrentChild = m_Children.begin();
	    if ( m_CurrentChild != m_Children.end() ) {
	    	return *m_CurrentChild;
	    }
	} else if ( m_Children.size() > 0 ) {
		RenderNodeList::const_iterator cit = std::find_if( m_Children.begin(), m_Children.end(), nameCompare( name ) );
		if ( cit != m_Children.end() ) {
			return *cit;
		}
		// now search all children
		RenderNodeList::const_iterator it = m_Children.begin();
		while ( it != m_Children.end() ) {
			RenderNodeInterfacePtr ro = *it++;
			RenderNodeInterfacePtr child = ro->FindChild( name );
			if (child.get() != NULL ) {
				m_CurrentChild = it;
				return child;
			}
		}
	}
    return RenderNodeInterfacePtr();
}

RenderNodeInterfacePtr RenderNode::FindNextChild( const char* name ) const
{
	if ( name == NULL ) {
	    if ( (++m_CurrentChild) != m_Children.end() ) {
	    	return *m_CurrentChild;
	    }
	} else {
		if ( m_CurrentChild != m_Children.end() ) {
			RenderNodeList::const_iterator cit = std::find_if( ++m_CurrentChild, m_Children.end(), nameCompare( name ) );
			if ( cit != m_Children.end() ) {
				m_CurrentChild = cit;
				return *cit;
			}
		}
	}
    return RenderNodeInterfacePtr();
}

size_t RenderNode::GetNumberOfChildren() const
{
    return m_Children.size();
}

RenderNodeInterfacePtr& RenderNode::operator++(int) const// postfix
{
    const RenderNodeInterfacePtr& n = (*m_CurrentChild++);
    return const_cast<RenderNodeInterfacePtr&>(n);
}

RenderNodeInterfacePtr& RenderNode::operator--(int) const
{
    const RenderNodeInterfacePtr& n = (*m_CurrentChild--);
    return const_cast<RenderNodeInterfacePtr&>(n);
}

RenderNodeInterfacePtr& RenderNode::operator++() const// prefix
{
    if ( m_CurrentChild != m_Children.end() ) {
        const RenderNodeInterfacePtr& n = (*++m_CurrentChild);
        return const_cast<RenderNodeInterfacePtr&>(n);
    }
    return const_cast<RenderNodeInterfacePtr&>(*m_Children.end());
}

RenderNodeInterfacePtr& RenderNode::operator--() const
{
    const RenderNodeInterfacePtr& n = (*--m_CurrentChild);
    return const_cast<RenderNodeInterfacePtr&>(n);
}

RenderNodeInterfacePtr& RenderNode::begin() const
{
    // set current to begin
    m_CurrentChild = m_Children.begin();
    return const_cast<RenderNodeInterfacePtr&>(*m_CurrentChild);
}

RenderNodeInterfacePtr& RenderNode::end() const
{
    // do not set current
    return const_cast<RenderNodeInterfacePtr&>(*m_Children.end());
}

RenderNodeInterfacePtr RenderNode::NextChild() const
{
    // end is not a valid child!
    if ( m_CurrentChild != m_Children.end() && ++m_CurrentChild != m_Children.end() ) {
        return *(m_CurrentChild);
    }
    return RenderNodeInterfacePtr();
}

RenderNodeInterfacePtr RenderNode::PreviousChild() const
{
    // begin is a valid child
    if ( m_CurrentChild != m_Children.begin() ) {
        return *(--m_CurrentChild);
    }
    return RenderNodeInterfacePtr();
}

#ifdef USE_VECTOR
RenderNodeInterfacePtr RenderNode::GetChild( int num ) const
{
    if ( num >= 0 && num < (int)m_Children.size() ) {
        return m_Children[num];
    }
    return RenderNodeInterfacePtr();
}
#endif

bool RenderNode::Up( RenderNodeInterfacePtr rni )
{
    if ( rni && m_Children.size() >= 2 ) {
        RenderNodeList::reverse_iterator it = std::find( m_Children.rbegin(), m_Children.rend(), rni );
        if ( it != m_Children.rend() && it != m_Children.rbegin() ) {
            boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
            RenderNodeInterfacePtr& a = *it--;
            RenderNodeInterfacePtr& b = *it;
            bool r = Swap( a, b );
            return r;
        }
    }
    return false;
}

bool RenderNode::Down( RenderNodeInterfacePtr rni )
{
    if ( rni && m_Children.size() >= 2 ) {
        RenderNodeList::iterator it = std::find( m_Children.begin(), m_Children.end(), rni );
        if ( it != m_Children.end() && it != m_Children.begin() ) {
            boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
            RenderNodeInterfacePtr& a = *it--;
            RenderNodeInterfacePtr& b = *it;
            return Swap( a, b );
        }
    }
    return false;
}

bool RenderNode::Front( RenderNodeInterfacePtr rni )
{
    if ( rni && m_Children.size() >= 2 ) {
        // lock here to move to top without redraw
        boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
        while ( Up(rni ) ) ;
        return true;
    }
    return false;
}

bool RenderNode::Back( RenderNodeInterfacePtr rni )
{
    if ( rni && m_Children.size() >= 2 ) {
        // lock here to move to bottom without redraw
        boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
        while ( Down( rni )) ;
        return true;
    }
    return false;
}

bool RenderNode::Swap( RenderNodeInterfacePtr& a, RenderNodeInterfacePtr& b )
{
    if ( a && b ) {
        double za = a->GetZPosition();
        double zb = b->GetZPosition();
        a.swap( b );
        // swap z order - no need to resort!
        a->SetZPosition(za);
        b->SetZPosition(zb);
        // both must exist, then we return true
        RenderNodeList::iterator ita = std::find( m_Children.begin(), m_Children.end(), a );
        RenderNodeList::iterator itb = std::find( m_Children.begin(), m_Children.end(), b );
        return ( ita != m_Children.end() && itb != m_Children.end());
    }
    return false;
}

RenderNode& RenderNode::SetScale( double sx, double sy, double sz /*= 1.0 */ )
{
	RequestStateChanged((m_RenderParam.GetXScale() != sx) || (m_RenderParam.GetYScale() != sy) || (m_RenderParam.GetZScale() != sz));
	m_RenderParam.SetXScale(sx).SetYScale(sy).SetZScale(sz);
    return *this;
}

RenderNode& RenderNode::SetRotation( double xr, double yr, double zr ) {
	RequestStateChanged((m_RenderParam.GetXRotation() != xr) || (m_RenderParam.GetYRotation() != yr) || (m_RenderParam.GetZRotation() != zr));
    m_RenderParam.SetXRotation(xr).SetYRotation(yr).SetZRotation(zr);
    return *this;
}

void RenderNode::SetParent( RenderNodeInterfacePtr parent )
{
    m_ParentNode = parent;
}

RenderNodeInterfacePtr RenderNode::GetRoot() const
{
    boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );

    RenderNodeInterfacePtr root = GetParent();
    if ( root.get() != NULL ) {
        RenderNodeInterfacePtr parent = root->GetParent();
        while ( parent.get() != NULL ) {
            root = parent;
            parent = parent->GetParent();
        }
    }
    return root;
}

void RenderNode::ZSort( bool sort_children /*= false*/ )
{
    boost::lock_guard< boost::recursive_mutex > lock( m_RenderLock );
    if ( m_Children.size() > 0 ) {
#ifdef USE_VECTOR
        std::sort( m_Children.begin(), m_Children.end(), CompareRenderNodePtrFunc );
#else
        m_Children.sort( CompareRenderNodePtrFunc );
#endif
        if ( sort_children ) {
 	    RenderNodeList::iterator it = m_Children.begin();
		    if ( it != m_Children.end() ) {
			    RenderNodeInterfacePtr ro = *it++;
			    ro->ZSort();
		    }
        }
    }
    // reset current child
    m_CurrentChild = m_Children.begin();

}

RenderNode& RenderNode::TranslateFromScreenCoords( double &x, double &y )
{
    pei::RenderNodeInterfacePtr o = boost::static_pointer_cast<pei::RenderNodeInterface>(this->GetParent());
    while (o) {
        double xs = o->GetXScale();
        double ys = o->GetYScale();
        if ( xs > 0.0 ) x = (x-o->GetXPosition())/xs;
        if ( ys > 0.0 ) y = (y-o->GetYPosition())/ys;
        o = boost::static_pointer_cast<pei::RenderNodeInterface>(o->GetParent());
    };
    return *this;
}

RenderNode& RenderNode::TranslateToScreenCoords( double &x, double &y, double &w, double& h )
{
    // start with self
    pei::RenderNodeInterfacePtr o = boost::static_pointer_cast<pei::RenderNodeInterface>(GetParent());
    while (o) {
        double xs = o->GetXScale();
        double ys = o->GetYScale();
        x = (x+o->GetXPosition())*xs;
        y = (y+o->GetYPosition())*ys;
        w *= xs;
        h *= ys;
        o = boost::static_pointer_cast<pei::RenderNodeInterface>(o->GetParent());
    };
    return *this;
}

pei::Vector4f RenderNode::TranslateFromScreenCoords( )
{
    // ???
    return pei::Vector4f();
}

pei::Vector4f RenderNode::TranslateToScreenCoords( )
{
    // ???
    return pei::Vector4f();
}
