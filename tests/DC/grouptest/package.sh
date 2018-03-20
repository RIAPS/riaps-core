set +e
export outputdir=`mktemp -d`
mkdir $outputdir/Grouptest
cp -r pkg/* $outputdir/Grouptest
export currentdir=`pwd`
cd  $outputdir
tar czvf Grouptest.tar.gz Grouptest/*
cd $currentdir
mv $outputdir/Grouptest.tar.gz .   

