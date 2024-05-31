source /home/snu/sdk/environment-setup-core2-64-webos-linux
cd ReactApp
npm build
cp appinfo.json build/
cd ..
cd PipelineService
mkdir BUILD
cd BUILD
cmake ..
make
cd ..
ares-package ../ReactApp/build ./pkg_aarch64
cd ..