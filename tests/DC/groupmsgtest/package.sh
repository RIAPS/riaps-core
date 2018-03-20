set +e
export outputdir=`mktemp -d`
mkdir $outputdir/Groupmsgtest
cp -r pkg/* $outputdir/Groupmsgtest
export currentdir=`pwd`
cd  $outputdir
tar czvf Groupmsgtest.tar.gz Groupmsgtest/*
cd $currentdir
mv $outputdir/Groupmsgtest.tar.gz .   

