#ifndef _TRIANGLE_H
#define _TRIANGLE_H
#include <Eigen/Dense>
#include <limits>
#include "camera.h"
#include "primitive.h"
#include <iostream>

struct triangle : public shape {
  Vector3d v1,v2,v3,normal; //normal vector may not be the one you want to work with. if not, use -normal.
 triangle(Vector3d v1,Vector3d v2,Vector3d v3) : v1(v1),v2(v2),v3(v3),normal(((v2-v1).cross(v3-v1)).normalized()) {}
  
  bool intersects(ray r) const{
    //    std::cout<<r.pos.format(fmt)<<std::endl;
    Matrix3d m;
    m << -r.dir, v2-v1,v3-v1;
    Matrix3d inverse;
    bool invertible;
    Vector3d inter_pos;
    m.computeInverseWithCheck(inverse,invertible);
    if (invertible) {

      inter_pos = inverse*(r.pos-v1);
      //      std::cout<<inter_pos[0]<<std::endl;
      if (inter_pos[0]>=1.0e-10 && inter_pos[1] >= 0 && inter_pos[2] >= 0 && inter_pos[1]+inter_pos[2] <= 1) {

	return true;
      }
    }
    return false;   
  }

  intersection intersect(ray r) const{
    assert(intersects(r));
    //std::cout<<normal.format(fmt)<<std::endl;
    Matrix3d m;
    m << -r.dir, v2-v1,v3-v1;
    Matrix3d inverse;
    bool invertible;
    Vector3d inter_pos;
    m.computeInverseWithCheck(inverse,invertible);
    inter_pos = inverse*(r.pos-v1); // [t,u,v]
    Vector3d new_n;
    if (r.dir.dot(normal)>0) {
      new_n = -normal;
    } else {
      new_n = normal;
    }
    return intersection(r(inter_pos[0]),new_n,r.dir,inter_pos[0]);
  }
};
#endif
