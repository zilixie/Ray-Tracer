#include "blinn_phong_shading.h"
// Hint:
#include "first_hit.h"
#include <iostream>
#include <algorithm>

Eigen::Vector3d blinn_phong_shading(
  const Ray & ray,
  const int & hit_id,
  const double & t,
  const Eigen::Vector3d & n,
  const std::vector< std::shared_ptr<Object> > & objects,
  const std::vector<std::shared_ptr<Light> > & lights)
{
  Eigen::Vector3d query_point = ray.origin + t * ray.direction;
  Eigen::Vector3d direction_query_point_to_light;

  double parametric_distance_t;
  Eigen::Vector3d L = Eigen::Vector3d(0,0,0);

  Eigen::Vector3d L_shadow = Eigen::Vector3d(0,0,0);
  Eigen::Vector3d ka = objects[hit_id]->material->ka;


  for (int j=0; j<lights.size(); j++) {
    Eigen::Vector3d i = lights[j]->I;
    lights[j]->direction(query_point, direction_query_point_to_light, parametric_distance_t);

    Eigen::Vector3d l = direction_query_point_to_light;

    Eigen::Vector3d v = (-1) * ray.direction/(ray.direction).norm();
    Eigen::Vector3d h = (v + l) / ((v + l).norm());

    Eigen::Vector3d kd = objects[hit_id]->material->kd;
    Eigen::Vector3d ks = objects[hit_id]->material->ks;

    double exp = objects[hit_id]->material->phong_exponent;

    Eigen::Vector3d diffuse = fmax(0, n.dot(l)) * (i.array() * kd.array()).matrix();
    Eigen::Vector3d specular = pow(fmax(0, n.dot(h)), exp) * (i.array() * ks.array()).matrix();

    Ray shadow_ray;
    int shadow_hit_id;
    double shadow_t;
    Eigen::Vector3d shadow_n;
    shadow_ray.origin = query_point;
    shadow_ray.direction = l;

    bool this_light_shadow_hit = first_hit(shadow_ray, 0.000001, objects, shadow_hit_id, shadow_t,shadow_n);
    if ((shadow_t < parametric_distance_t) && this_light_shadow_hit) {
      L = L + L_shadow;
    }
    else {
      L = L + diffuse + specular;
    }
  }
  return L + ka;
}
