#include <stdio.h>
#include "triangle.h"
#include "xmalloc.h"
#include "myarray.h"
#include "mesh.h"
#include "problem-spec.h"

static struct triangulateio *problem_spec_to_triangle(struct problem_spec *spec)
{
	int i;
	struct triangulateio *in = xmalloc(sizeof *in);

	/* process points */
	in->numberofpoints = spec->num_points;
	make_vector(in->pointlist, 2 * in->numberofpoints);
	for (i = 0; i < in->numberofpoints; i++) {
		in->pointlist[2*i]   = spec->points[i].x;
		in->pointlist[2*i+1] = spec->points[i].y;
	}

	make_vector(in->pointmarkerlist, in->numberofpoints);
	for (i = 0; i < in->numberofpoints; i++)
		in->pointmarkerlist[i] = spec->points[i].bc;

	in->numberofpointattributes = 0;

	/* process segments */
	in->numberofsegments = spec->num_segments;
	make_vector(in->segmentlist, 2 * in->numberofsegments);
	for (i = 0; i < in->numberofsegments; i++) {
		in->segmentlist[2*i]   = spec->segments[i].point_id1;
		in->segmentlist[2*i+1] = spec->segments[i].point_id2;
	}

	make_vector(in->segmentmarkerlist, in->numberofsegments);
	for (i = 0; i < in->numberofsegments; i++)
		in->segmentmarkerlist[i] = spec->segments[i].bc;

	/* process holes */
	in->numberofholes = spec->num_holes;
	if (in->numberofholes != 0) {
		make_vector(in->holelist, 2 * in->numberofholes);
		for (i = 0; i < in->numberofholes; i++) {
			in->holelist[2*i]   = spec->holes[i].x;
			in->holelist[2*i+1] = spec->holes[i].y;
		}
	} else
		in->holelist = NULL;

	in->numberofregions = 0;

	return in;
}

static struct triangulateio *do_triangulate(struct triangulateio *in, double a)
{
	char *opts_string = "Qzpeq30a%f";
	char opts[64];
	struct triangulateio *out = xmalloc(sizeof *out);

	out->pointlist = NULL;
	out->pointmarkerlist = NULL;
	out->edgelist = NULL;
        out->edgemarkerlist = NULL;
	out->trianglelist = NULL;
	out->segmentlist = NULL;
        out->segmentmarkerlist = NULL;
	sprintf(opts, opts_string, a);
	triangulate(opts, in, out, NULL);

	return out;
}

/* This is ugly; there ought to be a better way but I don't see it */
static void assign_elem_edges(
		struct element *elements, int element_num,
		struct edge *edges, int edge_num)
{
	for (int r = 0; r < element_num; r++) {
		for (int i = 0; i < 3; i++) {	/* i: vertex index */
			int j = (i+1)%3;
			int k = (i+2)%3;
			int n1 = elements[r].node[j]->node_id;
			int n2 = elements[r].node[k]->node_id;
			for (int s = 0; s < edge_num; s++) {
				int m1 = edges[s].node[0]->node_id;
				int m2 = edges[s].node[1]->node_id;
				if ((m1 == n1 && m2 == n2)
						|| (m1 == n2 && m2 == n1)) {
					elements[r].edge[i] = &edges[s];
					break;
				}
			}
		}
	}
}

static void set_element_edge_vectors(struct element *ep)
{
	for (int i = 0; i < 3; i++) {	/* i: vertex index */
		int j = (i+1)%3;
		int k = (i+2)%3;
		ep->edge_vector_x[i] = ep->node[k]->x - ep->node[j]->x;
		ep->edge_vector_x[i] = ep->node[k]->y - ep->node[j]->y;
	}
}

static void set_element_area(struct element *ep)
{
	ep->area = (ep->edge_vector_x[0]*ep->edge_vector_x[1] - ep->edge_vector_x[1]*ep->edge_vector_x[0])/2.0;
}

static void set_edge_vectors_and_areas(struct element *elements, int element_num)
{
	for (int i = 0; i < element_num; i++) {
		struct element *ep = &elements[i];
		set_element_edge_vectors(ep); 
		set_element_area(ep); 
	}
}

static struct mesh *triangle_to_mesh(struct triangulateio *out)
{
	struct node *nodes;
	struct edge *edges;
	struct element *elements;
	int i, node_num, edge_num, element_num;
	struct mesh *mesh = xmalloc(sizeof *mesh);

	node_num = out->numberofpoints;
	make_vector(nodes, node_num);
	for (i = 0; i < out->numberofpoints; i++) {
		nodes[i].node_id = i;
		nodes[i].x = out->pointlist[2*i];
		nodes[i].y = out->pointlist[2*i+1];
		nodes[i].z = 0.0;
		nodes[i].bc = out->pointmarkerlist[i];
	}

	edge_num = out->numberofedges;
	make_vector(edges, edge_num);
	for (i = 0; i < edge_num; i++) {
		edges[i].edge_id = i;
		edges[i].node[0] = &nodes[out->edgelist[2*i]];
		edges[i].node[1] = &nodes[out->edgelist[2*i+1]];
		edges[i].bc = out->edgemarkerlist[i];
	}

	element_num = out->numberoftriangles;
	make_vector(elements, element_num);
	for (i = 0; i < element_num; i++) {
		elements[i].element_id = i;
		elements[i].node[0] = &nodes[out->trianglelist[3*i]];
		elements[i].node[1] = &nodes[out->trianglelist[3*i+1]];
		elements[i].node[2] = &nodes[out->trianglelist[3*i+2]];
	}

	assign_elem_edges(elements, element_num, edges, edge_num);
	set_edge_vectors_and_areas(elements, element_num);

	mesh->node_num = node_num;
	mesh->edge_num = edge_num;
	mesh->element_num = element_num;
	mesh->nodes = nodes;
	mesh->edges = edges;
	mesh->elements = elements;

	return mesh;
}

static void free_triangle_in_structure(struct triangulateio *in)
{
	free_vector(in->pointlist);
	free_vector(in->pointmarkerlist);
	free_vector(in->segmentlist);
	free_vector(in->segmentmarkerlist);
	free_vector(in->holelist);
	free(in);
}

static void free_triangle_out_structure(struct triangulateio *out)
{

	free(out->pointlist);
	free(out->pointmarkerlist);
	free(out->edgelist);
	free(out->edgemarkerlist);
	free(out->trianglelist);
	free(out->segmentlist);
	free(out->segmentmarkerlist);
	free(out);
}

/**
 * @name make_mesh - 生成网格
 * @param 1.spec 问题规格 2.a 内圆半径
 * @return 网格
 * @note
 * 	生成网格的步骤
 * 	1.将问题描述(problem_spec)转换为三角形(triangulateio)结构
 * 	2.把三角形(triangulateio)结构传递给三角形(triangle)程序
 * 	3.将三角形(triangulateio)结构转换为网格(mesh)结构
*/
struct mesh *make_mesh(struct problem_spec *spec, double a)
{
	struct triangulateio *in, *out;
	struct mesh *mesh;

	in = problem_spec_to_triangle(spec);
	out = do_triangulate(in, a);
	mesh = triangle_to_mesh(out);
	free_triangle_in_structure(in);
	free_triangle_out_structure(out);
	return mesh;
}

void free_mesh(struct mesh *mesh)
{
	if (mesh == NULL)
		return;

	free_vector(mesh->nodes);
	free_vector(mesh->edges);
	free_vector(mesh->elements);
	free(mesh);
}
