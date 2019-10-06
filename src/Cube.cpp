#include "Cube.h"
#include "Triangle.h"
#include "Ray.h"
#include <Eigen/Geometry>
#include <iostream>

bool Cube::intersect(
        const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{

  Eigen::Vector3d e = ray.origin;
  Eigen::Vector3d d = ray.direction;

/*
  Eigen::Vector3d v1 = this->v1;
  Eigen::Vector3d v2 = this->v2;
  Eigen::Vector3d v3 = this->v3;
  Eigen::Vector3d v4 = this->v4;

  Eigen::Vector3d v5(v1(0), -23, v1(2));
  Eigen::Vector3d v6(v2(0), -23, v2(2));
  Eigen::Vector3d v7(v3(0), -23, v3(2));
  Eigen::Vector3d v8(v4(0), -23, v4(2));
  */


  Eigen::Vector3d v1 = this->v1;
  Eigen::Vector3d v8 = this->v8;

  Eigen::Vector3d v2(v8(0), v1(1), v1(2));
  Eigen::Vector3d v3(v1(0), v1(1), v8(2));
  Eigen::Vector3d v4(v8(0), v1(1), v8(2));
  Eigen::Vector3d v5(v1(0), v8(1), v1(2));

  Eigen::Vector3d v6(v8(0), v8(1), v1(2));
  Eigen::Vector3d v7(v1(0), v8(1), v8(2));



  t = std::numeric_limits<double>::infinity();
  double another_t;
  Eigen::Vector3d another_n;

  Triangle t1;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c1(v1,v2,v3);
  t1.corners = c1;

  Triangle t2;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c2(v2,v3,v4);
  t2.corners = c2;

  Triangle t3;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c3(v2,v4,v8);
  t3.corners = c3;

  Triangle t4;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c4(v3,v4,v8);
  t4.corners = c4;

  Triangle t5;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c5(v6,v2,v8);
  t5.corners = c5;

  Triangle t6;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c6(v7,v8,v3);
  t6.corners = c6;

  Triangle t7;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c7(v1,v2,v6);
  t7.corners = c7;

  Triangle t8;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c8(v1,v5,v6);
  t8.corners = c8;

  Triangle t9;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c9(v1,v3,v7);
  t9.corners = c9;

  Triangle t10;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c10(v1,v5,v7);
  t10.corners = c10;

  Triangle t11;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c11(v5,v6,v7);
  t11.corners = c11;

  Triangle t12;
  std::tuple< Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> c12(v6,v7,v8);
  t12.corners = c12;

  bool intersect = false;

  if (t1.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
    intersect = true;
  }

  if (t2.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
    intersect = true;
  }

  if (t3.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
      t = another_t;
      n = another_n;
    }
    intersect = true;
  }

  if (t4.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
      t = another_t;
      n = another_n;
    }
    intersect = true;
  }

  if (t5.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
      t = another_t;
      n = another_n;
    }
    intersect = true;
  }

  if (t6.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
      intersect = true;
  }

  if (t7.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
      intersect = true;
  }
  if (t8.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
      intersect = true;
  }

  if (t9.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
      intersect = true;
  }
  if (t10.intersect(ray, min_t, another_t, another_n)) {
    if (another_t < t) {
        t = another_t;
        n = another_n;
    }
      intersect = true;
  }


  //printf("intersect: %d\n", intersect);
  if (intersect) {
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////


Eigen::Vector3d Cube::set_texture_color(
	Eigen::Vector3d p) const
{
	double x, y, z, h, l, w, r, g, b;
	double u, v;
	int i, j;

	if (texture_data.size() == 0) {
	    return material->kd;
    }


	p = p - v8;          // move lb to (0, 0, 0)
	l = v1(2) - v8(2);   // y coordinate
	h = v1(1) - v8(1);   // z coordinate
	w = v1(0) - v8(0);   // x coordinate

	x = p(0);
    z = p(1);
	y = p(2);


	//std::cout << "h,w,l,x,y,z:  " << h << " " << w << " " << l << " " << x << " " << y << " " << z << "\n";
	double ratio_u = texture_width / (2 * w + 2 * l);
	double ratio_v = texture_height / (2 * w + h);

    //std::cout << "width, height:  " << texture_width << "  " << texture_height << "\n";

	if (x >= 0 && x <= w  && z >= 0 && z <= h && y <0.000001) {
		u = x * ratio_u;
		v = (w + z) * ratio_v;
	}
	else if (abs(x-w)< 0.000001 && y >= 0 && y <= l && z >= 0 && z <= h) {
		u = (w + y) * ratio_u;
		v = (w + z) * ratio_v;
	}
	else if (x >= 0 && x <= w && y >= 0 && y <= l && abs(z - h) < 0.0001) {
		//std::cout << "...............hit\n";
		u = (w + y) * ratio_u;
		v = (2* w + h - x) * ratio_v;
	}

	else if (x < 0.000001 && y >= 0 && y <= l && z >= 0 && z <= h) {
		u = (2 * w + 2 * l - y) * ratio_u;
		v = (w + z) * ratio_v;
	}

	else if (x >= 0 && x <= w && y >= 0 && y <= l && z < 0.0001) {
		u = (w + y) * ratio_u;
		v = x * ratio_v;
	}
	else if (x >= 0 && x <= w  && z >= 0 && z <= h && abs(y- l)<0.0001) {
		u = (2* w + l -x) * ratio_u;
		v = (w + z) * ratio_v;
	}


	i = floor(u);
	j = floor(v);
	r = texture_data[j*texture_width * 4 + i* 4] / 255.0;
	g = texture_data[j*texture_width * 4 + i * 4 + 1] / 255.0;
	b = texture_data[j*texture_width * 4 + i * 4 + 2] / 255.0;
	//std::cout << "i, j: " << i << " " << j << "\n";
	//std::cout << "r, g, b:" << r << " " << g << " " << b << "\n";
	return Eigen::Vector3d(r, g, b);
}


