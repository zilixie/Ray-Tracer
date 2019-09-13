#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "read_json.h"
#include "write_ppm.h"
#include "viewing_ray.h"
#include "raycolor.h"
#include <Eigen/Core>
#include <vector>
#include <iostream> 
#include <memory>
#include <limits>
#include <functional>


int main(int argc, char * argv[])
{
  Camera camera;
  std::vector< std::shared_ptr<Object> > objects;
  std::vector< std::shared_ptr<Light> > lights;
  // Read a camera and scene description from given .json file
  read_json(
    argc<=1?"../shared/data/sphere-and-plane.json":argv[1],
    camera,
    objects,
    lights); //这里把json里面的的参数读出来

  int width =  1280;                //66
  int height = 720; //规定屏幕大小   //93
  std::vector<unsigned char> rgb_image(3*width*height);
  // For each pixel (i,j)
  for(unsigned i=0; i<height; ++i) 
  {
    for(unsigned j=0; j<width; ++j)
    {
      printf("%d, %d\n", i,j);
      // Set background color
      Eigen::Vector3d rgb(0,0,0);

      // Compute viewing ray
      Ray ray;
      viewing_ray(camera,i,j,width,height,ray); //根据每个pixel 修改 the ray的 origin和direction
      
      // Shoot ray and collect color
      raycolor(ray,1.0,objects,lights,0,rgb); //给一个ray 获得这个ray的颜色信息, 1.0*d 是camera到screen的距离, 所以min_t的值为1.0
                                              //objects,lights 都是std::vector< std::shared_ptr<Light> >
                                              //output: rgb color
      // Write double precision color into image
      auto clamp = [](double s){ return std::max(std::min(s,1.0),0.0);}; //一个 helper function 把值圈定在0.0 到 1.0
      rgb_image[0+3*(j+width*i)] = 255.0*clamp(rgb(0));
      rgb_image[1+3*(j+width*i)] = 255.0*clamp(rgb(1));
      rgb_image[2+3*(j+width*i)] = 255.0*clamp(rgb(2));

    }
  }

  write_ppm("rgb.ppm",rgb_image,width,height,3);
}
