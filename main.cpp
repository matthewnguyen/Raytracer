#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Magick++.h>
#include <iostream>
#include <tr1/memory>
#include <cassert>
#include <utility>
#include <math.h>
#include <limits>
#include <vector>
#include "camera.h"
#include "primitive.h"
#include "sphere.h"
#include "triangle.h"
#include "light.h"
#include "scene.h"
#include "ellipsoid.h"
#include "obj_parser.h"
#include "loadscene.h"

using namespace Eigen;
using namespace Magick;


Color render_color(Vector3d rgb) {
  return Color(rgb[0],rgb[1],rgb[2],0/*transparency value*/);
}


int main(int argc, char **argv) {
  int resx,resy;
  std::string obj_fname = "no obj file specified";
  std::string sc_fname = "no scene specfied";
  material m;
  m.ka=Vector3d(.1,.1,.1);
  m.kd=Vector3d(.8,.8,.8);
  m.ks=Vector3d(.9,.9,.9);
  m.sp=50;
  m.kr=Vector3d(0,0,0);
  
  for (int i =0; i < argc; i++){
    if (!strcmp(argv[i],"-o")) {
      obj_fname = argv[i+1];
    }
    else if (!strcmp(argv[i],"-sc")) {
      sc_fname = argv[i+1];
    }
  }
  scene sc = loadScene(sc_fname);
  if (sc_fname == "no scene specfied") {
    std::cout<<sc_fname<<std::endl;
  } else {
    Transform<double,3,Affine> t(Scaling(Vector3d(1,1,1)));
    if (obj_fname != "no obj file specfied") {
      std::vector<shape*> shapes = parser(obj_fname);
      shape *shape_a(new shape_aggregate(shapes,t));
      primitive prim(m,shape_a);
      sc.prims.push_back(prim);
    }
    
    resx = sc.cam.w;
    resy = sc.cam.h;
    InitializeMagick(*argv);
    Image my_image(Geometry(resx,resy),Color(0,0,0,0));
    for (int i=0; i<resx; i++) {
      for (int j=0; j<resy; j++) {
	ray r = sc.cam.get_ray(i,resy-1-j);      
	my_image.pixelColor(i,j,render_color(sc.trace_ray(r)));
      }
    }
    my_image.write("z.png");
  }
  return 0;
}
