source /usr/local/webos-sdk-x86_64/environment-setup-core2-64-webos-linux
cd PipelineService
mkdir BUILD
cd BUILD
cmake ..
make
cd ..
ares-package ../App ./pkg_aarch64
cd ..