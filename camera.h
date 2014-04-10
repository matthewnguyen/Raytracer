#ifndef _CAMERA_H
#define _CAMERA_H
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <limits>
#define _USE_MATH_DEFINES

using namespace Eigen;

const double pi = M_PI;

IOFormat fmt(StreamPrecision, 0, ", ", "; ", "", "", "[", "]");

struct ray {
  Vector3d pos, dir;
  double max_dist;
  int depth;
ray(Vector3d pos, Vector3d dir): pos(pos),dir(dir), depth(0) {
  max_dist=std::numeric_limits<double>::max();
  }

  Vector3d operator()(double t) const {
    return pos + dir*t;
  }
};

// ASSUMPTION: t.linear().inverse() == t.inverse().linear()
ray operator*(const ray r, Transform<double, 3, Affine> t) {
  return ray(t*r.pos,t.linear()*r.dir);
}

std::ostream &operator<<(std::ostream &os, ray r) {
  os << "ray("
     << "pos=" << r.pos.format(fmt) << ","
     << "dir=" << r.dir.format(fmt) << ")";
  return os;
}

ray through_normalized(Vector3d from, Vector3d to) {
  return ray(from,(to-from).normalized());
}

struct camera {
  double near, far;
  int w, h;
  double fov;
  Vector3d pos, look_at, up;
  camera() {}
  camera(double near, double far, int w, int h, double fov_, Vector3d pos_, Vector3d look_at_, Vector3d up_) : near(near), far(far), w(w), h(h), fov(fov_*pi/180), pos(pos_), look_at(look_at_.normalized()), up(up_.normalized()) { 
    Vector3d cam_left = -look_at_.cross(up_);
    double f_w = 2*near*tan(fov/2);
    double f_h = static_cast<double>(h)*f_w/static_cast<double>(w);
    lower_left = pos + look_at_*near + cam_left*f_w/2 - up_*f_h/2;
    xv = -1*cam_left*f_w/static_cast<double>(w);
    yv = 1*up_*f_h/static_cast<double>(h);
  }

  Vector3d lower_left, xv, yv;

  ray get_ray(int x, int y) const {
    return through_normalized(pos,lower_left+(x+.5)*xv+(y+.5)*yv); 
  }
};
#endif
