#! /usr/bin/env bash

apt-get update
apt-get install --yes gcc g++ \
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

# http://stackoverflow.com/questions/12689304/ctypes-error-libdc1394-error-failed-to-initialize-libdc1394
ln /dev/null /dev/raw1394

ln /usr/bin/clang-3.6 /usr/bin/clang
ln /usr/bin/clang++-3.6 /usr/bin/clang++

apt-get build-dep --yes libboost-dev

wget -O /tmp/boost_1_58_0.tar.gz "http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz?r=http%3A%2F%2Fwww.boost.org%2Fusers%2Fhistory%2Fversion_1_58_0.html&ts=1433603736&use_mirror=vorboss"

(cd /tmp \
 && tar -xf boost_1_58_0.tar.gz \
 && cd ./boost_1_58_0 \
 && ./bootstrap.sh --with-libraries=filesystem,program_options,system,serialization,thread,date_time --exec-prefix=/usr/local \
 && ./b2 \
 && ./b2 install )

wget http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1404/x86_64/cuda-repo-ubuntu1404_7.0-28_amd64.deb
dpkg -i cuda-repo-ubuntu1404_7.0-28_amd64.deb
apt-get install cuda

cat <<'EOF' >> ~/.bashrc
export CUDA_HOME=/usr/local/cuda-7.0
export LD_LIBRARY_PATH=${CUDA_HOME}/lib64

PATH=${CUDA_HOME}/bin:${PATH}
export PATH
EOF

CAFFE_DIR="opt/caffe"
if [ ! -e $CAFFE_DIR ]; then
    git clone https://github.com/BVLC/caffe.git $CAFFE_DIR
fi
mkdir -pv $CAFFE_DIR/build
(cd $CAFFE_DIR/build \
  && cmake -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DBLAS=open  -DCMAKE_BUILD_TYPE=Release .. \
  && make all \
  && make install)


DEEP_LOCALIZER_DIR="/opt/deeplocalizer"
if [ ! -e $DEEP_LOCALIZER_DIR ]; then
    git clone git@github.com:berleon/deeplocalizer.git  $DEEP_LOCALIZER_DIR
fi
mkdir -pv $DEEP_LOCALIZER_DIR/build
(cd $DEEP_LOCALIZER_DIR
  cmake -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DCMAKE_BUILD_TYPE=Release ..
  make
  make install)

