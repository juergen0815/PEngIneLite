/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_PROFILE_H
#define RENDER_PROFILE_H

#include <render/render_surface.h>
#include <render/render_param.h>
#include <render/render_node.h>
#include <render/render_target.h>

#include <math/matrix.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>

#include <string>
#include <map>
#include <deque>

namespace pei
{
	class RenderProfile;
	typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

	class RenderProfile
	{
	public:
        enum enFlags {
            EN_RENDER_ASYNC = 1<<0
        };
        static const char* DEFAULT_TARGET;
	protected:
		boost::weak_ptr<RenderProfile> m_WeakSelf;
		RenderNodeInterfacePtr         m_Screen;     // main virtual view port

        size_t                         m_Flags;
        pei::Color                     m_ClearColor;
        unsigned int                   m_MatrixMode;
        pei::Matrix4x4                 m_Matrix;        //!< current modelview matrix
        pei::RenderParam               m_RenderParam;   //!< current render param - incl. ref to model view matrix
        std::deque<pei::RenderParam>   m_ParamStack;

        typedef std::map< std::string, pei::RenderTargetInterfacePtr > RenderTargetList;
        RenderTargetList 			   m_RenderTargets;

        static pei::RenderTargetInterfacePtr m_DefaultRenderTarget;

        pei::Rectangle                 m_ClipRegion;
        bool						   m_ClippingEnabled;

        pei::SurfacePtr                m_BackBuffer; // default == NULL

		RenderProfile();

	public:
		virtual void LinkSmartPtr( pei::RenderProfilePtr rp ) {
			m_WeakSelf = rp;
		}

		virtual pei::RenderProfilePtr GetSmartPtr() {
			return pei::RenderProfilePtr( m_WeakSelf );
		}

		virtual ~RenderProfile();

		static void SetDefaultRenderTarget( const RenderTargetInterfacePtr& rti );

		static pei::RenderTargetInterfacePtr GetDefaultRenderTarget();

		virtual bool Init();

		virtual void Close();

        virtual size_t GetFlags() { return m_Flags; }

		virtual bool IsActive();

		virtual double GetTicks();

		virtual void Delay( double t );

		virtual void Lock();

		virtual bool TryLock();

		virtual void Unlock();

        /////////////////////////////////////////////////////////////
        // Screen handling

        virtual bool OpenScreen( size_t width, size_t height, size_t flags, const char* title );

        virtual RenderNodeInterfacePtr ResizeScreen( int w, int h );

        pei::RenderNodeInterfacePtr GetScreen() const;

        virtual void MakeCurrent();

        virtual void ClearBuffers();

        virtual void SwapBuffers();

		/////////////////////////////////////////////////////////////
		// Render targets, etc
		virtual void AddRenderTarget( const char* name, const pei::RenderTargetInterfacePtr& rti );

		virtual pei::RenderTargetInterfacePtr GetRenderTarget( const char* name = DEFAULT_TARGET ) const;

        /** install a clip regin in window coordinates */
        virtual void SetClipRegion( const pei::Rectangle& bounds );

        virtual const Rectangle& GetClipRegion() const;

        virtual void ClearClipRegion();

        virtual void EnableClipping();

        virtual void DisableClipping();

        virtual bool IsClippingEnabled();

        virtual void SetClearColor( const pei::Color& color ) { m_ClearColor = color; }

        virtual void RenderFrame( pei::RenderProfilePtr& self, double ticks );

        virtual bool Enable( int attr ) { return false; }

        virtual bool Disable( int attr ) { return false; }

        virtual pei::RenderParam LoadIdentity( const pei::RenderParam& identity = pei::RenderParam() );

        /*!
         *  Set the matrix mode
         * @param mode - PROJECTION, MODELVIEW
         */
        virtual void SetMatrixMode( unsigned int mode );

        virtual unsigned int GetMatrixMode( ) const ;

        /*!
         * Return the current matrix
         * @param mode - projection or modelview matrix where supported
         * @return a [4][4] matrix or NULL
         */
        virtual const Matrix4x4& GetMatrix() const { return m_Matrix; }

        virtual Matrix4x4& GetMatrix() { return m_Matrix; }

        virtual void LoadMatrix( const Matrix4x4& matrix );

        virtual const RenderParam& PushMatrix( const RenderParam& p );

        virtual const RenderParam& PushMatrix( );

        virtual RenderParam Translate( const RenderParam& param );

        virtual const RenderParam& PopMatrix();

		/////////////////////////////////////////////////////////////
		// Low level interfaces

		virtual void Fill( SurfacePtr surface, const RenderParam& param );

		virtual SurfacePtr CreateSurface( const pei::Format& fmt);

	};

} /* namespace pei */
#endif /* RENDER_PROFILE_H */
