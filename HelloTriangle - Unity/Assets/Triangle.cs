using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Triangle : MonoBehaviour
{
    Mesh m;
    MeshFilter mf;
    // Use this for initialization
    void Start()
    {
        mf = GetComponent<MeshFilter>();
        m = new Mesh();
        mf.mesh = m;
        createTriangle();
    }

    private void Update()
    {
        
    }

    //This draws a triangle
    void createTriangle()
    {
        //We need two arrays one to hold the vertices and one to hold the triangles
        Vector3[] VerteicesArray = new Vector3[3];
        int[] trianglesArray = new int[3];

        //lets add 3 vertices in the 3d space
        VerteicesArray[0] = new Vector3(0, 1, 0);
        VerteicesArray[1] = new Vector3(1, 0, 0);
        VerteicesArray[2] = new Vector3(-1, 0, 0);

        //define the order in which the vertices in the VerteicesArray shoudl be used to draw the triangle
        trianglesArray[0] = 0;
        trianglesArray[1] = 1;
        trianglesArray[2] = 2;

        //add these two triangles to the mesh
        m.vertices = VerteicesArray;
        m.triangles = trianglesArray;

        Vector3[] vertices = m.vertices;

        //Create new colors array where the colors will be created.
        Color32[] colors = new Color32[vertices.Length];

        for (int i = 0; i < vertices.Length; i += 3)
        {
            colors[i] = new Color32(255, 0, 0, 255);
            colors[i + 1] = new Color32(0, 255, 0, 255);
            colors[i + 2] = new Color32(0, 0, 255, 255);
        }

        //assign the array of colors to the Mesh.
        m.colors32 = colors;
    }
}
