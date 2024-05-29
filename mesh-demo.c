/**
 * @file mesh-demo.c
 * @brief 该文件包含了生成网格的示例
 * @details 该文件包含了生成网格的示例，包括三角形网格、圆环网格、正方形网格、三洞圆环网格等
 * 返回结果生成一个EPS图像格式
*/

#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "problem-spec.h"
#include "triangle.h"

static void do_demo(struct problem_spec *spec, double a, char *eps_filename){
    struct mesh *mesh = make_mesh(spec, a);
    printf("网格生成完毕\n");
    printf("节点个数: %d, 边个数: %d, 面个数: %d\n", mesh->node_num, mesh->edge_num, mesh->element_num);
    mesh_to_eps(mesh, eps_filename);
    free_mesh(mesh);
}

/*
 * @name: show_usage
 * @msg: 打印程序使用方法
 * @param: progname 程序名
 */
static void show_usage(char *progname){
    // 打印程序使用方法
    // 检查argv,如果参数个数不对，打印程序使用方法，并退出
        printf("Usage: %s <a> \n", progname);
        printf("  a是每个小三角形中最大的面积\n");
        exit(1);
}

int main(int argc, char *argv[])
{
    struct problem_spec *square(void);
    struct problem_spec *triangle_with_hole(void);
    struct problem_spec *annulus(int n);
    void free_annulus(struct problem_spec *spec);    
    struct problem_spec *three_holes(int n);
    void free_three_holes(struct problem_spec *spec);
    struct problem_spec *spec;
    
    char *endptr; // 用于strtod函数
    double a;

    if (argc != 2){
        show_usage(argv[0]);
    }
    a = strtod(argv[1], &endptr); // strtod将字符串转换为double类型

    printf("-----------------------------------\n");
    printf("三角形带孔区域\n");
    do_demo(triangle_with_hole(), a, "triangle-with-hole.eps");

    printf("-----------------------------------\n");
    printf("圆环区域\n");
    spec = annulus(24);
    do_demo(spec, a, "annulus.eps");
    free_annulus(spec);

    printf("-----------------------------------\n");
    printf("方形区域\n");
    do_demo(square(), a, "square.eps");

    printf("-----------------------------------\n");

    return 1;
}