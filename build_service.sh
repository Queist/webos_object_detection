cd PipelineService
mkdir BUILD
cd BUILD
cmake ..
make
cd ..
ares-package ../App ./pkg_aarch64
cd ..