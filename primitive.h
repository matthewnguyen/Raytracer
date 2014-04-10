#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
#include <limits>
#include "camera.h"

using namespace Eigen;

struct material {
  Vector3d ka,ks,kd, kr;
  double sp;
};

struct intersection {
  Vector3d pos, normal, incident;
  double dist;
  material m;
  intersection(Vector3d pos, Vector3d normal, Vector3d incident, double d): pos(pos), normal(normal.normalized()),incident(incident),dist(d){}

  bool operator<(const intersection x) {
    return dist<x.dist;
  }
};

struct shape {
  virtual bool intersects(ray r) const = 0;
  virtual intersection intersect(ray r) const = 0;
};

struct shape_aggregate : public shape {
  std::vector<shape*> shapes;
  Transform<double,3,Affine> t; //obj2world transformation
 shape_aggregate(std::vector<shape*> shapes, Transform<double,3,Affine> t) : shapes(shapes), t(t) {}

  bool intersects(ray r) const {
    ray r_t = r*t.inverse();
    for (std::vector<shape*>::const_iterator iter = shapes.begin(); iter != shapes.end(); ++iter) {
      if ((*iter)->intersects(r_t)) {
	//	std::cout<<"intersects"<<std::endl;    
	//	std::cout<<"  r.pos"<<r.pos.format(fmt)<<" r.dir"<<r.dir.format(fmt)<<std::endl;
	//	std::cout<<"r_t.pos"<<r_t.pos.format(fmt)<<" r_t.dir"<<r_t.dir.format(fmt)<<std::endl;
	return true;
      }
    }
    return false;
  }

  intersection intersect(ray r) const{
    //    std::cout<<"calling intersect"<<std::endl;    
    //    std::cout<<"  r.pos"<<r.pos.format(fmt)<<" r.dir"<<r.dir.format(fmt)<<std::endl;
    assert(intersects(r));
    std::vector<intersection> inters;
    ray r_t = r*t.inverse();
    for (std::vector<shape*>::const_iterator iter = shapes.begin(); iter != shapes.end(); ++iter) {
      //std::cout<<"intersect innerloop"<<std::endl;
      if ((*iter)->intersects(r_t)) {
	inters.push_back((*iter)->intersect(r_t));
      }
    }
    //    std::cout<<"finished calling intersect"<<std::endl;
    intersection i = *std::min_element(inters.begin(),inters.end());
    Vector3d pos = t*i.pos; 
    Vector3d n = (t.linear().inverse().transpose()*i.normal).normalized();
    return intersection(pos,n,r.dir.normalized(),i.dist);
  }
};

struct primitive {
  material mat;
  shape *s;
  primitive() {}
primitive(material m, shape *s) : mat(m), s(s) {}
  intersection intersect(ray r) const {
    intersection i = (*s).intersect(r);
    i.m = mat;
    return i;
  }
};
#endif
 
