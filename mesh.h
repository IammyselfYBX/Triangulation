#ifndef MESH_H
#define MESH_H

#include "problem-spec.h"

struct node{
    int node_id;
    double x;
    double y;
    double z;
    int bc;
};

struct edge{
    int edge_id;
    struct node *node[2];   // 一条边两个节点
    int bc;                 // 边界条件
};

/*
 * 定义三角形单元
 *  其中 edge_vector_x, edge_vector_y 中的数据可以由 node[3] 计算得到
 *  但是为了避免重复计算，所以直接定义这两个数组
 *  只要计算一次，然后存放到数组中，后面就不用再计算了 
*/
struct element{                                 // 三角形单元(element就是triangle)
    int element_id;                             // 单元编号
    struct node *node[3];                       // 一个三角形单元三个节点
    struct edge *edge[3];                       // 一个三角形单元三条边
    double edge_vector_x[3], edge_vector_y[3];  // 边向量的x,y分量
    double area;                                // 三角形单元面积
};

/*
 * 定义网格
 *  包含上面定义的node, edge, element元素
 *  以及上述内容的个数node_num, edge_num, element_num 
 */
struct mesh{
    struct node *nodes;       // 节点数组
    struct edge *edges;       // 边数组
    struct element *elements; // 单元数组
    int node_num;             // 节点个数
    int edge_num;             // 边个数
    int element_num;          // 单元个数
};

struct mesh *make_mesh(struct problem_spec *spec, double a);
void free_mesh(struct mesh *mesh);
#endif