/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <render/render_node.h>

namespace pei {
    class Drawable;
    typedef boost::shared_ptr<Drawable> DrawablePtr;

    class Drawable : public RenderNode
    {
    public:
        /** */
        Drawable();

        /** */
        Drawable( const RenderParam& p, const SurfacePtr& s = SurfacePtr() );

        /** */
        Drawable( const SurfacePtr& s );

        /** */
        ~Drawable();
    protected:
        virtual SurfacePtr OnPreDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam()  );

        /** */
        virtual void OnDraw( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam()  );

        /** */
        virtual void Render( pei::RenderProfilePtr& profile, SurfacePtr& buffer, const RenderParam& param = RenderParam() );
    };

} /* namespace pei */

#endif /* DRAWABLE_H */
