#ifndef _LOADSCENE_H
#define _LOADSCENE_H
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "scene.h"
#include "camera.h"
#include "primitive.h"
#include "light.h"


scene loadScene(std::string file) {

  //store variables and set stuff at the end
  int width, height, max_depth;
  std::string fname = "output.png";
  camera cam;
  std::vector<primitive> prims;
  std::vector<light*> lights;
  std::vector<shape*> shapes;
  std::vector<Vector3d> vertices;
  
  material m;
  Vector3d zero_vec(0,0,0);
  m.ka= zero_vec;
  m.ks= zero_vec;
  m.kr= zero_vec;
  m.kd= zero_vec;
  m.sp= 1;
  Transform<double,3,Affine> t;

  std::ifstream inpfile(file.c_str());
  if(!inpfile.is_open()) {
    std::cout << "Unable to open file" << std::endl;
  } else {
    std::string line;


    while(inpfile.good()) {
      std::vector<std::string> splitline;
      std::string buf;

      std::getline(inpfile,line);
      std::stringstream ss(line);

      while (ss >> buf) {
        splitline.push_back(buf);
      }
      //Ignore blank lines
      if(splitline.size() == 0) {
        continue;
      }

      //Ignore comments
      if(splitline[0][0] == '#') {
        continue;
      }

      //Valid commands:
      //size width height
      //  must be first command of file, controls image size
      else if(!splitline[0].compare("size")) {
        width = atoi(splitline[1].c_str());
        height = atoi(splitline[2].c_str());
      }
      //maxdepth depth
      //  max # of bounces for ray (default 5)
      else if(!splitline[0].compare("maxdepth")) {
        max_depth = atoi(splitline[1].c_str());
      }
      //output filename
      //  output file to write image to 
      else if(!splitline[0].compare("output")) {
        fname = splitline[1];
      }

      //camera lookfromx lookfromy lookfromz lookatx lookaty lookatz upx upy upz fov
      //  speciﬁes the camera in the standard way, as in homework 2.
      else if(!splitline[0].compare("camera")) {
        Vector3d lookfrom(atof(splitline[1].c_str()),atof(splitline[2].c_str()),atof(splitline[3].c_str()));
	Vector3d lookat(atof(splitline[4].c_str()),atof(splitline[5].c_str()),atof(splitline[6].c_str()));
        Vector3d up(atof(splitline[7].c_str()),atof(splitline[8].c_str()),atof(splitline[9].c_str()));
	double fov = atof(splitline[10].c_str());
	cam = camera(.1,1000,width,height,fov,lookfrom,lookat,up);
      }

      //sphere x y z radius
      //  Deﬁnes a sphere with a given position and radius.
      else if(!splitline[0].compare("sphere")) {
        double x = atof(splitline[1].c_str());
	double y = atof(splitline[2].c_str());
	double z = atof(splitline[3].c_str());
	double r = atof(splitline[4].c_str());
        // Create new sphere:
	shape *sph(new sphere(Vector3d(x,y,z),r));	
	prims.push_back(primitive(m,sph));
        //   Store 4 numbers
        //   Store current property values
        //   Store current top of matrix stack
      }
      //maxverts number
      //  Deﬁnes a maximum number of vertices for later triangle speciﬁcations. 
      //  It must be set before vertices are deﬁned.
      else if(!splitline[0].compare("maxverts")) {
        // Care if you want
        // Here, either declare array size
        // Or you can just use a STL vector, in which case you can ignore this
      }
      //maxvertnorms number
      //  Deﬁnes a maximum number of vertices with normals for later speciﬁcations.
      //  It must be set before vertices with normals are deﬁned.
      else if(!splitline[0].compare("maxvertnorms")) {
        // Care if you want
      }
      //vertex x y z
      //  Deﬁnes a vertex at the given location.
      //  The vertex is put into a pile, starting to be numbered at 0.
      else if(!splitline[0].compare("vertex")) {
        double x = atof(splitline[1].c_str());
        double y = atof(splitline[2].c_str());
        double z = atof(splitline[3].c_str());
	vertices.push_back(Vector3d(x,y,z));
        // Create a new vertex with these 3 values, store in some array
      }
      //vertexnormal x y z nx ny nz
      //  Similar to the above, but deﬁne a surface normal with each vertex.
      //  The vertex and vertexnormal set of vertices are completely independent
      //  (as are maxverts and maxvertnorms).
      else if(!splitline[0].compare("vertexnormal")) {
        // x: atof(splitline[1].c_str()),
        // y: atof(splitline[2].c_str()),
        // z: atof(splitline[3].c_str()));
        // nx: atof(splitline[4].c_str()),
        // ny: atof(splitline[5].c_str()),
        // nz: atof(splitline[6].c_str()));
        // Create a new vertex+normal with these 6 values, store in some array
      }
      //tri v1 v2 v3
      //  Create a triangle out of the vertices involved (which have previously been speciﬁed with
      //  the vertex command). The vertices are assumed to be speciﬁed in counter-clockwise order. Your code
      //  should internally compute a face normal for this triangle.
      else if(!splitline[0].compare("tri")) {
        int v1= atof(splitline[1].c_str());
        int v2= atof(splitline[2].c_str());
        int v3= atof(splitline[3].c_str());
        // Create new triangle:
        //   Store pointer to array of vertices
        //   Store 3 integers to index into array
        //   Store current property values
        //   Store current top of matrix stac
	shape *tri(new triangle(vertices[v1],vertices[v2],vertices[v3]));	
	prims.push_back(primitive(m,tri));
      }
      //trinormal v1 v2 v3
      //  Same as above but for vertices speciﬁed with normals.
      //  In this case, each vertex has an associated normal, 
      //  and when doing shading, you should interpolate the normals 
      //  for intermediate points on the triangle.
      else if(!splitline[0].compare("trinormal")) {
        // v1: atof(splitline[1].c_str())
        // v2: atof(splitline[2].c_str())
        // v3: atof(splitline[3].c_str())
        // Create new triangle:
        //   Store pointer to array of vertices (Different array than above)
        //   Store 3 integers to index into array
        //   Store current property values
        //   Store current top of matrix stack
      }

      //translate x y z
      //  A translation 3-vector
      else if(!splitline[0].compare("translate")) {
        double x = atof(splitline[1].c_str());
        double y = atof(splitline[2].c_str());
        double z = atof(splitline[3].c_str());
	Vector3d trans(x,y,z);
	Translation3d tr(trans);
	t = tr*t;
	
        // Update top of matrix stack
      }
      //rotate x y z angle
      //  Rotate by angle (in degrees) about the given axis as in OpenGL.
      else if(!splitline[0].compare("rotate")) {
        double x = atof(splitline[1].c_str());
        double y = atof(splitline[2].c_str());
        double z = atof(splitline[3].c_str());
        double angle = atof(splitline[4].c_str());
	Vector3d axis = Vector3d(x,y,z).normalized();
	AngleAxisd r(angle,axis);
        t = r*t;
        // Update top of matrix stack
      }
      //scale x y z
      //  Scale by the corresponding amount in each axis (a non-uniform scaling).
      else if(!splitline[0].compare("scale")) {
        double x = atof(splitline[1].c_str());
        double y= atof(splitline[2].c_str());
        double z= atof(splitline[3].c_str());
	DiagonalMatrix<double,3,3> s(x,y,z);
	//AngleAxisd(1.0,Vector3d(0,0,1))*
	t = s;
        // Update top of matrix stack
      }
      //ellipsoid
      else if(!splitline[0].compare("ellipsoid")){
	shape *ellip(new ellipsoid(t));
	primitive p_e(m,ellip);
	prims.push_back(p_e);       
	t = Transform<double,3,Affine>(Scaling(static_cast<double>(1)));
      }
      //pushTransform
      //  Push the current modeling transform on the stack as in OpenGL. 
      //  You might want to do pushTransform immediately after setting 
      //   the camera to preserve the “identity” transformation.
      else if(!splitline[0].compare("pushTransform")) {
	
      }
      //popTransform
      //  Pop the current transform from the stack as in OpenGL. 
      //  The sequence of popTransform and pushTransform can be used if 
      //  desired before every primitive to reset the transformation 
      //  (assuming the initial camera transformation is on the stack as 
      //  discussed above).
      else if(!splitline[0].compare("popTransform")) {
        //mst.pop();
      }

      //directional x y z r g b
      //  The direction to the light source, and the color, as in OpenGL.
      else if(!splitline[0].compare("directional")) {
        double x = atof(splitline[1].c_str());
        double y = atof(splitline[2].c_str());
        double z = atof(splitline[3].c_str());
        double r = atof(splitline[4].c_str());
        double g = atof(splitline[5].c_str());
        double b = atof(splitline[6].c_str());
	light *dl(new d_light(Vector3d(x,y,z),Vector3d(r,g,b)));
	lights.push_back(dl);
        // add light to scene...
      }
      //point x y z r g b
      //  The location of a point source and the color, as in OpenGL.
      else if(!splitline[0].compare("point")) {
        double x = atof(splitline[1].c_str());
        double y = atof(splitline[2].c_str());
        double z = atof(splitline[3].c_str());
        double r = atof(splitline[4].c_str());
        double g = atof(splitline[5].c_str());
        double b = atof(splitline[6].c_str());
	light *pl(new p_light(Vector3d(x,y,z),Vector3d(r,g,b)));
	lights.push_back(pl);
        // Add light to scene...
      }
      //attenuation const linear quadratic
      //  Sets the constant, linear and quadratic attenuations 
      //  (default 1,0,0) as in OpenGL.
      else if(!splitline[0].compare("attenuation")) {
        // const: atof(splitline[1].c_str())
        // linear: atof(splitline[2].c_str())
        // quadratic: atof(splitline[3].c_str())
      }
      //ambient r g b
      //  The global ambient color to be added for each object 
      //  (default is .2,.2,.2)
      else if(!splitline[0].compare("ambient")) {
        double r= atof(splitline[1].c_str());
        double g= atof(splitline[2].c_str());
        double b= atof(splitline[3].c_str());
	m.ka = Vector3d(r,g,b);
      }

      //diﬀuse r g b
      //  speciﬁes the diﬀuse color of the surface.
      else if(!splitline[0].compare("diffuse")) {
        // r: atof(splitline[1].c_str())
        // g: atof(splitline[2].c_str())
        // b: atof(splitline[3].c_str())
        double r= atof(splitline[1].c_str());
        double g= atof(splitline[2].c_str());
        double b= atof(splitline[3].c_str());
	m.kd = Vector3d(r,g,b);
        // Update current properties
      }
      //specular r g b 
      //  speciﬁes the specular color of the surface.
      else if(!splitline[0].compare("specular")) {
        // r: atof(splitline[1].c_str())
        // g: atof(splitline[2].c_str())
        // b: atof(splitline[3].c_str())
        double r= atof(splitline[1].c_str());
        double g= atof(splitline[2].c_str());
        double b= atof(splitline[3].c_str());
	m.ks = Vector3d(r,g,b);
        // Update current properties
      }
      //shininess s
      //  speciﬁes the shininess of the surface.
      else if(!splitline[0].compare("shininess")) {
        // shininess: 
	m.sp = atof(splitline[1].c_str());
        // Update current properties
      }
      //emission r g b
      //  gives the emissive color of the surface.
      else if(!splitline[0].compare("emission")) {
        // r: atof(splitline[1].c_str())
        // g: atof(splitline[2].c_str())
        // b: atof(splitline[3].c_str())
        // Update current properties
        double r= atof(splitline[1].c_str());
        double g= atof(splitline[2].c_str());
        double b= atof(splitline[3].c_str());
	m.kr = Vector3d(r,g,b);
      } else {
        std::cerr << "Unknown command: " << splitline[0] << std::endl;
      }
    }

    inpfile.close();
  }
  return scene(prims,cam,lights);
}

#endif
