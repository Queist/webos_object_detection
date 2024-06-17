source /home/snu/sdk/environment-setup-core2-64-webos-linux
cd ReactApp
npm run build
cp appinfo.json build/
cp icon.png build/
cd ..
cd PipelineService
mkdir BUILD
cd BUILD
cmake ..
make
cd ..
ares-package ../ReactApp/build ./pkg_x86_64
cd ..                        