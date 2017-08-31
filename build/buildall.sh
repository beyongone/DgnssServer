#/bin/sh

PWD=`pwd`
THIS_DIR=$(cd "$(dirname "$0")"; pwd)
ROOT_DIR=$THIS_DIR/..

cd $ROOT_DIR/AgnssCaster && mvn clean package && cd $PWD

# build website
# cd $ROOT_DIR/AgnssCaster/website && npm run build && cd $PWD

# caster
DIR_CASTER=$ROOT_DIR/build/AgnssCaster
mkdir -p $DIR_CASTER
cp $ROOT_DIR/AgnssCaster/target/AgnssCaster*.jar $DIR_CASTER/
cp $ROOT_DIR/AgnssCaster/src/main/conf/release/AgnssCaster.json $DIR_CASTER/AgnssCaster.json

cp $ROOT_DIR/setup/tcpkill.sh  $DIR_CASTER/
cp $ROOT_DIR/setup/AgnssCaster.sh  $DIR_CASTER/

#cp -rf $ROOT_DIR/AgnssCaster/webroot    $DIR_CASTER/

# zip
cd $THIS_DIR
tar -czf AgnssCaster.tar.gz  AgnssCaster

