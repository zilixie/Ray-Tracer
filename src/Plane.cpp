#include <iostream>
#include "Plane.h"
#include "Ray.h"

bool Plane::intersect(
        const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  t = 0;
  Eigen::Vector3d e = ray.origin;
  Eigen::Vector3d d = ray.direction;
  Eigen::Vector3d n_ = this->normal;

  Eigen::Vector3d p = this->point;
  Eigen::Vector3d a = p - e;

  double product = ((-1) * n_).dot(a);
  double cos_theta = product/(a.norm()*n_.norm());

  Eigen::Vector3d h = cos_theta * a.norm() * (-1) * n_;

  double cos_beta = d.dot(h)/(d.norm() * h.norm());
  double t_ = (h.norm()/cos_beta)/d.norm();
  t = t_;
  n = n_;

  if (t==std::numeric_limits<double>::infinity()) {
    return false;
  }
  if ((t < min_t) || (isnan(t))){
    return false;
  }
  return true;

}


Eigen::Vector3d Plane::set_texture_color(
        Eigen::Vector3d p) const
{
    if (texture_width == 0 ) {
        return this->material->kd;
    }

    //printf("%d\n", texture_width);
    //printf("%d\n", texture_height);
    double x, y, z, u, v, r, g, b, ratio_u, ratio_v;
    int i, j;

    if (normal(0) != 0) {
        x = p(0);
        y = p(1) + 200;
        z = p(2) + 200;
        if (z>400 || y>400 ||z<0 ||y<0)
            return material->kd;

        ratio_u = texture_width / 400;
        ratio_v = texture_height / 400;
    }

    if (normal(1) != 0) {
        x = p(0) + 200;
        y = p(1);
        z = p(2) + 200;
        //std::cout << "x,y,z:" << x << " " << z << " " << y << "\n";

        if (x > 400 || z > 400 || x < 0 || z < 0) {
            //std::cout << "out of bound:  "<<x<<" "<<z<< "\n";
            return this->material->kd;
        }
        ratio_u = texture_width / 400;
        ratio_v = texture_height / 400;
    }

    if (normal(2) != 0) {
        x = p(0) + 200;
        y = p(1) + 200;
        z = p(2);
        if (x > 400 || y > 400 || x < 0 || y < 0) {
            //std::cout << "out of bound" << x << " " << y << "\n";
            return material->kd;
        }

        ratio_u = texture_width / 400;
        ratio_v = texture_height / 400;
    }
    //std::cout << x<<" "<<y<<"  "<<z<<"\n";
    //return material->kd;

    if (normal(0) != 0) {
        u = z * ratio_u;
        v = y * ratio_v;
    }
    else if (normal(1) != 0) {
        u = x * ratio_u;
        v = z * ratio_v;
    }
    else if (normal(2) != 0) {
        u = x * ratio_u;
        v = y * ratio_v;
    }
    i = floor(u);
    j = floor(v);
    //i = texture_width - i-1;
    //j = texture_height - j-1;
    r = texture_data[j*texture_width * 4 + i * 4] / 255.0;
    g = texture_data[j*texture_width * 4 + i * 4 + 1] / 255.0;
    b = texture_data[j*texture_width * 4 + i * 4 + 2] / 255.0;

    return Eigen::Vector3d(r, g, b);

}