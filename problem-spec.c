/**
 * @file problem-spec.c
 * @brief 包含问题描述
 * 
*/
#include "problem-spec.h"
#include "xmalloc.h"
#include "myarray.h"
#include <stdlib.h>
#include <stdio.h>

struct problem_spec *triangle_with_hole(void)
{
    // 点的信息
    static struct problem_spec_point points[] = {
        // 三角形顶点
        {0, -1.0, 0.0, FEM_BC_DIRICHLET},
        {1,  1.0, 0.0, FEM_BC_DIRICHLET},
        {2,  0.0, 2.0, FEM_BC_DIRICHLET},
        // 洞的顶点
        {3, -0.25, 0.25, FEM_BC_DIRICHLET},
        {4, +0.25, 0.25, FEM_BC_DIRICHLET},
        {5, +0.25, 1.0,  FEM_BC_DIRICHLET},
        {6, -0.25, 1.0,  FEM_BC_DIRICHLET},
    };

    // 边的信息
    static struct problem_spec_segment segments[] = {
        // 三角形边
        {0, 0, 1, FEM_BC_DIRICHLET},
        {1, 1, 2, FEM_BC_DIRICHLET},
        {2, 2, 0, FEM_BC_DIRICHLET},
        // 洞的边
        {3, 3, 4, FEM_BC_DIRICHLET},
        {4, 4, 5, FEM_BC_DIRICHLET},
        {5, 5, 6, FEM_BC_DIRICHLET},
        {6, 6, 3, FEM_BC_DIRICHLET},
    };

    // 洞的标识信息
    static struct problem_spec_hole holes[] = {
        {0.0, 0.5},
    };

    static struct problem_spec spec = {
        .points         = points,
        .num_points     = sizeof(points)/sizeof(points[0]),
        .segments       = segments,
        .num_segments   = sizeof(segments)/sizeof(segments[0]),
        .holes          = holes,
        .num_holes      = sizeof(holes)/sizeof(holes[0]),
        .f              = NULL,
        .u_exact        = NULL,
        .g              = NULL,
        .h              = NULL,
        .eta            = NULL,
    };

    printf("带孔的三角形区域\n");
    return &spec;
}

struct problem_spec *annulus(int n)
{
    double PI = 4*atan(1.0);
    double a = 0.325, b=2*a; 
    // a是内圆半径，b是外圆半径
    // 之所以这里 a = 0.325, b = 2*a 是因为 π(b²-a²)≈0.995,
    // 这样可以使得环形区域的面积接近1×1的矩形
    struct problem_spec *spec = xmalloc(sizeof(*spec));
    // 生成点、边的数量是2n个
    // 前n个点是内圆上的点，后n个点是外圆上的点
    make_vector(spec->points, 2*n);
    make_vector(spec->segments, 2*n);
    make_vector(spec->holes, 1);

    /* 
     * 定义点
     *  这里采用比较巧妙的定义方法
     *  按照角度旋转的方式定义点，旋转一圈，就得到了内外部所有的点
     *  ------
     *  | 内部点
     *  | xᵢ= a cos(2πi/n)  
     *  | yᵢ= a sin(2πi/n)
     *  ------
     *  
     *  ------
     *  | 外部点
     *  | xₙ₊ᵢ= b cos(2πi/n)
     *  | yₙ₊ᵢ= a sin(2πi/n)
     *  ------
     *  i = 0,1, ..., n-1
     */
    for (int i = 0; i < n; i++)
    {
        double t = 2*PI*i/n; // 角度
        spec->points[i].point_id = i;
        spec->points[i].x = a*cos(t);
        spec->points[i].y = a*sin(t);
        spec->points[i].bc = FEM_BC_DIRICHLET;
        spec->points[i+n].point_id = i+n;
        spec->points[i+n].x = b*cos(t);
        spec->points[i+n].y = b*sin(t);
        spec->points[i+n].bc = FEM_BC_DIRICHLET;
    }
    
    /* 
     * 定义边
     *  整体思路同定义点，也是按照角度旋转的方式定义边
     *  但是区别就是最后的边的终点(n-1)是起点(0)
     *                         (2n-1)是起点(n)
     *  所以这里最后两行特殊处理这个问题
     */
    for (int i = 0; i < n; i++)
    {
        spec->segments[i].segment_id = i;
        spec->segments[i].point_id1 = i;
        spec->segments[i].point_id2 = i+1;    
        spec->segments[i].bc = FEM_BC_DIRICHLET;
        spec->segments[i+n].segment_id = i+n;
        spec->segments[i+n].point_id1 = i+n;
        spec->segments[i+n].point_id2 = i+1+n;
        spec->segments[i+n].bc = FEM_BC_DIRICHLET;
    }
    spec->segments[n-1].point_id2 -= n;
    spec->segments[2*n-1].point_id2 -= n;   

    /*
     * 定义洞的标识点
     *   这里选择(0,0) 当然，选择洞中其他点也是可以的
     */
    spec->holes[0].x = spec->holes[0].y = 0.0;

    spec->num_points = 2*n;
    spec->num_segments = 2*n;
    spec->num_holes = 1;
    spec->f = spec->u_exact = spec->g = spec->h = spec->eta = NULL;
    printf("环形区域(%d角形), 内部半径%g, 外部半径%g\n", n, a, b);
    return spec;
}

void free_annulus(struct problem_spec *spec)
{
    if(spec != NULL){
        free_vector(spec->points);
        free_vector(spec->segments);
        free_vector(spec->holes);
        free(spec);
    }
}

struct problem_spec *square(void)
{
    // 点的信息
    static struct problem_spec_point points[] = {
        // 矩形顶点
        {0, 0.0, 0.0, FEM_BC_DIRICHLET},
        {1, 1.0, 0.0, FEM_BC_DIRICHLET},
        {2, 1.0, 3.0, FEM_BC_DIRICHLET},
        {3, 0.0, 3.0, FEM_BC_DIRICHLET},
        // 洞的顶点
        {4, 0.25, 0.25, FEM_BC_DIRICHLET},
        {5, 0.75, 0.25, FEM_BC_DIRICHLET},
        {6, 0.75, 0.75, FEM_BC_DIRICHLET},
        {7, 0.25, 0.75, FEM_BC_DIRICHLET},
    };

    // 边的信息
    static struct problem_spec_segment segments[] = {
        // 矩形边
        {0, 0, 1, FEM_BC_DIRICHLET},
        {1, 1, 2, FEM_BC_DIRICHLET},
        {2, 2, 3, FEM_BC_DIRICHLET},
        {3, 3, 0, FEM_BC_DIRICHLET},
        // 洞的边
        {4, 4, 5, FEM_BC_DIRICHLET},
        {5, 5, 6, FEM_BC_DIRICHLET},
        {6, 6, 7, FEM_BC_DIRICHLET},
        {7, 7, 4, FEM_BC_DIRICHLET},
    };

    // 洞的标识信息
    static struct problem_spec_hole holes[] = {
        {0.5, 0.5},
    };

    static struct problem_spec spec = {
        .points         = points,
        .num_points     = sizeof(points)/sizeof(points[0]),
        .segments       = segments,
        .num_segments   = sizeof(segments)/sizeof(segments[0]),
        .holes          = holes,
        .num_holes      = sizeof(holes)/sizeof(holes[0]),
        .f              = NULL,
        .u_exact        = NULL,
        .g              = NULL,
        .h              = NULL,
        .eta            = NULL,
    };

    printf("正方形区域\n");
    return &spec;
}

void free_three_holes(struct problem_spec *spec)
{
    if(spec != NULL){
        free_vector(spec->points);
        free_vector(spec->segments);
        free_vector(spec->holes);
        free(spec);
    }
}
