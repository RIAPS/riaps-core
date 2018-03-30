set +e
export outputdir=`mktemp -d`
mkdir $outputdir/TimedTest
cp -r pkg/* $outputdir/TimedTest
export currentdir=`pwd`
cd  $outputdir
tar czvf TimedTest.tar.gz TimedTest/*
cd $currentdir
mv $outputdir/TimedTest.tar.gz .   

