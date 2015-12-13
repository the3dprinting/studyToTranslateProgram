#ifndef slic3r_Model_hpp_
#define slic3r_Model_hpp_

#include <myinit.h>
#include "PrintConfig.hpp"
#include "Layer.hpp"
#include "Point.hpp"
#include "TriangleMesh.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Slic3r {

class ModelInstance;
class ModelMaterial;
class ModelObject;
class ModelVolume;

typedef std::string t_model_material_id;
typedef std::string t_model_material_attribute;
typedef std::map<t_model_material_attribute,std::string> t_model_material_attributes;

typedef std::map<t_model_material_id,ModelMaterial*> ModelMaterialMap;
typedef std::vector<ModelObject*> ModelObjectPtrs;
typedef std::vector<ModelVolume*> ModelVolumePtrs;
typedef std::vector<ModelInstance*> ModelInstancePtrs;

class Model
{
    public:
    ModelMaterialMap materials;   //存放本身材料的键值对
    ModelObjectPtrs objects;      //ModelObject的指针
    
    Model();
    Model(const Model &other);
    Model& operator= (Model other);
    void swap(Model &other);
    ~Model();
    ModelObject* add_object();
    ModelObject* add_object(const ModelObject &other, bool copy_volumes = true);
    void delete_object(size_t idx);
    void clear_objects();
    
    ModelMaterial* add_material(t_model_material_id material_id);   //如果materials里面没有material_id这个键则添加并返回该键的值，否则返回现存的键的值
    ModelMaterial* add_material(t_model_material_id material_id, const ModelMaterial &other);  //如果materials里面有material_id这个键,则删除这个键的值,然后加入传入的键值对,否则直接加入
    ModelMaterial* get_material(t_model_material_id material_id);  //返回material_id对应的ModelMaterial* (即一个指针)
    void delete_material(t_model_material_id material_id);    //如果materials有material_id,则把其对应的键值全部删除！
    void clear_materials();   //将materials里的键值全部删除
    // void duplicate_objects_grid(unsigned int x, unsigned int y, coordf_t distance);
    // void duplicate_objects(size_t copies_num, coordf_t distance, const BoundingBox &bb);
    // void arrange_objects(coordf_t distance, const BoundingBox &bb);
    // void duplicate(size_t copies_num, coordf_t distance, const BoundingBox &bb);
    bool has_objects_with_no_instances() const;
    bool add_default_instances();
    BoundingBoxf3 bounding_box() const;
    void center_instances_around_point(const Pointf &point);
    void align_instances_to_origin();
    void translate(coordf_t x, coordf_t y, coordf_t z);
    TriangleMesh mesh() const;
    TriangleMesh raw_mesh() const;
    // std::string get_material_name(t_model_material_id material_id);

    
    private:
    void _arrange(const std::vector<Size> &sizes, coordf_t distance, const BoundingBox &bb) const;
};

class ModelMaterial
{
    friend class Model;
    public:
    t_model_material_attributes attributes;  //t_model_material_attribute(即string)→string的map
    DynamicPrintConfig config;

    Model* get_model() const { return this->model; };    //返回本身的model
    void apply(const t_model_material_attributes &attributes);   //本身的attributes加入(attributes.begin(), attributes.end())
    
    private:
    Model* model;   //推测存放指向包含它的Model的指针
    
    ModelMaterial(Model *model);    //传入model,推测指向包含它自己的Model
    ModelMaterial(Model *model, const ModelMaterial &other);     //本身的attributes,config和other的一样,传入model,推测指向包含它自己的Model

class ModelObject
{
    friend class Model;
    public:
    std::string name;   //存放模型名字
    std::string input_file;    //存放输入文件名
    ModelInstancePtrs instances;   //好多模型实例指针组成的vector, 
    ModelVolumePtrs volumes;    //好多模型容器指针组成的vector,每个模型容器都有一个TriangleMesh，就是一个单独的stl文件。
    DynamicPrintConfig config;  //
	t_layer_height_ranges layer_height_ranges;   //typedef std::pair<coordf_t,coordf_t> t_layer_height_range;
											     //typedef std::map<t_layer_height_range,coordf_t> t_layer_height_ranges;   
	/* 这个向量计算了通过center_around_origin()方法应用与这个物体的总平移，使用者也许想要将这个平移应用到新增加的volumes上，
		以便保持一致排列 */
    Pointf3 origin_translation;
    
    // these should be private but we need to expose them via XS until all methods are ported
    BoundingBoxf3 _bounding_box;   //本身所有vollumes的BoundingBoxf3
    bool _bounding_box_valid;     //说明_bounding_box是否有效
    
    Model* get_model() const { return this->model; };    //返回本身存储的Model类指针，推测指向存储它自己的Model
    
    ModelVolume* add_volume(const TriangleMesh &mesh);   //本身的volumes增加一个ModelVolume(this, mesh)
    ModelVolume* add_volume(const ModelVolume &volume);  //本身的volumes增加一个volume
    void delete_volume(size_t idx);   //删除第idx个volume,然后使得本身的_bounding_box_valid = false
    void clear_volumes();    //清空所有volumes

    ModelInstance* add_instance();   //增加一个ModelInstance并返回，其中_bounding_box_valid = false
    ModelInstance* add_instance(const ModelInstance &instance);  //增加一个为instance的ModelInstance并返回，其中_bounding_box_valid = false
    void delete_instance(size_t idx);   //删除第idx个instance,然后使得本身的_bounding_box_valid = false
    void delete_last_instance();   //删除最后一个instance,然后使得本身的_bounding_box_valid = false
    void clear_instances();    ///清空所有instances

    BoundingBoxf3 bounding_box();     //将本身mesh()函数返回的TriangleMesh计算得到BoundingBoxf3并返回,使得_bounding_box_valid = ture
    void invalidate_bounding_box();   //使得本身的_bounding_box_valid = false

    TriangleMesh mesh() const;   //将volumes里面的所有stl文件按照instances进行平移、旋转、放缩操作然后合并！
    TriangleMesh raw_mesh() const;   //volumes里面的所有非modifier合并，然后返回
    BoundingBoxf3 raw_bounding_box() const;   //将volumes里的所有stl文件按照instances的第一个方式进行平移、旋转、放缩操作然后合并BoundingBoxf3并返回
    BoundingBoxf3 instance_bounding_box(size_t instance_idx) const;   //将本身的raw_mesh()的返回值按照instances[instance_idx]进行平移、旋转、放缩操作然后返回BoundingBoxf3
    void center_around_origin();    //移动物体到中心原点平台上，同时更新所有instances(不太理解)
    void translate(const Vectorf3 &vector);  //移动所有volumes里的stl文件，同时_bounding_box有效也同时更新
    void translate(coordf_t x, coordf_t y, coordf_t z);   //同上
    void scale(const Pointf3 &versor);   //放大所有volumes里的stl文件scale倍,同时origin_translation=(0,0,0),_bounding_box_valid = false
    void rotate(float angle, const Axis &axis);   //所有volumes里的stl文件绕axis选择angle角度,同时origin_translation=(0,0,0),_bounding_box_valid = false
    void flip(const Axis &axis);    //
    size_t materials_count() const;   //
    size_t facets_count() const;   //返回所有面片数
    bool needed_repair() const;   //返回volumes里面的stl是否需要修复
    void cut(coordf_t z, Model* model) const;   //
    void split(ModelObjectPtrs* new_objects);   //
    void update_bounding_box();   // this is a private method but we expose it until we need to expose it via XS
    
    private:
    Model* model;   //推测存放指向包含它的Model的指针
    
    ModelObject(Model *model);   //传入model,推测指向包含它自己的Model
    ModelObject(Model *model, const ModelObject &other, bool copy_volumes = true);    //本身的name,input_file,config,layer_height_ranges,origin_translation,_bounding_box,_bounding_box_valid与other一样    传入model,推测指向包含它自己的Model
    ModelObject& operator= (ModelObject other);      //重载运算符=，表示除了name，其他都等于other
    void swap(ModelObject &other);   //除了name，其他都和other交换了  
    ~ModelObject();
};

class ModelVolume    //模型容器类，ModelObject类包含它,同时它包含TriangleMesh,就是单独的一个stl物体
{
    friend class ModelObject;
    public:
    std::string name;
    TriangleMesh mesh;
    DynamicPrintConfig config;
    bool modifier;   //调节器，暂不清楚
    
    ModelObject* get_object() const { return this->object; };   //返回本身包含的模型物体类    
    t_model_material_id material_id() const;  //得到材料id的字符串
    void material_id(t_model_material_id material_id);   //设置材料id
    ModelMaterial* material() const;   //返回本身_material_id对应的ModelMaterial*  (即一个指针)
    void set_material(t_model_material_id material_id, const ModelMaterial &material);   //
    
    ModelMaterial* assign_unique_material();   //
    
    private:
    ModelObject* object;                //推测存放指向包含它的ModelObject的指针
    t_model_material_id _material_id;   //模型材料id，即字符串
    
    ModelVolume(ModelObject *object, const TriangleMesh &mesh);  //私有构造函数！只有ModelObject可以使用，输入的mesh赋值给mesh,默认未修改,传入object,推测指向包含它自己的ModelObject
    ModelVolume(ModelObject *object, const ModelVolume &other);  //本身的name,mesh config,modifier跟other一样！传入object,推测指向包含它自己的ModelObject
};

class ModelInstance    //模型实例类，ModelObject类包含它,应该是包含所有的平移，旋转，放缩的动作，并实现TriangleMesh和Polygon的对应动作
{
    friend class ModelObject;
    public:
    double rotation;            // 绕z轴的弧度
    double scaling_factor;      //放缩系数
    Pointf offset;              // 未放缩的坐标系中的偏置
    
    ModelObject* get_object() const { return this->object; };      //返回本身包含的模型物体类    
    void transform_mesh(TriangleMesh* mesh, bool dont_translate = false) const;    //将mesh按照rotation绕z轴旋转,sacling_factor放缩,根据dont_translate决定是否按照offset平移
    void transform_polygon(Polygon* polygon) const;    //将polygon按照原点旋转rotation,放缩scaling_factor
    
    private:
    ModelObject* object;   //推测存放指向包含它的ModelObject的指针
    
    ModelInstance(ModelObject *object);  //私有构造函数！只有ModelObject可以使用，默认没有旋转角度，放缩系数是1,传入object,推测指向包含它自己的ModelObject
    ModelInstance(ModelObject *object, const ModelInstance &other);   //根据other本身的属性定义rotation,scaling_factor,offset的大小,传入object,推测指向包含它自己的ModelObject
};

}

#endif
