#ifndef _OBJ_PARSER_H
#define _OBJ_PARSER_H
#include <Eigen/Dense>
#include <vector>
#include "triangle.h"
#include "primitive.h"
#include <ios>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

double read_double(istream &is) {
  string d;
  char ch;
  ch = is.get();
  while (isdigit(ch) || isalpha(ch) || ch == '.' || ch == '+' || ch == '-') {
    d.push_back(ch);
    ch = is.get();
  }
  is.unget();
  double n = atof(d.c_str());
  return n;
}

void eat_whitespace(ifstream &in) {
  char c = in.get();
  while (isspace(c))
    c = in.get();
  in.unget();
}
 
void parse_vertex(ifstream &in, vector<Vector3d> &vertices) {
  eat_whitespace(in);
  double vx,vy,vz;
  char ch;
  vx = read_double(in);
  eat_whitespace(in);
  vy = read_double(in);
  eat_whitespace(in);
  vz = read_double(in);
  Vector3d v(vx,vy,vz);
  vertices.push_back(v);
}

void parse_face(ifstream &in, vector<Vector3d> &vertices, vector<shape*> &shapes) {
  eat_whitespace(in);
  int v1,v2,v3;
  char ch;
  v1 = read_double(in);
  while (!isspace(ch)) {
    in.get(ch);
  }
  eat_whitespace(in);
  v2 = read_double(in);
  while (!isspace(ch)) {
    in.get(ch);
  }
  eat_whitespace(in);
  v3 = read_double(in);
  while (!isspace(ch)) {
    in.get(ch);
  }
  shape *tri(new triangle(vertices[v1-1],vertices[v2-1],vertices[v3-1]));
  shapes.push_back(tri);
}



vector<shape*> parser(string filename) {
  vector<Vector3d> vertices;
  vector<shape*> shapes; 
  ifstream in(filename.c_str());
  char ch;					
  while (in.peek() > 0) {
    eat_whitespace(in);
    ch = in.get();
    string s;
    while (!isspace(ch) && in.peek() > 0) {
      s.push_back(ch);
      ch = in.get();
    }
    if (s == "v") {
      parse_vertex(in,vertices);
    }
    //if (s == "vn") {}
    //    if (s == "#") {      
    else if (s == "f") {
	parse_face(in,vertices,shapes);
    } 
    else  {
      while (ch != '\n' && in.peek() > 0)
	ch = in.get();
    }
  }
  return shapes;
}
#endif
