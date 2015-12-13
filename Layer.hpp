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
typedef std::map<t_layer_height_range,coordf_t> t_layer_height_ranges;   //model�����õ���

class Layer;
class PrintRegion;
class PrintObject;


// TODO: make stuff private
class LayerRegion    //ʵ�ַ�����LayerRegion.cpp����
{
    friend class Layer;

    public:
    Layer* layer();    //���ر���_layer���ָ��
    PrintRegion* region();    //���ر���_region���ָ��

    SurfaceCollection slices;  //�ռ�ͨ��slice�õ���ԭ����surfaces,��Ϊtop/bottom/internal

    ExtrusionEntityCollection thin_fills;   //�ռ����gaps�ط���paths/loops 

    SurfaceCollection fill_surfaces;  //�ռ�Ϊ����������ߵ�surfaces

    ExPolygonCollection bridged;  //�ռ�����bridged�����expolygons����˲���Ҫ֧�ţ�Ҳ������Polygons����

    PolylineCollection unsupported_bridge_edges;   //�ռ�polylines�����֧���ŵı�

    ExtrusionEntityCollection perimeters;     //�������ռ����������ܳ���paths/loops������ռ������������ExtrusionEntityCollection����

    ExtrusionEntityCollection fills;    //�������ռ����surfaces��paths������ռ������������ExtrusionEntityCollection����
    
    Flow flow(FlowRole role, bool bridge = false, double width = -1) const;   //�����������_layer����ز�������_region->flow(...)������
    void merge_slices();    //��slices�����polygonsȫ�������������һ����expolygon�ټӵ���պ��slices����
    void prepare_fill_surfaces();    //����������Ը�����������Ӧ���޸�,Ϊ��һ���������׼��
    
    private:
    Layer *_layer;   //ָ��ʹ���������Layer��
    PrintRegion *_region;   //ָ��ʹ���������PrintRegion��

    LayerRegion(Layer *layer, PrintRegion *region);   //˽�й��캯����Ϊ����layerӵ���䱾��,ͬʱһ��PrintRegion��������Flow����ΪPrintRegion����ӵ����ͬ��ӡ���õ�һ�����򣬺���ʵ����PrintRegion.cpp����
    ~LayerRegion();
};


typedef std::vector<LayerRegion*> LayerRegionPtrs;   //LayerRegionָ����ɵ�������

class Layer {    //ʵ�ַ�����Layer.cpp����
    friend class PrintObject;

    public:
    size_t id() const;        //����_id��ֵ
    void set_id(size_t id);   //���ñ����_id
    PrintObject* object();    //���ر����_object���ָ��
    const PrintObject* object() const;    //���ر����_object���ָ��,���ܲ������޸�

    Layer *upper_layer;          
    Layer *lower_layer;
    LayerRegionPtrs regions;    //���LayerRegionָ����ɵ�������
    bool slicing_errors;    
    coordf_t slice_z;       // ��û�з���������ϵ��������Ƭ��zֵ
    coordf_t print_z;       // ��û�з���������ϵ��������ӡ��zֵ
    coordf_t height;        // ��û�з���������ϵ�еĲ�߶�

    // �ռ�ͨ��ԭʼ������״��Ƭ���ɵ�expolygons
    // Ҳ����Ϊ'islands' (all regions and surface types are merged here)
    ExPolygonCollection slices;    //ԭ����Ƭ�������ܵ������ˡ���


    size_t region_count();     //����regions����Ŀ
    LayerRegion* get_region(int idx);    //����idx����regions��Ӧ��LayerRegionָ��
    LayerRegion* add_region(PrintRegion* print_region);   //��regions�����һ������print_region�����LayerRegion��ָ�벢����
    
    void make_slices();     //ʹ��regions�������е�slices�ϲ������ŵ�this->slices����
    void merge_slices();    //ʹ��regions�����ÿһ��LayerRegion����merge_slices()�������
    template <class T> bool any_internal_region_slice_contains(const T &item) const;
    template <class T> bool any_bottom_region_slice_contains(const T &item) const;
    
    protected:   //֮����ʹ��protected������Ϊ����SupportLayer��Ҫʹ����Щ����
    size_t _id;     // sequential number of layer, 0-based
    PrintObject *_object;   //


    Layer(size_t id, PrintObject *object, coordf_t height, coordf_t print_z,
        coordf_t slice_z);
    virtual ~Layer();

    void clear_regions();     //ɾ��ȫ��regions������
    void delete_region(int idx);   //ɾ��regions�ڵ�idx��Ӧ��LayerRegionָ��
};


class SupportLayer : public Layer {  //������Layerһ����
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
