ROOT=/home/dhruva/disk1/sanjay/Packages/LibRA/libra
local_exodus=$PWD/l_exodus
apps="roadrunner coyote hummbee mssplit tableinfo subms"
apps_bundle=libra.sh

# if test -d $local_exodus; then
#     rm -rf $local_exodus
# fi
sh $ROOT/scripts/install_exodus.sh $local_exodus;

strip $apps
$local_exodus/bin/exodus $apps -o $apps_bundle

mkdir -p $ROOT/{bin,lib,include}
cp $apps $apps_bundle $ROOT/bin
cp *.so $ROOT/lib

# The appropriate .h files aren't in apps/install yet
