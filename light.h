#ifndef _LIGHT_H
#define _LIGHT_H
#include <vector>
#include <Eigen/Dense>
#include <Magick++.h>
#include "camera.h"
#include "primitive.h"

using namespace Eigen;
using namespace Magick;

Vector3d mult_by_elem(Vector3d u, Vector3d v) {
  return Vector3d(u[0]*v[0],u[1]*v[1],u[2]*v[2]);
}

Vector3d reflected(Vector3d l, Vector3d n) {
  double k = 2*l.dot(n);
  return -l+n*k;
}

struct light {
  Vector3d intensity;
  virtual Vector3d diffuse(intersection i) const = 0;
  virtual Vector3d specular(intersection i) const = 0;
  virtual Vector3d BRDF(intersection i,std::vector<primitive> p) const = 0;

  bool shadowed(ray r, intersection i, std::vector<primitive> p) const {
    for (std::vector<primitive>::iterator iter = p.begin(); iter != p.end(); ++iter) {
      shape *s = (*iter).s;
      if (s->intersects(r)) {
	return true;	
      }
    }
    return false;
  }

  Vector3d ambient(intersection i) const {
    return mult_by_elem((i.m).ka,intensity);
  }
};

struct p_light : public light {
  Vector3d pos;
 p_light(Vector3d pos, Vector3d i): pos(pos) {intensity=i;}

  ray ray_to_light(intersection i) const {
    ray r = through_normalized(i.pos, pos);
    r.max_dist = (i.pos-pos).norm();
    return r;
  }
  
  Vector3d diffuse(intersection i) const {
    Vector3d l = (pos - i.pos).normalized();
    return mult_by_elem((i.m).kd,intensity)*std::max(l.dot(i.normal),static_cast<double>(0));
  }
  
  Vector3d specular(intersection i) const {
    Vector3d l = (pos - i.pos).normalized();
    Vector3d r = reflected(l,i.normal).normalized();
    Vector3d v = -i.pos.normalized();
    Vector3d rgb = mult_by_elem((i.m).ks,intensity)*pow(std::max(r.dot(v),static_cast<double>(0)),(i.m).sp);

    return rgb;
  }

  //Calculates RGB value for an intersection given the primitives in the scene
  Vector3d BRDF(intersection i,std::vector<primitive> p) const {
    ray lray = ray_to_light(i);
    if (!shadowed(lray,i,p)) {
      return MaxRGB*(diffuse(i)+specular(i)+ambient(i));
    } else {
      return MaxRGB*ambient(i);
    }
  }
};

struct d_light : public light{
  Vector3d dir;
 d_light(Vector3d dir, Vector3d i) : dir(dir.normalized()) {intensity=i;}
  
  Vector3d diffuse(intersection i) const {
    //  std::cout<<i.normal<<std::endl;
    return mult_by_elem((i.m).kd,intensity)*std::max(-dir.dot(i.normal),static_cast<double>(0));
  }
  
  Vector3d specular(intersection i) const {
    Vector3d r = reflected(-dir,i.normal).normalized();
    Vector3d v = -i.pos.normalized();
    //    std::cout<<i.m.ks.format(fmt)<<std::endl;
    //    std::cout<<intensity.format(fmt)<<std::endl;
    //    std::cout<<r.dot(v)<<std::endl;
    Vector3d rgb = mult_by_elem((i.m).ks,intensity)*pow(std::max(r.dot(v),static_cast<double>(0)),(i.m).sp);
    return rgb;
  }

  //Calculates RGB value for an intersection given the primitives in the scene
  Vector3d BRDF(intersection i,std::vector<primitive> p) const {
    ray lray = ray(i.pos,-dir);
    if (!shadowed(lray,i,p)) {
      //std::cout<<ambient(i).format(fmt)<<std::endl;
      return MaxRGB*(diffuse(i)+specular(i)+ambient(i));
    } else {
      return MaxRGB*ambient(i);
    }
  }  
};

#endif
