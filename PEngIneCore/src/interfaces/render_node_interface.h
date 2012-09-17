/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_INTERFACE_H
#define RENDER_INTERFACE_H

#include <boost/shared_ptr.hpp>

namespace pei
{
	class RenderProfile;
	typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;
	class RenderParam;
	typedef boost::shared_ptr<RenderParam> RenderParamPtr;
	class Surface;
	typedef boost::shared_ptr<Surface> SurfacePtr;
	class RenderTargetInterface;
	typedef boost::shared_ptr<RenderTargetInterface> RenderTargetInterfacePtr;

	class RenderNodeInterface;
    typedef boost::shared_ptr<RenderNodeInterface> RenderNodeInterfacePtr;

    class Color;

    enum enRenderPass
    {
        DEFAULT_PASS    = 1 << 0,
        STENCIL_PASS    = 1 << 1,
        REFLECTION_PASS = 1 << 2,
        SHADOW_PASS     = 1 << 3,
        LIGHTING_PASS   = 1 << 4,   // transform only! all lights

        // multi pass for front/backface culling - default is front & backface rendered in DEFAULT pass. If you want
        // an extra back face pass, use FRONT_FACE in DEFAULT and add an additional BACK_FACE pass.
        FRONT_FACE_PASS = DEFAULT_PASS,
        BACK_FACE_PASS  = 1 << 5,

        USER_PASS = 1 << 24
    };

	class RenderNodeInterface
    {
    public:
    	virtual ~RenderNodeInterface() {}

        virtual void LinkSmartPtr(RenderNodeInterfacePtr smart_this ) = 0;

        virtual RenderNodeInterfacePtr Get() = 0;

        // use this to create a render node! Or must link manually
        template <class T>
        static RenderNodeInterfacePtr Create( ) {
            RenderNodeInterfacePtr ptr( new T );
            ptr->LinkSmartPtr( ptr );
            return ptr;
        }

        template <class T>
        boost::shared_ptr<T> as() { return boost::dynamic_pointer_cast<T>(Get()); }

    	virtual const char* GetName() const = 0;

        virtual RenderNodeInterface& SetName( const char* ) = 0;

        virtual void SetFlags( unsigned int flags ) = 0;

        virtual unsigned int GetFlags() const = 0;

        virtual RenderNodeInterface& Activate( bool active ) = 0;

        /** visible: visible, drawn if active */
        virtual RenderNodeInterface& Show( bool visible ) = 0;

        /** paused: active but not updated, still drawn */
        virtual RenderNodeInterface& Pause( bool pause ) = 0;

        virtual bool IsActive()  const = 0;
        virtual bool IsVisible() const = 0;
        virtual bool IsPaused()  const = 0;

        /** mark an updated state change - we need to process these changes in UpdateState() */
        virtual RenderNodeInterface& RequestStateChanged() = 0;

        virtual bool MustRefreshState() const = 0;

        virtual RenderNodeInterface& RequestRedraw() = 0;

        virtual bool MustRedraw() const = 0;

        virtual void AutoSortChildren( bool auto_sort = true ) = 0;

        virtual bool IsAutoSortEnabled() = 0;

        virtual bool CanRender( int renderPass ) = 0;

        virtual void EnableRenderPass( int renderPass ) = 0;

        virtual void DisableRenderPass( int renderPass ) = 0;

        virtual void EnableRenderMode( int renderMode ) = 0;

        virtual void DisableRenderMode( int renderMode ) = 0;

        //////////////////////////////////////////////////////////////////////////

        virtual SurfacePtr SetSurface( const SurfacePtr& s, int n = 0 ) = 0;

        virtual SurfacePtr GetSurface( int n = 0) const = 0;

        virtual void SetRenderTarget( RenderTargetInterfacePtr rti ) = 0;

        virtual RenderTargetInterfacePtr GetRenderTarget() const = 0;

        virtual bool Init( RenderProfilePtr& profile ) = 0;

        virtual int Draw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param ) = 0;

        virtual void Update( RenderProfilePtr& profile, double time, int parent_flags = 0, double zindex = 0 ) = 0;

        /** get the zindex - this cannot be set other than within Update! */
        virtual double GetZIndex( ) const = 0;

        virtual RenderNodeInterface& SetPosition( double x, double y ) = 0;

        virtual RenderNodeInterface& SetPosition( double x, double y, double z ) = 0;

        virtual RenderNodeInterface& SetXPosition( double x ) = 0;

        virtual RenderNodeInterface& SetYPosition( double y ) = 0;

        virtual RenderNodeInterface& SetZPosition( double z, bool resort = false ) = 0;

        virtual double GetXPosition() const = 0;

        virtual double GetYPosition() const = 0;

        virtual double GetZPosition( ) const = 0;

        virtual RenderNodeInterface& SetSize( double w, double h ) = 0;

        virtual RenderNodeInterface& SetWidth( double w ) = 0;

        virtual RenderNodeInterface& SetHeight( double h ) = 0;

        virtual double GetWidth() const = 0;

        virtual double GetHeight() const = 0;

        virtual RenderNodeInterface& SetScale( double xs, double ys, double zs = 1.0 ) = 0;

        virtual RenderNodeInterface& SetXScale( double s ) = 0;

        virtual RenderNodeInterface& SetYScale( double s ) = 0;

        virtual RenderNodeInterface& SetZScale( double s ) = 0;

        virtual double GetXScale( ) const = 0;

        virtual double GetYScale( ) const = 0;

        virtual double GetZScale( ) const = 0;

        virtual RenderNodeInterface& SetRotation( double xr, double yr, double zr ) = 0;

        virtual RenderNodeInterface& SetXRotation( double a ) = 0;

        virtual RenderNodeInterface& SetYRotation( double a ) = 0;

        virtual RenderNodeInterface& SetZRotation( double s ) = 0;

        virtual double GetXRotation( ) const = 0;

        virtual double GetYRotation( ) const = 0;

        virtual double GetZRotation( ) const = 0;


        virtual RenderNodeInterface& SetAlpha( double level ) = 0;

        virtual double GetAlpha() const = 0;

        virtual RenderNodeInterface& SetColor( const pei::Color& c ) = 0;

        virtual const pei::Color& GetColor() const = 0;

        virtual RenderNodeInterface& SetRenderParam( const pei::RenderParam& p ) = 0;

        virtual const pei::RenderParam& GetRenderParam() const = 0;

        virtual pei::RenderParam& GetRenderParam() = 0;

        virtual void Lock() = 0;

        virtual bool TryLock() = 0;

        virtual void Unlock() = 0;

        //////////////////////////////////////////////////////////////////////////

        virtual RenderNodeInterfacePtr GetParent() const = 0;

        /** */
        virtual RenderNodeInterfacePtr AddChild( RenderNodeInterfacePtr ro ) = 0;

        /** */
        virtual RenderNodeInterfacePtr AddChild( RenderNodeInterfacePtr ro, double z ) = 0;

        /*!
         * Call Delete within Update/Draw
         */
        virtual void Remove( bool remove_children = false ) = 0;

        /*!
         * Must not be called withing Update/Draw
         */
        virtual RenderNodeInterfacePtr RemoveChild( RenderNodeInterfacePtr ro ) = 0;

        /*!
         * Must not be called within Update/Draw
         */
        virtual void RemoveAllChildren( ) = 0;

        virtual RenderNodeInterfacePtr FindChild( RenderNodeInterfacePtr ro ) const = 0;

        virtual RenderNodeInterfacePtr FindChild( const char* name = NULL ) const = 0;

        virtual RenderNodeInterfacePtr FindNextChild( const char* name ) const = 0;

        virtual size_t GetNumberOfChildren() const = 0;

//        virtual RenderNodeInterfacePtr GetChild( int num ) const = 0;

        virtual RenderNodeInterfacePtr& operator++(int) const = 0;

        virtual RenderNodeInterfacePtr& operator--(int) const = 0;

        virtual RenderNodeInterfacePtr& operator++() const = 0;

        virtual RenderNodeInterfacePtr& operator--() const = 0;

        virtual RenderNodeInterfacePtr& begin() const = 0;

        virtual RenderNodeInterfacePtr& end() const = 0;

        virtual RenderNodeInterfacePtr NextChild() const = 0;

        virtual RenderNodeInterfacePtr PreviousChild() const = 0;

        virtual bool Up( RenderNodeInterfacePtr rni ) = 0;

        virtual bool Down( RenderNodeInterfacePtr rni ) = 0;

        virtual bool Front( RenderNodeInterfacePtr rni ) = 0;

        virtual bool Back( RenderNodeInterfacePtr rni ) = 0;

        virtual bool Swap( RenderNodeInterfacePtr& a, RenderNodeInterfacePtr& b ) = 0;

        virtual void ZSort( bool sort_children = false ) = 0;

        virtual void SetParent( RenderNodeInterfacePtr parent ) = 0;

        //////////////////////////////////////////////////////////////////////////

        virtual bool IsInside( double x, double y ) const = 0;
    protected:

//        virtual bool OnInit( RenderProfilePtr& profile ) = 0;
//
//        virtual void OnUpdateState( RenderProfilePtr& profile ) = 0;
//
//        /** update time dependent animation states (not called if paused) */
//        virtual void OnUpdateAnimation( RenderProfilePtr& profile, double time ) = 0;
//
//        /** TODO: reconsider returning a surface here */
//        virtual SurfacePtr OnPreDraw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param ) = 0;
//
//        /** */
//        virtual void OnDraw( RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param ) = 0;

    };


}; // namespace pei

#endif // RENDER_INTERFACE_H
