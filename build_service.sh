source /home/snu/sdk/environment-setup-core2-64-webos-linux
cd PipelineService
mkdir BUILD
cd BUILD
cmake ..
make
cd ..
ares-package ../App ./pkg_x86_64
cd ..