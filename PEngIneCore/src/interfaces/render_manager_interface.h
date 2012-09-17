/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_MANAGER_INTERFACE_H
#define RENDER_MANAGER_INTERFACE_H

#include <interfaces/render_node_interface.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace pei
{
    class RenderProfile;
    typedef boost::shared_ptr<RenderProfile> RenderProfilePtr;

    class RenderManagerInterface;
    typedef boost::shared_ptr<RenderManagerInterface> RenderManagerInterfacePtr;

    class RenderManagerInterface
    {
    public:
		typedef boost::function<void( double ticks )> PerformanceMonitorFunc;
    public:
        virtual ~RenderManagerInterface() {}

        virtual void InstallPerformanceMonitor( const PerformanceMonitorFunc& func ) = 0;

        virtual void SetRenderProfile( RenderProfilePtr profile ) = 0;

	    virtual RenderProfilePtr GetRenderProfile() const = 0;

	    virtual bool StartRenderLoop() = 0;

        virtual void ShutDown() = 0;

        virtual void WaitThreadFinished() = 0;

        virtual double RenderFrame( double ticks ) = 0;

        virtual void ResizeViewport( int w, int h ) = 0;

        virtual void Lock() = 0;

        virtual bool TryLock() = 0;

        virtual void Unlock() = 0;

        virtual double SetTimeBase( double t ) = 0;

        virtual double GetTimeBase() const = 0; 

    };

} /* namespace pei */

#endif /* RENDER_MANAGER_INTERFACE_H */
