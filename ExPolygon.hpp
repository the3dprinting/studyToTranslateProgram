#ifndef slic3r_ExPolygon_hpp_
#define slic3r_ExPolygon_hpp_

#include "Polygon.hpp"
#include "Polyline.hpp"
#include <vector>

namespace Slic3r {

class ExPolygon;
typedef std::vector<ExPolygon> ExPolygons;

class ExPolygon  //真正意义上的多边形，有一个外轮廓，若干个洞组成的
{
    public:
    Polygon contour;
    Polygons holes;
    operator Points() const;  //类型转换函数，轮廓和洞的所有的点（注意复习类型转换函数）
    operator Polygons() const;  //类型转换函数，返回Polydons第一个是外轮廓，后面一次是洞
    void scale(double factor);  //放大factor倍	
    void translate(double x, double y);  //平移x y距离
    void rotate(double angle, const Point &center);  //绕center逆时针旋转angle
    double area() const;  //总面积=轮廓面积-洞面积
    bool is_valid() const;  //有效必须满足，外轮廓逆时针，洞顺时针，同时都只是3个点
    bool contains(const Line &line) const;  //返回line是否在多边形内部
    bool contains(const Polyline &polyline) const;  //返回一个多线段是否在多边形内部
    bool contains(const Point &point) const;   //点是否在轮廓内
    bool contains_b(const Point &point) const;  //点是否在轮廓内或在边界上都返回true
    bool has_boundary_point(const Point &point) const;  //point是否在轮廓边界上
    Polygons simplify_p(double tolerance) const;  //按照弦高误差为tolerance简化多边形，返回Polygons即可，第一个为轮廓，后面的为洞
    ExPolygons simplify(double tolerance) const;  //按照弦高误差为tolerance简化多边形，返回ExPolygons
    void simplify(double tolerance, ExPolygons &expolygons) const;  //将本身简化后的Expolygons传入的expolygons的后面
    void medial_axis(double max_width, double min_width, Polylines* polylines) const;  //生成中轴线，并放在polylines里面
    void get_trapezoids(Polygons* polygons) const;  //将本身的多边形集合分为一堆四边形，放在polygons里面
    void get_trapezoids(Polygons* polygons, double angle) const;  //以angle角度方向划分四边形放在polygons里面
    void get_trapezoids2(Polygons* polygons) const;   //将本身沿着x正方向每两个点在y方向延长后和本身相交的图形划分为一堆多边形并存储到polygons里
    void get_trapezoids2(Polygons* polygons, double angle) const;  //以一定角度分割本身，调用上面函数
    void triangulate(Polygons* polygons) const;  //先按照上面函数四边形化，再三角化
    void triangulate_pp(Polygons* polygons) const;  //https://github.com/ivanfratric/polypartition里面的Triangulate_MONO三角化
    void triangulate_p2t(Polygons* polygons) const;   //http://code.google.com/p/poly2tri/里的三角化方法进行三角化
    Lines lines() const;  //将带洞多边形所有点转换为lines类来返回，即一个带洞多边形的所有边
    
    #ifdef SLIC3RXS
    void from_SV(SV* poly_sv);
    void from_SV_check(SV* poly_sv);
    SV* to_AV();
    SV* to_SV_pureperl() const;
    #endif
};

}

// start Boost
#include <boost/polygon/polygon.hpp>
namespace boost { namespace polygon {
    template <>
        struct polygon_traits<ExPolygon> {
        typedef coord_t coordinate_type;
        typedef Points::const_iterator iterator_type;
        typedef Point point_type;

        // Get the begin iterator
        static inline iterator_type begin_points(const ExPolygon& t) {
            return t.contour.points.begin();
        }

        // Get the end iterator
        static inline iterator_type end_points(const ExPolygon& t) {
            return t.contour.points.end();
        }

        // Get the number of sides of the polygon
        static inline std::size_t size(const ExPolygon& t) {
            return t.contour.points.size();
        }

        // Get the winding direction of the polygon
        static inline winding_direction winding(const ExPolygon& t) {
            return unknown_winding;
        }
    };

    template <>
    struct polygon_mutable_traits<ExPolygon> {
        //expects stl style iterators
        template <typename iT>
        static inline ExPolygon& set_points(ExPolygon& expolygon, iT input_begin, iT input_end) {
            expolygon.contour.points.assign(input_begin, input_end);
            // skip last point since Boost will set last point = first point
            expolygon.contour.points.pop_back();
            return expolygon;
        }
    };
    
    
    template <>
    struct geometry_concept<ExPolygon> { typedef polygon_with_holes_concept type; };

    template <>
    struct polygon_with_holes_traits<ExPolygon> {
        typedef Polygons::const_iterator iterator_holes_type;
        typedef Polygon hole_type;
        static inline iterator_holes_type begin_holes(const ExPolygon& t) {
            return t.holes.begin();
        }
        static inline iterator_holes_type end_holes(const ExPolygon& t) {
            return t.holes.end();
        }
        static inline unsigned int size_holes(const ExPolygon& t) {
            return t.holes.size();
        }
    };

    template <>
    struct polygon_with_holes_mutable_traits<ExPolygon> {
         template <typename iT>
         static inline ExPolygon& set_holes(ExPolygon& t, iT inputBegin, iT inputEnd) {
              t.holes.assign(inputBegin, inputEnd);
              return t;
         }
    };
    
    //first we register CPolygonSet as a polygon set
    template <>
    struct geometry_concept<ExPolygons> { typedef polygon_set_concept type; };

    //next we map to the concept through traits
    template <>
    struct polygon_set_traits<ExPolygons> {
        typedef coord_t coordinate_type;
        typedef ExPolygons::const_iterator iterator_type;
        typedef ExPolygons operator_arg_type;

        static inline iterator_type begin(const ExPolygons& polygon_set) {
            return polygon_set.begin();
        }

        static inline iterator_type end(const ExPolygons& polygon_set) {
            return polygon_set.end();
        }

        //don't worry about these, just return false from them
        static inline bool clean(const ExPolygons& polygon_set) { return false; }
        static inline bool sorted(const ExPolygons& polygon_set) { return false; }
    };

    template <>
    struct polygon_set_mutable_traits<ExPolygons> {
        template <typename input_iterator_type>
        static inline void set(ExPolygons& expolygons, input_iterator_type input_begin, input_iterator_type input_end) {
            expolygons.assign(input_begin, input_end);
        }
    };
} }
// end Boost

#endif
