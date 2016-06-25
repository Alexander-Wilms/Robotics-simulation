/******************************************************************************
    file:             box.cpp
    created:          12.04.2004
    last modified:    01.12.2015
******************************************************************************/
#include "Box.h"

#pragma warning(disable : 4305)
#pragma warning(disable : 4136)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Box::Box()
{
    // Vertices (Eckpunkte)
    m_Verts[0].Set(0.0, 0.0, 0.0);
    m_Verts[1].Set(1.0, 0.0, 0.0);
    m_Verts[2].Set(1.0, 1.0, 0.0);
    m_Verts[3].Set(0.0, 1.0, 0.0);
    m_Verts[4].Set(0.0, 0.0, 1.0);
    m_Verts[5].Set(1.0, 0.0, 1.0);
    m_Verts[6].Set(1.0, 1.0, 1.0);
    m_Verts[7].Set(0.0, 1.0, 1.0);

    // Faces geben an, wie die Eckpunkte zu Dreiecken verbunden sind
    m_Faces[0]  = IndexedTriangle(0, 1, 5);
    m_Faces[1]  = IndexedTriangle(0, 5, 4);
    m_Faces[2]  = IndexedTriangle(1, 2, 6);
    m_Faces[3]  = IndexedTriangle(1, 6, 5);
    m_Faces[4]  = IndexedTriangle(2, 6, 7);
    m_Faces[5]  = IndexedTriangle(2, 7, 3);
    m_Faces[6]  = IndexedTriangle(7, 4, 0);
    m_Faces[7]  = IndexedTriangle(7, 0, 3);
    m_Faces[8]  = IndexedTriangle(4, 5, 6);
    m_Faces[9]  = IndexedTriangle(4, 6, 7);
    m_Faces[10] = IndexedTriangle(2, 1, 0);
    m_Faces[11] = IndexedTriangle(0, 3, 2);

    // MeshInterface initialisieren
    m_OpMeshIf.SetNbVertices(sizeof(m_Verts) / sizeof(*m_Verts));
    m_OpMeshIf.SetNbTriangles(sizeof(m_Faces) / sizeof(*m_Faces));
    m_OpMeshIf.SetPointers(m_Faces, m_Verts);

    // Opcode Modell aufbauen
    Opcode::OPCODECREATE OPCC;
    OPCC.mIMesh = &m_OpMeshIf;
    OPCC.mNoLeaf = true;
    OPCC.mQuantized = true;
    OPCC.mKeepOriginal = false;
    OPCC.mSettings.mRules = Opcode::SPLIT_BEST_AXIS | Opcode::SPLIT_GEOM_CENTER | Opcode::SPLIT_SPLATTER_POINTS;
    m_OpModel.Build(OPCC);

    // Einheitsmatrix
    m_Matrix.Identity();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Box::~Box()
{
}

void Box::Scale(float sx, float sy, float sz)
{
    // Skalieren
    for (size_t i = 0; i < sizeof(m_Verts) / sizeof(*m_Verts); ++i)
    {
        m_Verts[i].x *= sx;
        m_Verts[i].y *= sy;
        m_Verts[i].z *= sz;
    }

    // Opcode Modell neu aufbauen
    Opcode::OPCODECREATE OPCC;
    OPCC.mIMesh = &m_OpMeshIf;
    OPCC.mNoLeaf = true;
    OPCC.mQuantized = true;
    OPCC.mKeepOriginal = false;
    OPCC.mSettings.mRules = Opcode::SPLIT_BEST_AXIS | Opcode::SPLIT_GEOM_CENTER | Opcode::SPLIT_SPLATTER_POINTS;
    m_OpModel.Build(OPCC);
}

void Box::Translate(float x, float y, float z)
{
    m_Matrix.SetTrans(x, y, z);
}

void Box::RotX(float rx)
{
    m_Matrix.RotX(rx * DEGTORAD);
}

void Box::RotY(float ry)
{
    m_Matrix.RotX(ry * DEGTORAD);
}

void Box::RotZ(float rz)
{
    m_Matrix.RotX(rz * DEGTORAD);
}
