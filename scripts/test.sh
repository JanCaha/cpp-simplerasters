BASEDIR=$(dirname "$0")

cd $BASEDIR/..

cmake --workflow --preset workflow-debug-tests
