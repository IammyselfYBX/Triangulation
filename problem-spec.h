/*
 * problem-spec.h
 *  问题描述
 *  ---------------
 *  | ∇·(η(x,y)∇u(x,y)) + f(x,y) = 0 , (x,y)∈Ω
 *  | u = g                          , (x,y)∈ Γ_D(∂Ω_D)
 *  | η·∂u/∂n = h                    , (x,y)∈ Γ_N(∂Ω_N)
 *  ---------------
 * 
 *  u_exact: 精确解, 通常是未知的，但是为了测试代码和计算误差，可以给出一个精确解
 */


#ifndef PROBLEM_SPEC_H_
#define PROBLEM_SPEC_H_

#include <math.h>

// 狄利克雷(Dirichlet)边值条件 boundary condition
#define FEM_BC_DIRICHLET 2
// 诺依曼(Neumann)边值条件 
#define FEM_BC_NEUMANN   3

// 问题规格(problem specification)点(points/vertices)
struct problem_spec_point
{
    int point_id; // 点的编号(从0开始)
    double x;     // 点的x坐标
    double y;     // 点的y坐标
    int bc;       // 点的边界条件(FEM_BC_DIRICHLET/FEM_BC_NEUMANN)
};

// 问题规格边(segmennts/edges)
struct problem_spec_segment
{
    int segment_id; // 线段的编号(从0开始)
    int point_id1;  // 线段的端点1
    int point_id2;  // 线段的端点2
    int bc;         // 线段的边界条件(FEM_BC_DIRICHLET/FEM_BC_NEUMANN)
};

/* 
 * 问题规格洞
 *  通过洞中任意一个点的坐标来替代洞的信息
 */
struct problem_spec_hole
{
    double x; // 洞中点x坐标
    double y; // 洞中点y坐标
};

// 问题规格
struct problem_spec
{
    struct problem_spec_point *points;     // 点
    struct problem_spec_segment *segments; // 线段
    struct problem_spec_hole *holes;       // 洞 (如果没有洞 holes = NULL)
    int num_points;                        // 点的数量
    int num_segments;                      // 线段的数量
    int num_holes;                         // 洞的数量(如果没有洞 num_holes = 0)

    double (*f)(double x, double y);       // 二阶PDE右端项
    double (*u_exact)(double x, double y); // 精确解
    double (*g)(double x, double y);       // 狄利克雷边界条件的右端项
    double (*h)(double x, double y);       // 纽曼边界条件的右端项
    double (*eta)(double x, double y);     // 纽曼边界条件的函数
};

struct problem_spec *triangle_with_hole(void);
struct problem_spec *annulus(int n);
void free_annulus(struct problem_spec *spec);
struct problem_spec *square(void);
void free_square(struct problem_spec *spec);
struct problem_spec *three_holes(int n);
void free_three_holes(struct problem_spec *spec);

#endif