/************************************************************************/
/* pei::Engine (©)Copyright 2009-12 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef BOUNDS3D_H
#define BOUNDS3D_H

#include <math/vector4f.h>

#include <string>
#include <vector>

#include <math.h>

namespace pei
{
    class Bounds3D {
        Vector4f m_MinVertex;
        Vector4f m_MaxVertex;
        Vector4f m_Center;
        mutable std::vector<float> m_QuadStrips; // cached cube array

    public:
        /*!
         *
         */
        Bounds3D() { }

        /*!
         *
         * @param o
         */
        Bounds3D( const Bounds3D& o )
            : m_MinVertex(o.m_MinVertex)
            , m_MaxVertex(o.m_MaxVertex)
        {
            m_QuadStrips = MakeQuadStrips( m_MinVertex, m_MaxVertex );
            m_Center = Vector4f::Sub( m_MaxVertex, m_MinVertex ).Div(2).Add(m_MinVertex);
        }

        /*!
         *
         * @param min
         * @param max
         */
        Bounds3D( const Vector4f& min, const Vector4f& max ) : m_MinVertex(min), m_MaxVertex(max)
        {
            m_Center = Vector4f::Sub( m_MaxVertex, m_MinVertex ).Div(2).Add(m_MinVertex);
	        m_QuadStrips = MakeQuadStrips( min, max );
        }

        /*!
         *
         * @param min
         * @param max
         * @return
         */
        inline Bounds3D& Set( const Vector4f& min, const Vector4f& max ) {
	        m_MinVertex = min;
	        m_MaxVertex = max;
	        m_QuadStrips = MakeQuadStrips( min, max );
            m_Center = Vector4f::Sub( m_MaxVertex, m_MinVertex ).Div(2).Add(m_MinVertex);
	        return *this;
        }

        /*!
         *
         * @param min
         * @return
         */
        inline Bounds3D& Min( const Vector4f& min ) {
	        m_MinVertex = min;
	        // reset bounds
	        m_QuadStrips.clear();
	        return *this;
        }

        /*!
         *
         * @param max
         * @return
         */
        inline Bounds3D& Max( const Vector4f& max ) {
	        m_MaxVertex = max;
	        // reset bounds
	        m_QuadStrips.clear();
	        return *this;
        }

        /*!
         *
         * @return
         */
        inline const Vector4f& Min() const {
	        return m_MinVertex;
        }

        /*!
         *
         * @return
         */
        inline const Vector4f& Max() const {
	        return m_MaxVertex;
        }

        /*!
         *
         * @param center
         */
        inline void SetCenter( const Vector4f& center ) {
            // override. e.g. off bounds center
            m_Center = center;
        }

        /*!
         *
         * @return
         */
        inline Vector4f GetCenter() const {
            return m_Center;
        }

        /*!
         *
         * @return
         */
        // should be min/max radius (inner/outer radius)
        inline float GetRadius() const {
            // well, thats the center of gravity of the box - can be off bounds, though
	        return GetSize().Magnitude()/2;
        }

        /*!
         *
         * @return
         */
        inline float GetWidth() const {
	        return fabs( m_MaxVertex.x() - m_MinVertex.x() );
        }

        /*!
         *
         * @return
         */
        inline float GetHeight() const {
	        return fabs( m_MaxVertex.y() - m_MinVertex.y() );
        }

        /*!
         *
         * @return
         */
        inline float GetDepth() const {
	        return fabs( m_MaxVertex.z() - m_MinVertex.z() );
        }

        /*!
         *
         * @return
         */
        inline Vector4f GetSize() const {
	        return Vector4f( GetWidth(), GetHeight(), GetDepth() );
        }

        /*! 6 quads sequentially counted (quad strips - GL_QUAD_STRIP) - no bottom/top!
         *
         *       7 +-----------+ 5 (max)
         *        /|   <--    /|
         *       / |         / |
         *    1 /  |      3 /  |
         *  y  +-----------+   |
         *  ^  |   +-------|---+ 4
         *  |  |  / 6      |  /
         *  |  | /         | /
         *  |  |/   -->    |/
         *  |  +-----------+
         *  |  0 (min)     2
         *  + ----------------> x
         *
         */
        inline const std::vector<float>& asQuadStrips() const {
	        if ( m_QuadStrips.size() != 0 ) {
		        m_QuadStrips = MakeQuadStrips( m_MinVertex, m_MaxVertex );
	        }
	        return m_QuadStrips;
        }

        /*!
         *
         * @param min
         * @param max
         * @return
         */
        static inline std::vector<float> MakeQuadStrips( const Vector4f& min, const Vector4f& max ) {
	        // Does not wrap around!
	        float tv [] = {
			        min.x(), min.y(), min.z(), // 0
			        min.x(), max.y(), min.z(), // 1
			        max.x(), min.y(), min.z(), // 2
			        max.x(), max.y(), min.z(), // 3
			        max.x(), min.y(), max.z(), // 4
			        max.x(), max.y(), max.z(), // 5
			        min.x(), min.y(), max.z(), // 6
			        min.x(), max.y(), max.z(), // 7
	        };
            return std::vector<float>(&tv[0],&tv[23]);
        }
    };

} // namespace pei

#endif /* BOUNDS3D_H */
