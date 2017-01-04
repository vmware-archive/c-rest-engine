# !/bin/bash
TOPDIR=`pwd`
INDIR=$TOPDIR/data/input
OUTDIR=$TOPDIR/data/out
EXPECTEDDIR=$TOPDIR/data/expected
IPADDR="172.16.127.129"
PORT="81"

#=========================== TEST 1 : Large Data In Loop =========================================

for i in `seq 1 100`;
do
    rm -f $OUTDIR/resHeader.txt
    rm -f $OUTDIR/resData.txt

    curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET -d @$INDIR/largedata.txt http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

    header=$(<$OUTDIR/resHeader.txt)
    data=$(<$OUTDIR/resData.txt)

    inputData=$(<$INDIR/largedata.txt)

    if [ "$data" == "$inputData" ]
    then
       echo "PASSED-TEST $i: Large data"
    else
       echo "FAILED-TEST $i: Large data"
       break
    fi
done
