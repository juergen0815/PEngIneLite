/************************************************************************/
/* pei::Engine (�)Copyright 2009-11 J�rgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#include <render/render_manager.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace pei;

RenderManager::RenderManager( )
    : m_TimeBase(1.0)   // default timebase
{
}

RenderManager::~RenderManager()
{
    // signal exit
    ShutDown();
    // wait for render thread to finish
	WaitThreadFinished();
    m_RenderProfile = pei::RenderProfilePtr();
}

void RenderManager::SetRenderProfile( RenderProfilePtr profile )
{
	m_RenderProfile = profile;
}

RenderProfilePtr RenderManager::GetRenderProfile() const
{
	return m_RenderProfile;
}

bool RenderManager::StartRenderLoop()
{
    // MacOS (10.5.x) won't render async - see Cacao limitations/autofree issues with render thread
    // EventManager can handle all events asynchronously. It is safe to render synchronously.
    // Use a render callback and render signal with the render manager to render a frame.
    // Allows to set render priorities and interleave generic events with draw events.

    if ( m_RenderProfile )
    {
        m_RenderProfile->Init();
        if  ( (m_RenderProfile->GetFlags() & pei::RenderProfile::EN_RENDER_ASYNC) != 0 )
        {
	        m_RenderThread = boost::shared_ptr<boost::thread>( new boost::thread( &RenderManager::RenderLoop, this ));
	        return true;
        }
    }
    return false;
}

void RenderManager::WaitThreadFinished()
{
	if ( m_RenderThread ) {
		m_RenderThread->join();
	}
}

void pei::RenderManager::ShutDown()
{
    if ( m_RenderProfile ) {
        m_RenderProfile->Close();
    }
}

void pei::RenderManager::ResizeViewport( int w, int h )
{
	if ( m_RenderProfile ) {
	    m_RenderProfile->ResizeScreen( w, h );
	}
}

void pei::RenderManager::Lock()
{
    if ( m_RenderProfile ) {
        m_RenderProfile->Lock();
    }
}

bool pei::RenderManager::TryLock()
{
    if ( m_RenderProfile ) {
        return m_RenderProfile->TryLock();
    }
    return false;
}

void pei::RenderManager::Unlock()
{
    if ( m_RenderProfile ) {
        m_RenderProfile->Unlock();
    }
}

void RenderManager::RenderLoop()
{
    if ( m_RenderProfile ) {
        double current_ticks = m_RenderProfile->GetTicks();
        while ( m_RenderProfile->IsActive() )
        {
            if ( m_RenderProfile->TryLock() ) {
                current_ticks = RenderFrame( current_ticks );
                m_RenderProfile->Unlock();
            }
        }
    }
}

double RenderManager::RenderFrame( double current_ticks )
{
    if ( m_RenderProfile ) {
        double ticks = m_RenderProfile->GetTicks();
        m_RenderProfile->MakeCurrent();
        m_RenderProfile->ClearBuffers();
        if ( m_RenderProfile->IsActive() ) {
            double delta = ticks - current_ticks;
            m_RenderProfile->RenderFrame( m_RenderProfile, delta * GetTimeBase() );
            if ( m_PerformanceMonitor != NULL ) {
                m_PerformanceMonitor( delta );
            }
        }
        m_RenderProfile->SwapBuffers();
        return ticks;
    }
    return current_ticks;
}

double RenderManager::SetTimeBase( double t )
{
    double to = m_TimeBase;
    m_TimeBase = t;
    return to;
}

double RenderManager::GetTimeBase() const
{
    return m_TimeBase;
}
