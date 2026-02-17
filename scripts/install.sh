BASEDIR=$(dirname "$0")

cd $BASEDIR/..

sudo cmake --workflow --preset workflow-release-install
