#include <stdio.h>
#include <stdlib.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "math.h"
#include "gmath.h"

/*======== void add_polygon() ==========
  Inputs:   struct matrix *polygons
            double x0
            double y0
            double z0
            double x1
            double y1
            double z1
            double x2
            double y2
            double z2
  Returns:
  Adds the vertices (x0, y0, z0), (x1, y1, z1)
  and (x2, y2, z2) to the polygon matrix. They
  define a single triangle surface.
  ====================*/
void add_polygon( struct matrix *polygons, 
                  double x0, double y0, double z0, 
                  double x1, double y1, double z1, 
                  double x2, double y2, double z2 ) {
  add_point(polygons, x0, y0, z0);
  add_point(polygons, x1, y1, z1);
  add_point(polygons, x2, y2, z2);
}

/*======== void draw_polygons() ==========
  Inputs:   struct matrix *polygons
            screen s
            color c
  Returns:
  Goes through polygons 3 points at a time, drawing
  lines connecting each points to create bounding triangles
  ====================*/
void draw_polygons( struct matrix *polygons, screen s, color c ) {
    double ** matrix = polygons -> m;
    int lastcol = polygons -> cols;
    double vector[3];
    vector[0] = 0;
    vector[1] = 0;
    vector[2] = 1;

    for (int col = 0; col < lastcol; col += 3) {
        double * norm = calculate_normal(polygons, col);
        double dp = dot_product(norm, vector);
        if (dp > 0) {
          double x0 = matrix[0][col];
          double y0 = matrix[1][col];
          double x1 = matrix[0][col + 1];
          double y1 = matrix[1][col + 1];
          double x2 = matrix[0][col + 2];
          double y2 = matrix[1][col + 2];

        draw_line(x0, y0, x1, y1, s, c);
        draw_line(x1, y1, x2, y2, s, c);
        draw_line(x2, y2, x0, y0, s, c);
        }
    }
}

/*======== void add_box() ==========
  Inputs:   struct matrix * edges
            double x
            double y
            double z
            double width
            double height
            double depth

  add the points for a rectagular prism whose
  upper-left-front corner is (x, y, z) with width,
  height and depth dimensions.
  ====================*/
void add_box( struct matrix * polygons,
              double x, double y, double z,
              double width, double height, double depth ) {
  double x0, y0, z0, x1, y1, z1;
  x0 = x;
  x1 = x+width;
  y0 = y;
  y1 = y-height;
  z0 = z;
  z1 = z-depth;

  // Left Face
  add_polygon(polygons, x0, y0, z1, x0, y1, z1, x0, y0, z0);
  add_polygon(polygons, x0, y0, z0, x0, y1, z1, x0, y1, z0);
  // // Right Face
  add_polygon(polygons, x1, y0, z0, x1, y1, z0, x1, y0, z1);
  add_polygon(polygons, x1, y0, z1, x1, y1, z0, x1, y1, z1);

  // Front Face
  add_polygon(polygons, x0, y0, z0, x0, y1, z0, x1, y0, z0);
  add_polygon(polygons, x1, y0, z0, x0, y1, z0, x1, y1, z0);
  // Back Face
  add_polygon(polygons, x1, y0, z1, x1, y1, z1, x0, y0, z1);
  add_polygon(polygons, x0, y0, z1, x1, y1, z1, x0, y1, z1);

  // Top Face
  add_polygon(polygons, x0, y0, z1, x0, y0, z0, x1, y0, z1);
  add_polygon(polygons, x1, y0, z1, x0, y0, z0, x1, y0, z0);
  // Bottom Face
   add_polygon(polygons, x0, y1, z0, x0, y1, z1, x1, y1, z0);
   add_polygon(polygons, x1, y1, z0, x0, y1, z1, x1, y1, z1);

}


/*======== void add_sphere() ==========
  Inputs:   struct matrix * points
            double cx
            double cy
            double cz
            double r
            int step  

  adds all the points for a sphere with center (cx, cy, cz)
  and radius r using step points per circle/semicircle.

  Since edges are drawn using 2 points, add each point twice,
  or add each point and then another point 1 pixel away.

  should call generate_sphere to create the necessary points
  ====================*/
void add_sphere( struct matrix * polygons,
                 double cx, double cy, double cz,
                 double r, int steps ) {

  struct matrix *points = generate_sphere(cx, cy, cz, r, steps);
  double ** matrix = points -> m;
  int index, lat, longt;
  int latStop, longStop, latStart, longStart;
  latStart = 0;
  latStop = steps;
  longStart = 0;
  longStop = steps;

  steps++;
  for ( lat = latStart; lat < latStop; lat++ ) {
    for ( longt = longStart; longt < longStop; longt++ ) {
      index = lat * (steps) + longt;

      int p0 = index;
      int p1 = index + 1;
      int p2 = (index + steps) % points -> lastcol;
      int p3 = (index + steps + 1) % points -> lastcol;

      double x0 = matrix[0][p0];
      double y0 = matrix[1][p0];
      double z0 = matrix[2][p0];

      double x1 = matrix[0][p1];
      double y1 = matrix[1][p1];
      double z1 = matrix[2][p1];

      double x2 = matrix[0][p2];
      double y2 = matrix[1][p2];
      double z2 = matrix[2][p2];

      double x3 = matrix[0][p3];
      double y3 = matrix[1][p3];
      double z3 = matrix[2][p3];

      if (longt != 0) {
        add_polygon(polygons, x0, y0, z0, x3, y3, z3, x2, y2, z2);
      }

      if (longt != steps - 2) {
        add_polygon(polygons, x0, y0, z0, x1, y1, z1, x3, y3, z3);
      }

    }
  }
  free_matrix(points);
}

/*======== void generate_sphere() ==========
  Inputs:   struct matrix * points
            double cx
            double cy
            double cz
            double r
            int step
  Returns: Generates all the points along the surface
           of a sphere with center (cx, cy, cz) and
           radius r using step points per circle/semicircle.
           Returns a matrix of those points
  ====================*/
struct matrix * generate_sphere(double cx, double cy, double cz,
                                double r, int steps ) {

  struct matrix *points = new_matrix(4, steps * steps);
  int circle, rotation, rot_start, rot_stop, circ_start, circ_stop;
  double x, y, z, rot, circ;

  rot_start = 0;
  rot_stop = steps;
  circ_start = 0;
  circ_stop = steps;

  for (rotation = rot_start; rotation < rot_stop; rotation++) {
    rot = (double)rotation / steps;

    for(circle = circ_start; circle <= circ_stop; circle++){
      circ = (double)circle / steps;

      x = r * cos(M_PI * circ) + cx;
      y = r * sin(M_PI * circ) *
        cos(2*M_PI * rot) + cy;
      z = r * sin(M_PI * circ) *
        sin(2*M_PI * rot) + cz;

      /* printf("rotation: %d\tcircle: %d\n", rotation, circle); */
      /* printf("rot: %lf\tcirc: %lf\n", rot, circ); */
      /* printf("sphere point: (%0.2f, %0.2f, %0.2f)\n\n", x, y, z); */
      add_point(points, x, y, z);
    }
  }

  return points;
}

/*======== void add_torus() ==========
  Inputs:   struct matrix * points
            double cx
            double cy
            double cz
            double r1
            double r2
            double step
  Returns:

  adds all the points required for a torus with center (cx, cy, cz),
  circle radius r1 and torus radius r2 using step points per circle.

  should call generate_torus to create the necessary points
  ====================*/
void add_torus( struct matrix * polygons, 
                double cx, double cy, double cz,
                double r1, double r2, int steps ) {

  struct matrix *points = generate_torus(cx, cy, cz, r1, r2, steps);
  double ** matrix = points -> m;
  int index, lat, longt;
  int latStop, longStop, latStart, longStart;
  latStart = 0;
  latStop = steps;
  longStart = 0;
  longStop = steps;

  for ( lat = latStart; lat < latStop; lat++ ) {
    for ( longt = longStart; longt < longStop; longt++ ) {
      index = lat * steps + longt;

      int p0 = index;
      int p1 = (index + steps) % points -> lastcol;
      int p2 = (index + steps + 1) % points -> lastcol;
      int p3 = index + 1;
      int p4 = p3 % points -> lastcol;
      int p5 = (index - longt) % points -> lastcol;

      double x0 = matrix[0][p0];
      double y0 = matrix[1][p0];
      double z0 = matrix[2][p0];

      double x1 = matrix[0][p1];
      double y1 = matrix[1][p1];
      double z1 = matrix[2][p1];

      double x2 = matrix[0][p2];
      double y2 = matrix[1][p2];
      double z2 = matrix[2][p2];

      double x3 = matrix[0][p3];
      double y3 = matrix[1][p3];
      double z3 = matrix[2][p3];

      double x4 = matrix[0][p4];
      double y4 = matrix[1][p4];
      double z4 = matrix[2][p4];

      double x5 = matrix[0][p5];
      double y5 = matrix[1][p5];
      double z5 = matrix[2][p5];

      if (longt == steps - 1) {
        add_polygon(polygons, x0, y0, z0, x1, y1, z1, x4, y4, z4);
        add_polygon(polygons, x0, y0, z0, x4, y4, z4, x5, y5, z5);
      }
      else {
        add_polygon(polygons, x0, y0, z0, x1, y1, z1, x2, y2, z2);
        add_polygon(polygons, x0, y0, z0, x2, y2, z2, x3, y3, z3);
      }
    }
  }
  free_matrix(points);
}

/*======== void generate_torus() ==========
  Inputs:   struct matrix * points
            double cx
            double cy
            double cz
            double r
            int step
  Returns: Generates all the points along the surface
           of a torus with center (cx, cy, cz),
           circle radius r1 and torus radius r2 using
           step points per circle.
           Returns a matrix of those points
  ====================*/
struct matrix * generate_torus( double cx, double cy, double cz,
                                double r1, double r2, int steps ) {

  struct matrix *points = new_matrix(4, steps * steps);
  int circle, rotation, rot_start, rot_stop, circ_start, circ_stop;
  double x, y, z, rot, circ;

  rot_start = 0;
  rot_stop = steps;
  circ_start = 0;
  circ_stop = steps;

  for (rotation = rot_start; rotation < rot_stop; rotation++) {
    rot = (double)rotation / steps;

    for(circle = circ_start; circle < circ_stop; circle++){
      circ = (double)circle / steps;

      x = cos(2*M_PI * rot) *
        (r1 * cos(2*M_PI * circ) + r2) + cx;
      y = r1 * sin(2*M_PI * circ) + cy;
      z = -1*sin(2*M_PI * rot) *
        (r1 * cos(2*M_PI * circ) + r2) + cz;

      //printf("rotation: %d\tcircle: %d\n", rotation, circle);
      //printf("torus point: (%0.2f, %0.2f, %0.2f)\n", x, y, z);
      add_point(points, x, y, z);
    }
  }
  return points;
}

/*======== void add_circle() ==========
  Inputs:   struct matrix * edges
            double cx
            double cy
            double r
            double step

  Adds the circle at (cx, cy) with radius r to edges
  ====================*/
void add_circle( struct matrix *edges,
                 double cx, double cy, double cz,
                 double r, int steps ) {
  double x0, y0, x1, y1, t;
  int i;

  x0 = r + cx;
  y0 = cy;
  for (i=1; i<=steps; i++) {
    t = (double)i/steps;
    x1 = r * cos(2 * M_PI * t) + cx;
    y1 = r * sin(2 * M_PI * t) + cy;

    add_edge(edges, x0, y0, cz, x1, y1, cz);
    x0 = x1;
    y0 = y1;
  }
}


/*======== void add_curve() ==========
Inputs:   struct matrix *edges
         double x0
         double y0
         double x1
         double y1
         double x2
         double y2
         double x3
         double y3
         double step
         int type

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix edges
====================*/
void add_curve( struct matrix *edges,
                double x0, double y0, 
                double x1, double y1, 
                double x2, double y2, 
                double x3, double y3, 
                int steps, int type ) {
  double t, x, y;
  int i;
  struct matrix *xcoefs;
  struct matrix *ycoefs;

  xcoefs = generate_curve_coefs(x0, x1, x2, x3, type);
  ycoefs = generate_curve_coefs(y0, y1, y2, y3, type);
  
  /* print_matrix(xcoefs); */
  /* printf("\n"); */
  /* print_matrix(ycoefs); */
  
  for (i=1; i<=steps; i++) {
    t = (double)i/steps;

    x = xcoefs->m[0][0] *t*t*t + xcoefs->m[1][0] *t*t+
      xcoefs->m[2][0] *t + xcoefs->m[3][0];
    y = ycoefs->m[0][0] *t*t*t + ycoefs->m[1][0] *t*t+
      ycoefs->m[2][0] *t + ycoefs->m[3][0];

    add_edge(edges, x0, y0, 0, x, y, 0);
    x0 = x;
    y0 = y;
  }

  free_matrix(xcoefs);
  free_matrix(ycoefs);
}



/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z 
Returns: 
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {

  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );
  
  points->m[0][ points->lastcol ] = x;
  points->m[1][ points->lastcol ] = y;
  points->m[2][ points->lastcol ] = z;
  points->m[3][ points->lastcol ] = 1;
  points->lastcol++;
} //end add_point

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns: 
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1) {
  add_point( points, x0, y0, z0 );
  add_point( points, x1, y1, z1 );
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c 
Returns: 
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, screen s, color c) {

 if ( points->lastcol < 2 ) {
   printf("Need at least 2 points to draw a line!\n");
   return;
 }
 
 int point;
 for (point=0; point < points->lastcol-1; point+=2)
   draw_line( points->m[0][point],
	      points->m[1][point],
	      points->m[0][point+1],
	      points->m[1][point+1],
	      s, c);	       
}// end draw_lines









void draw_line(int x0, int y0, int x1, int y1, screen s, color c) {
  
  int x, y, d, A, B;
  //swap points if going right -> left
  int xt, yt;
  if (x0 > x1) {
    xt = x0;
    yt = y0;
    x0 = x1;
    y0 = y1;
    x1 = xt;
    y1 = yt;
  }
  
  x = x0;
  y = y0;
  A = 2 * (y1 - y0);
  B = -2 * (x1 - x0);  

  //octants 1 and 8
  if ( abs(x1 - x0) >= abs(y1 - y0) ) {

    //octant 1    
    if ( A > 0 ) {
      
      d = A + B/2;      
      while ( x < x1 ) {
	plot( s, c, x, y );
	if ( d > 0 ) {
	  y+= 1;
	  d+= B;
	}
	x++;
	d+= A;
      } //end octant 1 while
      plot( s, c, x1, y1 );
    } //end octant 1

    //octant 8
    else {
      d = A - B/2;
      
      while ( x < x1 ) {
	//printf("(%d, %d)\n", x, y);
	plot( s, c, x, y );
	if ( d < 0 ) {
	  y-= 1;
	  d-= B;
	}
	x++;
	d+= A;
      } //end octant 8 while
      plot( s, c, x1, y1 );
    } //end octant 8
  }//end octants 1 and 8

  //octants 2 and 7
  else {
    
    //octant 2    
    if ( A > 0 ) {
      d = A/2 + B;      

      while ( y < y1 ) {
	plot( s, c, x, y );
	if ( d < 0 ) {
	  x+= 1;
	  d+= A;
	}
	y++;
	d+= B;
      } //end octant 2 while
      plot( s, c, x1, y1 );
    } //end octant 2

    //octant 7
    else {
      d = A/2 - B;
      
      while ( y > y1 ) {
	plot( s, c, x, y );
	if ( d > 0 ) {
	  x+= 1;
	  d+= A;
	}
	y--;
	d-= B;
      } //end octant 7 while
      plot( s, c, x1, y1 );
    } //end octant 7   
  }//end octants 2 and 7  
} //end draw_line
