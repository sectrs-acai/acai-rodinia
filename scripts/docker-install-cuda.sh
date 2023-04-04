#
# XXX Installing nvcc with support for fermi is a real pain
# We automate this in the container
#
mkdir -p /build
cd /build
wget https://developer.nvidia.com/compute/cuda/8.0/Prod2/local_installers/cuda_8.0.61_375.26_linux-run
sh ./cuda*run --tar mxvf
cp InstallUtils.pm /usr/lib/x86_64-linux-gnu/perl-base
export $PERL5LIB
sh ./run_files/cuda-linux64-rel-8.0.61-21551265.run -noprompt

ls -al /usr/local/cuda
