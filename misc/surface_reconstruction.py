#! python3

import open3d as o3d
import numpy as np
import os
import sys

if __name__ == "__main__":
    fname = sys.argv[1]
    print(fname)
    ply_point_cloud = o3d.data.PLYPointCloud()
    pcd = o3d.io.read_point_cloud(fname)
    print(pcd)
    # print(np.asarray(pcd.points))

    hull, _ = pcd.compute_convex_hull()
    hull_ls = o3d.geometry.LineSet.create_from_triangle_mesh(hull)
    hull_ls.paint_uniform_color((1, 0, 0))
    o3d.visualization.draw_geometries([pcd, hull_ls])


