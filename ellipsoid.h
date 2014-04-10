#ifndef _ELLIPSOID_H
#define _ELLIPSOID_H
#include <math.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <limits>
#include "camera.h"
#include "primitive.h"
#include "sphere.h"

using namespace Eigen;

struct ellipsoid : public shape {
  Transform<double, 3, Affine> t; //obj2world transformation
 ellipsoid(Transform<double, 3, Affine> t): t(t) {}

  bool intersects(ray r) const {
    ray r_t = r*t.inverse();    
    sphere sp(Vector3d(0,0,0),1);
    return sp.intersects(r_t);
  }

  intersection intersect(ray r) const {
    assert(intersects(r));
    ray r_t = r*t.inverse();
    sphere sp(Vector3d(0,0,0),1);
    intersection i = sp.intersect(r_t);
    Vector3d pos = t*i.pos;
    Vector3d n = (t.linear().inverse().transpose()*i.normal).normalized();
    return intersection(pos,n,r.dir.normalized(),i.dist);
  }
};
#endif
