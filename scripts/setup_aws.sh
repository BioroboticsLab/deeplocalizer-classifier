#! /usr/bin/env bash

sudo apt-get update
sudo apt-get install --yes gcc g++ \
                          clang-3.6 clang-3.6++ \
                          cmake git ssh  wget \
                          qt5-default libqt5opengl5-dev \
                          libopencv-dev \
                          liblmdb-dev \
                          opencl-headers \
                          build-essential protobuf-compiler \
                          libprotoc-dev \
                          hdf5-tools \
                          libhdf5-serial-dev \
                          libsnappy1 \
                          libsnappy-dev \
                          libatlas-base-dev \
                          libgoogle-glog0 \
                          libgoogle-glog-dev \
                          libgflags-dev \
                          liblmdb-dev \
                          libleveldb-dev \
                          python-numpy \
                          libopenblas-base \
                          libopenblas-dev \
                          gfortran

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get -y install gcc-4.9 g++-4.9
sudo update-alternatives -y --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9

# install cuda
wget http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1404/x86_64/cuda-repo-ubuntu1404_7.0-28_amd64.deb
sudo dpkg -i cuda-repo-ubuntu1404_7.0-28_amd64.deb
sudo apt-get update
sudo apt-get --yes install linux-headers-generic
sudo apt-get --yes install cuda

cat <<'EOF' >> ~/.bashrc
export CUDA_HOME=/usr/local/cuda-7.0
export LD_LIBRARY_PATH=${CUDA_HOME}/lib64

PATH=${CUDA_HOME}/bin:${PATH}
export PATH
EOF

# http://stackoverflow.com/questions/12689304/ctypes-error-libdc1394-error-failed-to-initialize-libdc1394
sudo ln /dev/null /dev/raw1394


# compile & install Boost 1.58
sudo apt-get build-dep --yes libboost-dev
wget -O boost_1_58_0.tar.gz "http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz?r=http%3A%2F%2Fwww.boost.org%2Fusers%2Fhistory%2Fversion_1_58_0.html&ts=1433603736&use_mirror=vorboss"
tar -xf boost_1_58_0.tar.gz
cd ./boost_1_58_0
./bootstrap.sh --with-libraries=filesystem,program_options,system,serialization,thread,date_time --exec-prefix=/usr/local \
./b2
./b2 install


# compile & install Caffe
CAFFE_DIR="~/caffe"
if [ ! -e $CAFFE_DIR ]; then
    git clone https://github.com/BVLC/caffe.git $CAFFE_DIR
fi
mkdir -pv $CAFFE_DIR/build
(cd $CAFFE_DIR/build \
  && cmake -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DBLAS=open -DBUILD_SHARED_LIB=ON -DCMAKE_BUILD_TYPE=Release .. \
  && make all -j8 \
  && make install)


# compile & install Deeplocalizer
DEEP_LOCALIZER_DIR="~/deeplocalizer"
if [ ! -e $DEEP_LOCALIZER_DIR ]; then
    git clone git@github.com:berleon/deeplocalizer.git  $DEEP_LOCALIZER_DIR
fi
mkdir -pv $DEEP_LOCALIZER_DIR/build
(cd $DEEP_LOCALIZER_DIR
  cmake -DCaffe_DIR=$CAFFE_DIR/build/ -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DCMAKE_BUILD_TYPE=Release ..
  make -j8
  make install)

