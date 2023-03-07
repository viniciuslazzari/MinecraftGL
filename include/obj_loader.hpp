#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "tiny_obj_loader/index.h"

class ObjModel {
    private:
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

    public:
        ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);
        void ComputeNormals();
        void BuildTrianglesAndAddToVirtualScene();
        void DrawVirtualObject(const char* object_name);
};

#endif