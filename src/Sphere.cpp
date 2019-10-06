#include "Sphere.h"
#include "Ray.h"
#include <Eigen/Geometry>
#define PI 3.1415926

bool Sphere::intersect(
        const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  Eigen::Vector3d e = ray.origin;
  Eigen::Vector3d d = ray.direction;
  Eigen::Vector3d c_minus_e = this->center - ray.origin;
  Eigen::Vector3d c = this->center;

  double r = this->radius;
  double k_sq = pow(r, 2) - (c_minus_e).dot(c_minus_e) + pow((c_minus_e.dot(d))/(d.norm()), 2);
  double k = 0;

  if (k_sq > 0) {
    k = sqrt(k_sq);
  }
  else {
    return false;
  }

  Eigen::Vector3d intersection1 = ray.origin + d*(((c_minus_e).dot(d))/(d.norm()) - k)/d.norm();
  Eigen::Vector3d intersection2 = ray.origin + d*(((c_minus_e).dot(d))/(d.norm()) + k)/d.norm();

  double t1 = (((c_minus_e).dot(d))/(d.norm()) - k)/d.norm();
  double t2 = (((c_minus_e).dot(d))/(d.norm()) + k)/d.norm();

  t = t1;
  if (t < min_t) {
    return false;
  }

  Eigen::Vector3d c_intersection1 = intersection1 - ray.origin - c_minus_e;
  Eigen::Vector3d normal = c_intersection1/c_intersection1.norm();
  n = normal;
  return true;
}

Eigen::Vector3d Sphere::set_texture_color(
        Eigen::Vector3d p) const
{

    double u, v;
    double ratio;

    if (texture_width == 0) {
        //std::cout << "shpere has no texture\n";
        return material->kd;
    }

    ratio = 0.5/radius;

    p = ratio * (p - center + Eigen::Vector3d(0, radius, 0));

    if (p(0) > 0 && p(2) > 0) {  // first region
        u = texture_width * (asin(p(2) / sqrt(pow(p(0), 2) + pow(p(2), 2)))) / (2*PI);
    }
    else if (p(0) < 0 && p(2) > 0) { // second region
        u = texture_width * (PI - asin(p(2) / sqrt(pow(p(0), 2) + pow(p(2), 2)))) / (2*PI);
    }
    else if (p(0) < 0 && p(2) < 0) {
        u = texture_width * (3*PI/2 + asin(p(2)/sqrt(pow(p(0), 2) + pow(p(2), 2)))) / (2 * PI);
    }
    else if (p(0) > 0 && p(2) < 0) { // fouth region
        u = texture_width * (2*PI + asin(p(2) / sqrt(pow(p(0), 2) + pow(p(2), 2)))) / (2 * PI);
    }

    v = p(1)*(texture_height);

    double r, g, b;
    int x, y;
    x =  floor(u);
    y =  floor(v);
    r = texture_data[y*texture_width * 4 + x * 4]/ 255.0;
    g = texture_data[y*texture_width * 4 + x * 4 + 1]/ 255.0;
    b = texture_data[y*texture_width * 4 + x * 4 + 2]/ 255.0;
    //std::cout << "x, y" << x << " " << y << " " << texture_width <<" "<<texture_height<< "\n";
    return Eigen::Vector3d(r, g, b);
    //return this->material->kd;
}