[![DOI](https://img.shields.io/badge/DOI-10.1016/j.cagd.2026.102550-blue)](https://doi.org/10.1016/j.cagd.2026.102550)
# Element-Saving-Hexahedral-3-Refinement-Templates

![teaser](teaser.jpg)

## Citation

```angular2html
@article{tong2026element,
  title={Element-saving hexahedral 3-refinement templates},
  author={Tong, Hua and Zhang, Yongjie Jessica},
  journal={Computer Aided Geometric Design},
  pages={102550},
  year={2026},
  publisher={Elsevier}
}
```

## Installation

mkdir build

cd build

cmake ..

cmake --build .

## Execution

Place one or multiple input .obj triangle meshes in the folder inputBoundaries.

Run the program.

Multiple output .vtk hexahedral meshes will be generated in the folder outputFiles.

The output hex meshes, after removing outside elements, can be optimized using any post-optimization tools.

## Report

Please report any problems in Issues.
