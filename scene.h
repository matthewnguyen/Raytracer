#ifndef _SCENE_H
#define _SCENE_H
#include <algorithm>
#include <iostream>
#include <vector>
#include "camera.h"
#include "triangle.h"
#include "sphere.h"
#include "primitive.h"
#include "light.h"

struct scene {
  int resx,resy;
  int max_depth; //default 5
  std::string output_fname;
  camera cam;
  std::vector<primitive> prims;
  std::vector<light*> lights;
scene(std::vector<primitive> prims, camera cam,  std::vector<light*> lights) : prims(prims), cam(cam), lights(lights) {}

  Vector3d trace_ray(ray r) {   
    Vector3d RGB(0,0,0); // initialized to black
    std::vector<intersection> inters;
    if (r.depth>4) {
      return RGB;
    }
    for (std::vector<primitive>::iterator iter = prims.begin(); iter != prims.end(); ++iter) {
      shape *sh = (*iter).s;
      if (sh->intersects(r)) {
	intersection inter = iter->intersect(r);
	inters.push_back(inter);
      }
    }
    if (!inters.empty()) {
      intersection inter = *std::min_element(inters.begin(),inters.end());
      for (std::vector<light*>::iterator iter = lights.begin(); iter != lights.end(); ++iter) {
	RGB += (*iter)->BRDF(inter,prims);
	//std::cout<<(*iter)->BRDF(inter,prims);
      }
      if (inter.m.kr.norm()) {
	ray ref = ray(inter.pos,reflected(-inter.incident,inter.normal));
	ref.depth = r.depth+1;
	Vector3d reflected = trace_ray(ref);
	Vector3d reflection = mult_by_elem(inter.m.kr,reflected);
	RGB+= reflection;
      }
    }
    return RGB;
  }
};
#endif
