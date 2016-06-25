#ifndef __Box_H__
#define __Box_H__

#include <Opcode.h>

class Box
{
public:
    // Constructor / Destructor
    Box();
    ~Box();

    // Data access
    inline Point*           GetVerts() { return m_Verts; }
    inline IndexedTriangle* GetFaces() { return m_Faces; }

    void Scale(float sx, float sy, float sz);
    void Translate(float x, float y, float z);
    void RotX(float rx);
    void RotY(float ry);
    void RotZ(float rz);

    Opcode::Model m_OpModel;
    Matrix4x4     m_Matrix;

protected:
    Opcode::MeshInterface m_OpMeshIf;
    Point                 m_Verts[8];
    IndexedTriangle       m_Faces[12];
};

#endif // __Box_H__
