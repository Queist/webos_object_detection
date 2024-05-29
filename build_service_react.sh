source /usr/local/webos-sdk-x86_64/environment-setup-cortexa72-webos-linux
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