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
    ModelMaterialMap materials;   //��ű�����ϵļ�ֵ��
    ModelObjectPtrs objects;      //ModelObject��ָ��
    
    Model();
    Model(const Model &other);
    Model& operator= (Model other);
    void swap(Model &other);
    ~Model();
    ModelObject* add_object();
    ModelObject* add_object(const ModelObject &other, bool copy_volumes = true);
    void delete_object(size_t idx);
    void clear_objects();
    
    ModelMaterial* add_material(t_model_material_id material_id);   //���materials����û��material_id���������Ӳ����ظü���ֵ�����򷵻��ִ�ļ���ֵ
    ModelMaterial* add_material(t_model_material_id material_id, const ModelMaterial &other);  //���materials������material_id�����,��ɾ���������ֵ,Ȼ����봫��ļ�ֵ��,����ֱ�Ӽ���
    ModelMaterial* get_material(t_model_material_id material_id);  //����material_id��Ӧ��ModelMaterial* (��һ��ָ��)
    void delete_material(t_model_material_id material_id);    //���materials��material_id,������Ӧ�ļ�ֵȫ��ɾ����
    void clear_materials();   //��materials��ļ�ֵȫ��ɾ��
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
    t_model_material_attributes attributes;  //t_model_material_attribute(��string)��string��map
    DynamicPrintConfig config;

    Model* get_model() const { return this->model; };    //���ر����model
    void apply(const t_model_material_attributes &attributes);   //�����attributes����(attributes.begin(), attributes.end())
    
    private:
    Model* model;   //�Ʋ���ָ���������Model��ָ��
    
    ModelMaterial(Model *model);    //����model,�Ʋ�ָ��������Լ���Model
    ModelMaterial(Model *model, const ModelMaterial &other);     //�����attributes,config��other��һ��,����model,�Ʋ�ָ��������Լ���Model

class ModelObject
{
    friend class Model;
    public:
    std::string name;   //���ģ������
    std::string input_file;    //��������ļ���
    ModelInstancePtrs instances;   //�ö�ģ��ʵ��ָ����ɵ�vector, 
    ModelVolumePtrs volumes;    //�ö�ģ������ָ����ɵ�vector,ÿ��ģ����������һ��TriangleMesh������һ��������stl�ļ���
    DynamicPrintConfig config;  //
	t_layer_height_ranges layer_height_ranges;   //typedef std::pair<coordf_t,coordf_t> t_layer_height_range;
											     //typedef std::map<t_layer_height_range,coordf_t> t_layer_height_ranges;   
	/* �������������ͨ��center_around_origin()����Ӧ��������������ƽ�ƣ�ʹ����Ҳ����Ҫ�����ƽ��Ӧ�õ������ӵ�volumes�ϣ�
		�Ա㱣��һ������ */
    Pointf3 origin_translation;
    
    // these should be private but we need to expose them via XS until all methods are ported
    BoundingBoxf3 _bounding_box;   //��������vollumes��BoundingBoxf3
    bool _bounding_box_valid;     //˵��_bounding_box�Ƿ���Ч
    
    Model* get_model() const { return this->model; };    //���ر���洢��Model��ָ�룬�Ʋ�ָ��洢���Լ���Model
    
    ModelVolume* add_volume(const TriangleMesh &mesh);   //�����volumes����һ��ModelVolume(this, mesh)
    ModelVolume* add_volume(const ModelVolume &volume);  //�����volumes����һ��volume
    void delete_volume(size_t idx);   //ɾ����idx��volume,Ȼ��ʹ�ñ����_bounding_box_valid = false
    void clear_volumes();    //�������volumes

    ModelInstance* add_instance();   //����һ��ModelInstance�����أ�����_bounding_box_valid = false
    ModelInstance* add_instance(const ModelInstance &instance);  //����һ��Ϊinstance��ModelInstance�����أ�����_bounding_box_valid = false
    void delete_instance(size_t idx);   //ɾ����idx��instance,Ȼ��ʹ�ñ����_bounding_box_valid = false
    void delete_last_instance();   //ɾ�����һ��instance,Ȼ��ʹ�ñ����_bounding_box_valid = false
    void clear_instances();    ///�������instances

    BoundingBoxf3 bounding_box();     //������mesh()�������ص�TriangleMesh����õ�BoundingBoxf3������,ʹ��_bounding_box_valid = ture
    void invalidate_bounding_box();   //ʹ�ñ����_bounding_box_valid = false

    TriangleMesh mesh() const;   //��volumes���������stl�ļ�����instances����ƽ�ơ���ת����������Ȼ��ϲ���
    TriangleMesh raw_mesh() const;   //volumes��������з�modifier�ϲ���Ȼ�󷵻�
    BoundingBoxf3 raw_bounding_box() const;   //��volumes�������stl�ļ�����instances�ĵ�һ����ʽ����ƽ�ơ���ת����������Ȼ��ϲ�BoundingBoxf3������
    BoundingBoxf3 instance_bounding_box(size_t instance_idx) const;   //�������raw_mesh()�ķ���ֵ����instances[instance_idx]����ƽ�ơ���ת����������Ȼ�󷵻�BoundingBoxf3
    void center_around_origin();    //�ƶ����嵽����ԭ��ƽ̨�ϣ�ͬʱ��������instances(��̫���)
    void translate(const Vectorf3 &vector);  //�ƶ�����volumes���stl�ļ���ͬʱ_bounding_box��ЧҲͬʱ����
    void translate(coordf_t x, coordf_t y, coordf_t z);   //ͬ��
    void scale(const Pointf3 &versor);   //�Ŵ�����volumes���stl�ļ�scale��,ͬʱorigin_translation=(0,0,0),_bounding_box_valid = false
    void rotate(float angle, const Axis &axis);   //����volumes���stl�ļ���axisѡ��angle�Ƕ�,ͬʱorigin_translation=(0,0,0),_bounding_box_valid = false
    void flip(const Axis &axis);    //
    size_t materials_count() const;   //
    size_t facets_count() const;   //����������Ƭ��
    bool needed_repair() const;   //����volumes�����stl�Ƿ���Ҫ�޸�
    void cut(coordf_t z, Model* model) const;   //
    void split(ModelObjectPtrs* new_objects);   //
    void update_bounding_box();   // this is a private method but we expose it until we need to expose it via XS
    
    private:
    Model* model;   //�Ʋ���ָ���������Model��ָ��
    
    ModelObject(Model *model);   //����model,�Ʋ�ָ��������Լ���Model
    ModelObject(Model *model, const ModelObject &other, bool copy_volumes = true);    //�����name,input_file,config,layer_height_ranges,origin_translation,_bounding_box,_bounding_box_valid��otherһ��    ����model,�Ʋ�ָ��������Լ���Model
    ModelObject& operator= (ModelObject other);      //���������=����ʾ����name������������other
    void swap(ModelObject &other);   //����name����������other������  
    ~ModelObject();
};

class ModelVolume    //ģ�������࣬ModelObject�������,ͬʱ������TriangleMesh,���ǵ�����һ��stl����
{
    friend class ModelObject;
    public:
    std::string name;
    TriangleMesh mesh;
    DynamicPrintConfig config;
    bool modifier;   //���������ݲ����
    
    ModelObject* get_object() const { return this->object; };   //���ر��������ģ��������    
    t_model_material_id material_id() const;  //�õ�����id���ַ���
    void material_id(t_model_material_id material_id);   //���ò���id
    ModelMaterial* material() const;   //���ر���_material_id��Ӧ��ModelMaterial*  (��һ��ָ��)
    void set_material(t_model_material_id material_id, const ModelMaterial &material);   //
    
    ModelMaterial* assign_unique_material();   //
    
    private:
    ModelObject* object;                //�Ʋ���ָ���������ModelObject��ָ��
    t_model_material_id _material_id;   //ģ�Ͳ���id�����ַ���
    
    ModelVolume(ModelObject *object, const TriangleMesh &mesh);  //˽�й��캯����ֻ��ModelObject����ʹ�ã������mesh��ֵ��mesh,Ĭ��δ�޸�,����object,�Ʋ�ָ��������Լ���ModelObject
    ModelVolume(ModelObject *object, const ModelVolume &other);  //�����name,mesh config,modifier��otherһ��������object,�Ʋ�ָ��������Լ���ModelObject
};

class ModelInstance    //ģ��ʵ���࣬ModelObject�������,Ӧ���ǰ������е�ƽ�ƣ���ת�������Ķ�������ʵ��TriangleMesh��Polygon�Ķ�Ӧ����
{
    friend class ModelObject;
    public:
    double rotation;            // ��z��Ļ���
    double scaling_factor;      //����ϵ��
    Pointf offset;              // δ����������ϵ�е�ƫ��
    
    ModelObject* get_object() const { return this->object; };      //���ر��������ģ��������    
    void transform_mesh(TriangleMesh* mesh, bool dont_translate = false) const;    //��mesh����rotation��z����ת,sacling_factor����,����dont_translate�����Ƿ���offsetƽ��
    void transform_polygon(Polygon* polygon) const;    //��polygon����ԭ����תrotation,����scaling_factor
    
    private:
    ModelObject* object;   //�Ʋ���ָ���������ModelObject��ָ��
    
    ModelInstance(ModelObject *object);  //˽�й��캯����ֻ��ModelObject����ʹ�ã�Ĭ��û����ת�Ƕȣ�����ϵ����1,����object,�Ʋ�ָ��������Լ���ModelObject
    ModelInstance(ModelObject *object, const ModelInstance &other);   //����other��������Զ���rotation,scaling_factor,offset�Ĵ�С,����object,�Ʋ�ָ��������Լ���ModelObject
};

}

#endif
