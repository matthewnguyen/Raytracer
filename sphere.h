#ifndef _SPHERE_H
#define _SPHERE_H
#include <Eigen/Dense>
#include <limits>
#include "camera.h"
#include "primitive.h"

struct sphere : public shape {
  double rad;
  Vector3d pos;
  sphere(Vector3d pos, double rad): rad(rad),pos(pos) {}

  bool intersects(ray r) const {
    //    std::cout<<"calling"<<std::endl;
    //    std::cout<<r.pos.format(fmt)<<std::endl;
    //    std::cout<<r.dir.format(fmt)<<std::endl;
    //    std::cout<<pos.format(fmt)<<std::endl;
    double s_proj_r = r.dir.normalized().dot(pos-r.pos);
    double t = s_proj_r/r.dir.norm(); 
    //    std::cout<<t<<std::endl;
    Vector3d vec_d = r(t)-pos;
    double d = sqrt(vec_d.dot(vec_d));
    //    std::cout<<d<<std::endl;
    if (d <= rad && t>=std::numeric_limits<double>::min() && t<r.max_dist) {
      //std::cout<<"intersects"<<std::endl;
      return true;
    } else{
      return false;
    }
  }
  intersection intersect(ray r) const {
    assert(intersects(r));
    double t = r.dir.normalized().dot(pos-r.pos)/r.dir.norm();
    Vector3d vec_d = r(t)-pos;
    double d = sqrt(vec_d.dot(vec_d));
    double t_i = t-sqrt(pow(rad,2)-pow(d,2));
    Vector3d p = r(t_i);
    Vector3d n = p-pos;
    //std::cout<<rad-d<<std::endl;
    return intersection(p,n,r.dir,t_i);
  }
};
#endif
