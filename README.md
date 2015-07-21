# deeplocalizer-classifier

## Build

Make sure you have OpenCV 2.4 and Boost 1.58.0 installed.
The code currently depends on Caffe's master branch. Check it out and compile it.
To build the code run:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This should build a static library `deeplocalizer-classifier` and the
`caffe_stas` program.

Or use the `caffe_stats` program. It prints a confusion matrix and extracts
images that got classified wrongly.

## caffe_stats

Tests a caffe model with all the data in `data/test` database and computes
a confusion matrix.

```
$ caffe_stats --weights models/conv12_conv48_fc1024_fc_2/model_iter_XXXXX.caffemodel \
              --data data/test \
              --model models/conv12_conv48_fc1024_fc_2/train_val.prototxt \
              --output_dir wrong_examples \
              --gpu
```
