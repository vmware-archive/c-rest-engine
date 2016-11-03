# !/bin/bash
TOPDIR=`pwd`
INDIR=$TOPDIR/data/input
OUTDIR=$TOPDIR/data/out
EXPECTEDDIR=$TOPDIR/data/expected
IPADDR="172.16.127.132"
PORT="81"

#=========================== TEST 1 : Echo Small data =========================================

for i in `seq 1 1000`;
do
    rm -f $OUTDIR/resHeader.txt
    rm -f $OUTDIR/resData.txt

    curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET -d @$INDIR/smalldata.txt http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt
    header=$(<$OUTDIR/resHeader.txt)
    data=$(<$OUTDIR/resData.txt)

    inputData=$(<$INDIR/smalldata.txt)

    if [ "$data" == "$inputData" ]
    then
        echo "PASSED-TEST $i - Small data test"
    else
        echo "FAILED-TEST $i: Small data test"
        break
    fi
done
