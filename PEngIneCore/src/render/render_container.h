/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef RENDER_CONTAINER_H
#define RENDER_CONTAINER_H

#include <render/render_node.h>

namespace pei {
    class RenderContainer;
    typedef boost::shared_ptr<RenderContainer> RenderContainerPtr;

    /*!
     * A RenderContainer is a dummy node optimized for performance. It allows you
     * to group nodes without location data - no matrix operations are performed in this node.
     * This can be helpful if you want to enable or disable a group of objects or move
     * them from one sub tree to another while all translation is done in the
     * child nodes only. The RenderManager uses a container as its root node, for example.
     * If you need to move a group of objects, use a standard RenderNode instead.
     */
    class RenderContainer : public RenderNode
    {
    public:
        /** */
        RenderContainer();

        /** */
        ~RenderContainer();

        virtual int Draw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param );

    private: // you cannot overload these
        virtual SurfacePtr OnPreDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam()  );

        /** */
        virtual void OnDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam()  );

        /** */
        virtual void Render( SurfacePtr& buffer, const RenderParam& param = RenderParam() );
    };

} /* namespace pei */

#endif /* RENDER_CONTAINER_H */
