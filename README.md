# Cascade_Toolbox
This is a toolbox to train, test, and run HAAR/LBP cascade easily. 

## Dependencies
- Cmake
- OpenCV (versions 2 or 3 - built with Boost)
- [Simd](https://github.com/ermig1979/Simd) (Optional)

## How to build?
```
git clone https://github.com/zanazakaryaie/Cascade_Toolbox.git
cd Cascade_Toolbox
mkdir build && cd build
cmake ..
make
```
If you want to use Simd library to accelerate the detection, replace the cmake command by:
```
cmake -DBUILD_SIMD_INFERENCE=ON -DSIMD_LIBRARY_PATH=~/Simd ..
```
## How to use?
Run the generated executables in the build folder following this order:

1. **collect_train_data**
    Input: a training video (.mp4 or .avi)
    Instructions: draw and edit positive boxes with mouse (no need to draw negatives), press space to move to the next frames
    Output: Positive and Negative folders that contain sample images
    
2. **collect_test_data**
    Input: a testing video (.mp4 or .avi)
    Instructions: draw and edit positive boxes with mouse, press space to move to the next frames
    Output: a folder that contain .txt annotations of each frame
    
3. **train**
    Input: configs.yaml file, Positive and Negative folders
    Instructions: just wait for the training to finish
    Output: trained model in .xml format
    
4. **test**
    Input: the testing video, the testing directory (output of 2), and the trained .xml model 
    Output: prints the Precision and Recall of the trained detector 
   
5. **hard_negative_mine**
    Input: the training video, the trained .xml model
    Instructions: right-click in false positives, press space to move to the next frames.
    Output: new negative images added to the Negative folder

6. **train**
    This is for including hard negatives and training a more accurate model. 

7. **inference** or **inference_simd** (if linked with Simd library)
    Input: a video (.mp4 or .avi) and the trained .xml model
    Output: the video played with detected objects and FPS

## Appendix
For more details read my posts on [cascade algorithm](http://imrid.net/?p=4367) and [training a cascade model](http://imrid.net/?p=4378)


