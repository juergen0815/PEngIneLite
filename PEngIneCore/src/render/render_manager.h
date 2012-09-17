/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <interfaces/render_manager_interface.h>

#include <render/render_profile.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

namespace pei
{
    class RenderManager;
    typedef boost::shared_ptr<RenderManager> RenderManagerPtr;

    class RenderManager : public RenderManagerInterface
    {

    protected:
        boost::weak_ptr<RenderManager>   m_WeakSelf;
	    RenderProfilePtr       			 m_RenderProfile;
	    boost::shared_ptr<boost::thread> m_RenderThread;
		PerformanceMonitorFunc 			 m_PerformanceMonitor;

        double                           m_TimeBase; // = 1.0

        RenderManager( );

        void InitSharedPtr( RenderManagerPtr self ) {
            m_WeakSelf = self;
        }
    public:
        static RenderManagerInterfacePtr Create() {
            RenderManagerPtr self( new pei::RenderManager() ) ;
            self->InitSharedPtr( self );
            return self;
        }
	    virtual ~RenderManager();

		virtual void InstallPerformanceMonitor( const PerformanceMonitorFunc& func ) {
			m_PerformanceMonitor = func;
		}

	    virtual void SetRenderProfile( RenderProfilePtr profile );

	    virtual RenderProfilePtr GetRenderProfile() const;

	    virtual bool StartRenderLoop();

        virtual void ShutDown();

	    virtual void WaitThreadFinished();

	    virtual double RenderFrame( double ticks );

        virtual void ResizeViewport( int w, int h );

        virtual void Lock();

        virtual bool TryLock();

        virtual void Unlock();

        virtual double SetTimeBase( double t );

        virtual double GetTimeBase() const;

    protected:
	    virtual void RenderLoop();
    };

} /* namespace peiTesting */
#endif /* RENDER_MANAGER_H */
