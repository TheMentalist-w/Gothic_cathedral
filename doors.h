#ifndef DOORS_H_INCLUDED
#define DOORS_H_INCLUDED

int	doorsVertexCount = 6;

float doorsVertices[] = { 0.342475, -0.5, 0, 1,
-0.342475, 0.5, 0, 1,
-0.342475, -0.5, 0, 1,
0.342475, -0.5, 0, 1,
0.342475, 0.5, 0, 1,
-0.342475, 0.5, 0, 1 };

float doorsNormals[] = { 0, 0, 1, 0,
0, 0, 1, 0,
0, 0, 1, 0,
0, 0, 1, 0,
0, 0, 1, 0,
0, 0, 1, 0 };

float doorsTexCoords0[] = { 1, 0,
0, 1,
0, 0,
1, 0,
1, 1,
0, 1 };

float doorsTexCoords1[] = { 0.835409, 0.248458,
0.999816, 0.000190293,
0.999816, 0.248458,
0.835409, 0.248458,
0.835409, 0.000190293,
0.999816, 0.000190293 };

#endif // MYCUBE_H_INCLUDED