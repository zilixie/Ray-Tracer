#ifndef READ_JSON_H
#define READ_JSON_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
// Forward declaration
struct Object;
struct Light;

// Read a scene description from a .json file
//
// Input:
//   filename  path to .json file
// Output:
//   camera  camera looking at the scene
//   objects  list of shared pointers to objects
//   lights  list of shared pointers to lights
inline bool read_json(
  const std::string & filename, 
  Camera & camera,
  std::vector<std::shared_ptr<Object> > & objects,
  std::vector<std::shared_ptr<Light> > & lights);

// Implementation

#include <json.hpp>
#include "readSTL.h"
#include "dirname.h"
#include "Object.h"
#include "Sphere.h"
#include "Cube.h"
#include "Plane.h"
#include "Triangle.h"
#include "TriangleSoup.h"
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "read_rgba_from_png.h"
#include "QuadViewer.h"
#include "../libigl/include/igl/readOBJ.h"
#include <Eigen/Geometry>
#include <fstream>
#include <iostream>
#include <cassert>

inline bool read_json(
  const std::string & filename, 
  Camera & camera,
  std::vector<std::shared_ptr<Object> > & objects,
  std::vector<std::shared_ptr<Light> > & lights)
{
  // Heavily borrowing from
  // https://github.com/yig/graphics101-raycasting/blob/master/parser.cpp
  using json = nlohmann::json;

  std::ifstream infile( filename );
  if( !infile ) return false;
  json j;
  infile >> j;


  // parse a vector
  auto parse_Vector3d = [](const json & j) -> Eigen::Vector3d
  {
    return Eigen::Vector3d(j[0],j[1],j[2]);
  };
  // parse camera
  auto parse_camera = 
    [&parse_Vector3d](const json & j, Camera & camera)
  {
    assert(j["type"] == "perspective" && "Only handling perspective cameras");
    camera.d = j["focal_length"].get<double>();
    camera.e =  parse_Vector3d(j["eye"]);
    camera.v =  parse_Vector3d(j["up"]).normalized();
    camera.w = -parse_Vector3d(j["look"]).normalized();
    camera.u = camera.v.cross(camera.w);
    camera.height = j["height"].get<double>();
    camera.width = j["width"].get<double>();
  };
  parse_camera(j["camera"],camera);

  // Parse materials
  std::unordered_map<std::string,std::shared_ptr<Material> > materials;
  auto parse_materials = [&parse_Vector3d](
    const json & j,
    std::unordered_map<std::string,std::shared_ptr<Material> > & materials)
  {
    materials.clear();
    for(const json & jmat : j)
    {
      std::string name = jmat["name"];
      std::shared_ptr<Material> material(new Material());
      material->ka = parse_Vector3d(jmat["ka"]);
      material->kd = parse_Vector3d(jmat["kd"]);
      material->ks = parse_Vector3d(jmat["ks"]);
      material->km = parse_Vector3d(jmat["km"]);
      material->phong_exponent = jmat["phong_exponent"];
      materials[name] = material;
    }
  };
  parse_materials(j["materials"],materials);

  auto parse_lights = [&parse_Vector3d](
    const json & j,
    std::vector<std::shared_ptr<Light> > & lights)
  {
    lights.clear();
    for(const json & jlight : j)
    {
      if(jlight["type"] == "directional")
      {
        std::shared_ptr<DirectionalLight> light(new DirectionalLight());
        light->d = parse_Vector3d(jlight["direction"]).normalized();
        light->I = parse_Vector3d(jlight["color"]);
        lights.push_back(light);
      }else if(jlight["type"] == "point")
      {
        std::shared_ptr<PointLight> light(new PointLight());
        light->p = parse_Vector3d(jlight["position"]);
        light->I = parse_Vector3d(jlight["color"]);
        lights.push_back(light);
      }
    }
  };
  parse_lights(j["lights"],lights);

  auto parse_objects = [&parse_Vector3d,&filename,&materials](
    const json & j,
    std::vector<std::shared_ptr<Object> > & objects)
  {
    objects.clear();
    for(const json & jobj : j)
    {
      if(jobj["type"] == "sphere")
      {
        std::shared_ptr<Sphere> sphere(new Sphere());
        sphere->center = parse_Vector3d(jobj["center"]);
        sphere->radius = jobj["radius"].get<double>();


          sphere->texture = jobj["texture"].get<std::string>();

          if (sphere->texture != "NULL") {
              std::vector<unsigned char> rgba;
              int width, height;
              //std::cout << cube->texture;
              read_rgba_from_png(sphere->texture, rgba, width, height);

              //std::cout << "size:" << rgba.size() << " " << (int)rgba[0] << " " << (int)rgba[1] << " " << (int)rgba[2] << "\n";
              sphere->texture_data = rgba;

              sphere->texture_width = width;
              sphere->texture_height = height;
          }
          else {
              sphere->texture_height = 0;
              sphere->texture_width = 0;
          }


        objects.push_back(sphere);
      }else if(jobj["type"] == "cube")
      {
        std::shared_ptr<Cube> cube(new Cube());
        cube->v1 = parse_Vector3d(jobj["v1"]);
        cube->v8 = parse_Vector3d(jobj["v8"]);

          cube->texture = jobj["texture"].get<std::string>();

          if (cube->texture != "NULL") {
              std::vector<unsigned char> rgba;
              int width, height;
              //std::cout << cube->texture;
              read_rgba_from_png(cube->texture, rgba, width, height);

              //std::cout << "size:" << rgba.size() << " " << (int)rgba[0] << " " << (int)rgba[1] << " " << (int)rgba[2] << "\n";
              cube->texture_data = rgba;

              cube->texture_width = width;
              cube->texture_height = height;
          }
          else {
              cube->texture_height = 0;
              cube->texture_width = 0;
          }


        objects.push_back(cube);
      }else if(jobj["type"] == "plane")
      {
        std::shared_ptr<Plane> plane(new Plane());
        plane->point = parse_Vector3d(jobj["point"]);
        plane->normal = parse_Vector3d(jobj["normal"]).normalized();


          plane->texture = jobj["texture"].get<std::string>();

          if (plane->texture != "NULL") {
              std::vector<unsigned char> rgba;
              int width, height;
              //std::cout << cube->texture;
              read_rgba_from_png(plane->texture, rgba, width, height);

              //std::cout << "size:" << rgba.size() << " " << (int)rgba[0] << " " << (int)rgba[1] << " " << (int)rgba[2] << "\n";
              plane->texture_data = rgba;

              plane->texture_width = width;
              plane->texture_height = height;
          }
          else {
              plane->texture_height = 0;
              plane->texture_width = 0;
          }


        objects.push_back(plane);
      }else if(jobj["type"] == "triangle")
      {
        std::shared_ptr<Triangle> tri(new Triangle());
        tri->corners = std::make_tuple(
          parse_Vector3d(jobj["corners"][0]),
          parse_Vector3d(jobj["corners"][1]),
          parse_Vector3d(jobj["corners"][2]));
        objects.push_back(tri);
      }else if(jobj["type"] == "soup")
      {

        std::vector<std::vector<double> > V;
        std::vector<std::vector<double> > F;
        std::vector<std::vector<int> > N;
        {
#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR std::string("\\")
#else
#define PATH_SEPARATOR std::string("/")
#endif
          const std::string stl_path = jobj["stl"];
          igl::readSTL(
              igl::dirname(filename)+
              PATH_SEPARATOR +
              stl_path,
              V,F,N);
        }

        Eigen::Vector3d p = parse_Vector3d(jobj["point"]);

        std::shared_ptr<TriangleSoup> soup(new TriangleSoup());
        for(int f = 0;f<F.size();f++)
        {
          std::shared_ptr<Triangle> tri(new Triangle());
          tri->corners = std::make_tuple(
            Eigen::Vector3d( V[F[f][0]][0] + p[0], V[F[f][0]][1]+ p[1], V[F[f][0]][2]+ p[2]),
            Eigen::Vector3d( V[F[f][1]][0] + p[0], V[F[f][1]][1]+ p[1], V[F[f][1]][2]+ p[2]),
            Eigen::Vector3d( V[F[f][2]][0] + p[0], V[F[f][2]][1]+ p[1], V[F[f][2]][2] + p[2])
          );
          soup->triangles.push_back(tri);
        }
        objects.push_back(soup);



          ///////////////

/*
          Eigen::MatrixXd V, UV, NV;
          Eigen::MatrixXi F, UF, NF;
          {
#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR std::string("\\")
#else
#define PATH_SEPARATOR std::string("/")
#endif
              std::cout << "read soup \n";
              const std::string obj_path = jobj["obj"];
              std::cout << "filename:" << obj_path << "\n";
              //igl::readOBJ("d:/homework/csc418/a7/release/zero.obj", V, UV, NV, F, UF, NF);
              igl::readOBJ(obj_path, V, UV, NV, F, UF, NF);
          }

          std::shared_ptr<TriangleSoup> soup(new TriangleSoup());
          soup->texture = jobj["texture"].get<std::string>();
          std::vector<unsigned char> rgba;
          if (soup->texture != "NULL") {
              std::vector<unsigned char> rgba_kd;
              int width, height;
              read_rgba_from_png(soup->texture, rgba, width, height);
              std::cout << "texture width, height" << width << " " << height << "\n";

              rgba_kd.reserve(F.size() * 3);
              std::cout << "rgba_kd size: " << rgba_kd.size() << "\n";
              soup->texture_height = height;
              soup->texture_width = width;
              //soup->texture_data =;
          }
          else {
              soup->texture_height = 0;
              soup->texture_width = 0;
          }
          //printf("here\n");

          std::cout << "soup V, UV size:" << V.size() <<" "<<UV.size()<< "\n";
          std::cout << V[0][0] << " " << V[0][1] << " " << V[0][2] << " " << "\n";
          std::cout << V[1][0] << " " << V[1][1] << " " << V[1][2] << " " << "\n";

          std::cout << UV[0][0] << " " << UV[0][1] << " "  << "\n";
          std::cout << UV[1][0] << " " << UV[1][1] << " "  << "\n";
          std::cout << "soup face size:" << F.size()<<" " <<UF.size()<< "\n";

          {
              QuadViewer v;
              v.set_mesh6(V,F,UV,UF,NV,NF);
              v.launch();
          }

          printf("here\n");
          for (int f = 0; f<F.size(); f++)
          {
              //printf("here\n");
              std::shared_ptr<Triangle> tri(new Triangle());
              tri->corners = std::make_tuple(
                      Eigen::Vector3d(V(F(f,0),0)+1, V(F(f,0),1) - 2, V(F(f,0),2)),
                      Eigen::Vector3d(V(F(f,1),0)+1, V(F(f,1),1) - 2, V(F(f,1),2)),
                      Eigen::Vector3d(V(F(f,2),0)+1, V(F(f,2),1) - 2, V(F(f,2),2))
              );
              if (soup->texture != "NULL") {
                  double u0, v0, u1, v1, u2, v2;

                  u0 = floor(UV(UF(f,0),0) * soup->texture_width);
                  v0 = floor((UV(UF(f,0),1)/2 ) * soup->texture_height);

                  u1 = floor(UV(UF(f,1),0) * soup->texture_width);
                  v1 = floor((UV(UF(f,1),1)/2) * soup->texture_height);

                  u2 = floor(UV(UF(f,2),0) * soup->texture_width);
                  v2 = floor((UV(UF(f,2),1)/2 ) * soup->texture_height);

                  soup->texture_data.push_back((rgba[v0 * soup->texture_width * 4 + u0 * 4] + rgba[v1 * soup->texture_width * 4 + u1 * 4] + rgba[v2 * soup->texture_width * 4 + u2 * 4])/3 ) ;
                  soup->texture_data.push_back((rgba[v0 * soup->texture_width * 4 + u0 * 4 + 1] + rgba[v1 * soup->texture_width * 4 + u1 * 4 + 1] + rgba[v2 * soup->texture_width * 4 + u2 * 4 + 1])/3) ;
                  soup->texture_data.push_back((rgba[v0 * soup->texture_width * 4 + u0 * 4 + 2] + rgba[v1 * soup->texture_width * 4 + u1 * 4 + 2] + rgba[v2 * soup->texture_width * 4 + u2 * 4 + 2])/3) ;

              }
              soup->triangles.push_back(tri);
          }
          //printf("triangle number:%d\n", soup->triangles.size());
          objects.push_back(soup);*/

            ///////////////


      }
      //objects.back()->material = default_material;
      if(jobj.count("material"))
      {
        if(materials.count(jobj["material"]))
        {
          objects.back()->material = materials[jobj["material"]];
        }
      }
    }
  };
  parse_objects(j["objects"],objects);

  return true;
}

#endif 
