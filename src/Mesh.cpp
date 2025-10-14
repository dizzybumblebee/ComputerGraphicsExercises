//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

//== INCLUDES =================================================================

#include "Mesh.h"
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <filesystem>
#include <array>
#include "vec3.h"


//== IMPLEMENTATION ===========================================================


Mesh::Mesh(std::istream &is, const std::filesystem::path &_scene_path)
{
    std::string meshFilename, mode;
    is >> meshFilename;
    auto offFilename = _scene_path.parent_path() / meshFilename;

    // load mesh from file
    read(offFilename);

    is >> mode;
    if      (mode ==  "FLAT") draw_mode_ = FLAT;
    else if (mode == "PHONG") draw_mode_ = PHONG;
    else throw std::runtime_error("Invalid draw mode " + mode);

    is >> material;
}


//-----------------------------------------------------------------------------


bool Mesh::read(const std::filesystem::path &_filename)
{
    // read a mesh in OFF format


    // open file
    std::ifstream ifs(_filename);
    if (!ifs)
    {
        std::cerr << "Can't open " << _filename << "\n";
        return false;
    }


    // read OFF header
    std::string s;
    unsigned int nV, nF, dummy, i;
    ifs >> s;
    if (s != "OFF")
    {
        std::cerr << "No OFF file\n";
        return false;
    }
    ifs >> nV >> nF >> dummy;
    std::cout << "\n  read " << _filename << ": " << nV << " vertices, " << nF << " triangles";


    // read vertices
    Vertex v;
    vertices_.clear();
    vertices_.reserve(nV);
    for (i=0; i<nV; ++i)
    {
        ifs >> v.position;
        vertices_.push_back(v);
    }


    // read triangles
    Triangle t;
    triangles_.clear();
    triangles_.reserve(nF);
    for (i=0; i<nF; ++i)
    {
        ifs >> dummy >> t.i0 >> t.i1 >> t.i2;
        triangles_.push_back(t);
    }


    // close file
    ifs.close();


    // compute face and vertex normals
    compute_normals();

    // compute bounding box
    compute_bounding_box();


    return true;
}


//-----------------------------------------------------------------------------

// Determine the weights by which to scale triangle (p0, p1, p2)'s normal when
// accumulating the vertex normals for vertices 0, 1, and 2.
// (Recall, vertex normals are a weighted average of their incident triangles'
// normals, and in our raytracer we'll use the incident angles as weights.)
// \param[in] p0, p1, p2    triangle vertex positions
// \param[out] w0, w1, w2    weights to be used for vertices 0, 1, and 2
void angleWeights(const vec3 &p0, const vec3 &p1, const vec3 &p2,
                  double &w0, double &w1, double &w2) {
    // compute angle weights
    const vec3 e01 = normalize(p1-p0);
    const vec3 e12 = normalize(p2-p1);
    const vec3 e20 = normalize(p0-p2);
    w0 = acos( std::max(-1.0, std::min(1.0, dot(e01, -e20) )));
    w1 = acos( std::max(-1.0, std::min(1.0, dot(e12, -e01) )));
    w2 = acos( std::max(-1.0, std::min(1.0, dot(e20, -e12) )));
}


//-----------------------------------------------------------------------------

void Mesh::compute_normals()
{

    /** \todo
     * In some scenes (e.g the office scene) some objects should be flat
     * shaded (e.g. the desk) while other objects should be Phong shaded to appear
     * realistic (e.g. chairs). You have to implement the following:
     * - Compute vertex normals by averaging the normals of their incident triangles.
     * - Store the vertex normals in the Vertex::normal member variable.
     * - Weigh the normals by their triangles' angles.
     */

    // initialize vertex normals to zero
    for (Vertex& v: vertices_)
    {
        v.normal = vec3(0,0,0);
    }

    // compute triangle normals and weights, compute vertices normals
    for (Triangle& t: triangles_)
    {
        const vec3& p0 = vertices_[t.i0].position;
        const vec3& p1 = vertices_[t.i1].position;
        const vec3& p2 = vertices_[t.i2].position;
        t.normal = normalize(cross(p1-p0, p2-p0));

        std::array<double, 3> weights;
        angleWeights(p0, p1, p2, weights[0], weights[1], weights[2]);

        vertices_[t.i0].normal += t.normal*weights[0];
        vertices_[t.i1].normal += t.normal*weights[1];
        vertices_[t.i2].normal += t.normal*weights[2];

    }

    // normalize normals
    for (Vertex& v: vertices_)
    {
        v.normal = normalize(v.normal);
    }




}


//-----------------------------------------------------------------------------


void Mesh::compute_bounding_box()
{
    bb_min_ = vec3(std::numeric_limits<double>::max());
    bb_max_ = vec3(std::numeric_limits<double>::lowest());

    for (Vertex v: vertices_)
    {
        bb_min_ = min(bb_min_, v.position);
        bb_max_ = max(bb_max_, v.position);
    }
}


//-----------------------------------------------------------------------------


bool Mesh::intersect_bounding_box(const Ray& _ray) const
{

    /** \todo
    * Intersect the ray `_ray` with the axis-aligned bounding box of the mesh.
    * Note that the minimum and maximum point of the bounding box are stored
    * in the member variables `bb_min_` and `bb_max_`. Return whether the ray
    * intersects the bounding box.
    * This function is ued in `Mesh::intersect()` to avoid the intersection test
    * with all triangles of every mesh in the scene. The bounding boxes are computed
    * in `Mesh::compute_bounding_box()`.
    */

    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    for (int i = 0; i < 3; i++)
    {
        // we need to handle the ray parallel to slab (direction is close to zero)
        if(std::abs(_ray.direction[i])  < 1e-6) {

            if(_ray.origin[i] < bb_min_[i] || _ray.origin[i] > bb_max_[i]) return false;

        }
        double t1, t2;

        t1 = (bb_min_[i] - _ray.origin[i]) / _ray.direction[i];
        t2 = (bb_max_[i] - _ray.origin[i]) / _ray.direction[i];

        if (t1 > t2) std::swap(t1, t2);

        t_min = std::min(t1, t_min);
        t_max = std::max(t2, t_max);

        //no intersection
        if (t_min > t_max) return false;

        //intersection is behind ray origin
        if (t_max < 0) return false;


    }
    return true;
}


//-----------------------------------------------------------------------------


bool Mesh::intersect(const Ray& _ray,
                     vec3&      _intersection_point,
                     vec3&      _intersection_normal,
                     double&    _intersection_t ) const
{
    // check bounding box intersection
    if (!intersect_bounding_box(_ray))
    {
        return false;
    }

    vec3   p, n;
    double t;

    _intersection_t = NO_INTERSECTION;

    // for each triangle
    for (const Triangle& triangle : triangles_)
    {
        // does ray intersect triangle?
        if (intersect_triangle(triangle, _ray, p, n, t))
        {
            // is intersection closer than previous intersections?
            if (t < _intersection_t)
            {
                // store data of this intersection
                _intersection_t      = t;
                _intersection_point  = p;
                _intersection_normal = n;
            }
        }
    }

    return (_intersection_t != NO_INTERSECTION);
}


//-----------------------------------------------------------------------------


bool
    Mesh::
    intersect_triangle(const Triangle&  _triangle,
                       const Ray&       _ray,
                       vec3&            _intersection_point,
                       vec3&            _intersection_normal,
                       double&          _intersection_t) const
{
    const vec3& p0 = vertices_[_triangle.i0].position;
    const vec3& p1 = vertices_[_triangle.i1].position;
    const vec3& p2 = vertices_[_triangle.i2].position;

    vec3 columnVector = (p0 - _ray.origin);

    //calculate determinants
    double detA = determinant(_ray.direction,(p0 - p1),(p0 - p2));
    double detT = determinant(columnVector,(p0 - p1),(p0 - p2));
    double detBeta = determinant(_ray.direction,columnVector,(p0 - p2));
    double detGamma = determinant(_ray.direction,(p0 - p1),columnVector);

    //get solutions via cramer's rule
    double t = detT / detA;
    double beta = detBeta / detA;
    double gamma = detGamma / detA;

    //no need for further computation if any of the following apply
    if (t <= 0 || beta < 0 || gamma < 0 || beta + gamma > 1) {
        return false;
    }

    _intersection_t = t;
    _intersection_point = t * _ray.direction + _ray.origin;

    if (draw_mode_ == FLAT) {
        _intersection_normal = _triangle.normal;
    }
    else if (draw_mode_ == PHONG) { //interpolate vertex normals
        double alpha = 1 - beta - gamma;
        const Vertex& v0 = vertices_[_triangle.i0];
        const Vertex& v1 = vertices_[_triangle.i1];
        const Vertex& v2 = vertices_[_triangle.i2];

        _intersection_normal = normalize(alpha*v0.normal + beta*v1.normal + gamma*v2.normal);
    }

    return true;
}

double
    Mesh::
    determinant(vec3 v1, vec3 v2, vec3 v3) const {

    double posProdSum[3] = {(v1[0]*v2[1]*v3[2]),(v2[0]*v3[1]*v1[2]),(v3[0]*v1[1]*v2[2])};
    double negProdSum[3] = {(v3[0]*v2[1]*v1[2]),(v2[0]*v1[1]*v3[2]),(v1[0]*v3[1]*v2[2])};

    double sum1 = 0;
    double sum2 = 0;

    for (int i = 0; i < 3; i++) {
        sum1 += posProdSum[i];
        sum2 += negProdSum[i];
    }
    return (sum1 - sum2);
}

//=============================================================================
