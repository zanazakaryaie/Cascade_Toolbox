# Detection_Toolbox
This is a toolbox to train, test, and run object detection algorithms. 

# Dependencies
- Cmake
- OpenCV (versions 2 or 3 - built with Boost)
- [Simd](https://github.com/ermig1979/Simd) (Optional)

# How to build?
```
git clone https://github.com/zanazakaryaie/Detection_Toolbox.git
cd Detection_Toolbox
mkdir build && cd build
cmake ..
make
```
If you want to use Simd library to accelerate the detection, replace the cmake command by:
```
cmake -DBUILD_SIMD_INFERENCE=ON -DSIMD_LIBRARY_PATH=/home/odroid/Simd ..
```
# How to use?
Run the generated executables in the build folder following this order:
- collect_train_data
- collect_test_data
- train
- test
- hard_negative_mine
- inference
- inference_simd (if linked with Simd library)

# Tutorials
For more details read my posts on [cascade algorithm](http://imrid.net/?p=4367) and [training a cascade model](http://imrid.net/?p=4378)


