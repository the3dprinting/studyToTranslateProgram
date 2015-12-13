#ifndef slic3r_Layer_hpp_
#define slic3r_Layer_hpp_

#include <myinit.h>
#include "Flow.hpp"
#include "SurfaceCollection.hpp"
#include "ExtrusionEntityCollection.hpp"
#include "ExPolygonCollection.hpp"
#include "PolylineCollection.hpp"


namespace Slic3r {

typedef std::pair<coordf_t,coordf_t> t_layer_height_range;   //
typedef std::map<t_layer_height_range,coordf_t> t_layer_height_ranges;   //model里面用到了

class Layer;
class PrintRegion;
class PrintObject;


// TODO: make stuff private
class LayerRegion    //实现方法在LayerRegion.cpp里面
{
    friend class Layer;

    public:
    Layer* layer();    //返回本身_layer这个指针
    PrintRegion* region();    //返回本身_region这个指针

    SurfaceCollection slices;  //收集通过slice得到的原几何surfaces,分为top/bottom/internal

    ExtrusionEntityCollection thin_fills;   //收集填充gaps地方的paths/loops 

    SurfaceCollection fill_surfaces;  //收集为了生成填充线的surfaces

    ExPolygonCollection bridged;  //收集代表bridged区域的expolygons（因此不需要支撑，也可以是Polygons对象）

    PolylineCollection unsupported_bridge_edges;   //收集polylines代表非支撑桥的边

    ExtrusionEntityCollection perimeters;     //按序列收集所有生成周长的paths/loops（这个收集结果仅仅包含ExtrusionEntityCollection对象）

    ExtrusionEntityCollection fills;    //按序列收集填充surfaces的paths（这个收集结果仅仅包含ExtrusionEntityCollection对象）
    
    Flow flow(FlowRole role, bool bridge = false, double width = -1) const;   //将输入参数和_layer的相关参数赋给_region->flow(...)并返回
    void merge_slices();    //将slices里面的polygons全部布尔并，变成一个个expolygon再加到清空后的slices里面
    void prepare_fill_surfaces();    //将区域的属性根据配置做相应的修改,为下一步的填充做准备
    
    private:
    Layer *_layer;   //指向使用它本身的Layer类
    PrintRegion *_region;   //指向使用它本身的PrintRegion类

    LayerRegion(Layer *layer, PrintRegion *region);   //私有构造函数，为了让layer拥有其本身,同时一个PrintRegion用来返回Flow，因为PrintRegion代表拥有相同打印配置的一块区域，函数实现在PrintRegion.cpp里面
    ~LayerRegion();
};


typedef std::vector<LayerRegion*> LayerRegionPtrs;   //LayerRegion指针组成的向量组

class Layer {    //实现方法在Layer.cpp里面
    friend class PrintObject;

    public:
    size_t id() const;        //返回_id的值
    void set_id(size_t id);   //设置本身的_id
    PrintObject* object();    //返回本身的_object这个指针
    const PrintObject* object() const;    //返回本身的_object这个指针,不能不进行修改

    Layer *upper_layer;          
    Layer *lower_layer;
    LayerRegionPtrs regions;    //存放LayerRegion指针组成的向量组
    bool slicing_errors;    
    coordf_t slice_z;       // 在没有放缩的坐标系中用来切片的z值
    coordf_t print_z;       // 在没有放缩的坐标系中用来打印的z值
    coordf_t height;        // 在没有放缩的坐标系中的层高度

    // 收集通过原始几何形状切片生成的expolygons
    // 也被称为'islands' (all regions and surface types are merged here)
    ExPolygonCollection slices;    //原来切片后数据跑到这里了……


    size_t region_count();     //返回regions的数目
    LayerRegion* get_region(int idx);    //根据idx返回regions对应的LayerRegion指针
    LayerRegion* add_region(PrintRegion* print_region);   //在regions后面加一个根据print_region构造的LayerRegion的指针并返回
    
    void make_slices();     //使得regions里面所有的slices合并排序后放到this->slices里面
    void merge_slices();    //使得regions里面的每一个LayerRegion调用merge_slices()这个函数
    template <class T> bool any_internal_region_slice_contains(const T &item) const;
    template <class T> bool any_bottom_region_slice_contains(const T &item) const;
    
    protected:   //之所以使用protected，是因为子类SupportLayer需要使用这些方法
    size_t _id;     // sequential number of layer, 0-based
    PrintObject *_object;   //


    Layer(size_t id, PrintObject *object, coordf_t height, coordf_t print_z,
        coordf_t slice_z);
    virtual ~Layer();

    void clear_regions();     //删除全部regions的内容
    void delete_region(int idx);   //删除regions内第idx对应的LayerRegion指针
};


class SupportLayer : public Layer {  //基本跟Layer一样！
    friend class PrintObject;

    public:
    ExPolygonCollection support_islands;
    ExtrusionEntityCollection support_fills;
    ExtrusionEntityCollection support_interface_fills;

    protected:
    SupportLayer(size_t id, PrintObject *object, coordf_t height, coordf_t print_z,
        coordf_t slice_z);
    virtual ~SupportLayer();
};


}

#endif
