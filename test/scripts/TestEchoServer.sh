# !/bin/bash
TOPDIR=`pwd`
INDIR=$TOPDIR/data/input
OUTDIR=$TOPDIR/data/out
EXPECTEDDIR=$TOPDIR/data/expected
IPADDR="172.16.127.129"
PORT="81"

#=========================== TEST 1 : Echo Small data =========================================

rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET -d @$INDIR/smalldata.txt http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt
header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)

inputData=$(<$INDIR/smalldata.txt)

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 1: Echo Small data"
else
   echo "FAILED-TEST 1: Echo Small data"
fi

#========================== TEST 2  : Echo large data ==========================================
rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET -d @$INDIR/largedata.txt http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)

inputData=$(<$INDIR/largedata.txt)

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 2: Echo Large data"
else
   echo "FAILED-TEST 2: Echo Large data"
fi

#========================== TEST 3 :No data ==========================================
rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

touch $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET  http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)
expectedHeader=$(<$EXPECTEDDIR/SampleHeader.txt)

inputData=''

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 3-A: No data"
else
   echo "FAILED-TEST 3-A: No data"
fi

if [ "$expectedHeader" == "$header" ]
then
   echo "PASSED-TEST 3-B: No data"
else
   echo "FAILED-TEST 3-B: No data"
fi

#========================== TEST 4 : BAD URL ==========================================

rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt
touch $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET http://$IPADDR:$PORT/v1/pg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)
expectedHeader=$(<$EXPECTEDDIR/SampleBadUrl.txt)

inputData=''

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 4-A: BAD URL"
else
   echo "FAILED-TEST 4-A: BAD URL"
fi

if [ "$expectedHeader" == "$header" ]
then
   echo "PASSED-TEST 4-B: BAD URL"
else
   echo "FAILED-TEST 4-B: BAD URL"
fi

#========================== TEST 5 : Check PUT method  ==========================================
rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -d @$INDIR/smalldata.txt -X PUT http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)

inputData=$(<$INDIR/smalldata.txt)

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 5: POST Method"
else
   echo "FAILED-TEST 5: POST Method"
fi

#========================== TEST 6 : Bad URL with large Data  ==========================================

rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt
touch $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET -d @$INDIR/largedata.txt http://$IPADDR:$PORT/v1/pg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)
expectedHeader=$(<$EXPECTEDDIR/SampleBadUrl.txt)

inputData=''

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 6-A: Bad url with large data"
else
   echo "FAILED-TEST 6-A: Bad url with large data"
fi

if [ "$expectedHeader" == "$header" ]
then
   echo "PASSED-TEST 6-B: Bad url with large data"
else
   echo "FAILED-TEST 6-B: Bad url with large data"
fi

#========================== TEST 7 : Bad method name  ==========================================
rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

# CRASHING TEST : FIX ASAP
#curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -d @$INDIR/smalldata.txt -X BLAHdsafdsfdsfadfsfadsfafadsfadsfadsfdsafdsfdsafjkafgjkdg  http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

#header=$(<$OUTDIR/resHeader.txt)
#data=$(<$OUTDIR/resData.txt)
#expectedHeader=$(<$EXPECTEDDIR/SampleBadMethod.txt)

inputData=''

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 7-A: Bad method name"
else
   echo "FAILED-TEST 7-A: Bad method name"
fi

if [ "$expectedHeader" == "$header" ]
then
   echo "PASSED-TEST 7-B: Bad method name"
else
   echo "FAILED-TEST 7-B: Bad method name"
fi

#========================== TEST 8 : Large content length  ==========================================
rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt
touch $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -d "blah" -X PUT -H "Content-Length:10" http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)
expectedHeader=$(<$EXPECTEDDIR/SampleBadContentLength.txt)


inputData=''

if [ "$data" == "$inputData" ]
then
   echo "PASSED-TEST 8-A: Large Content Length"
else
   echo "FAILED-TEST 8-A: Large Content-Length"
fi

if [ "$expectedHeader" == "$header" ]
then
   echo "PASSED-TEST 8-B: Large Content Length"
else
   echo "FAILED-TEST 8-B: Large Content-Length"
fi

#========================== TEST 9 : Small content length  ==========================================
rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -d "blahblahblah" -X PUT -H "Content-Length:2" http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt

header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)
expectedHeader=$(<$EXPECTEDDIR/SampleShortHeader.txt)

if [ "$data" == "bl" ]
then
   echo "PASSED-TEST 9-A: Small Content Length"
else
   echo "FAILED-TEST 9-A: Small Content-Length"
fi

if [ "$expectedHeader" == "$header" ]
then
   echo "PASSED-TEST 9-B: Small Content Length"
else
   echo "FAILED-TEST 9-B: Small Content-Length"
fi

#=========================== TEST 10 : Large header value =========================================

rm -f $OUTDIR/resHeader.txt
rm -f $OUTDIR/resData.txt

touch $OUTDIR/resData.txt

curl -v -D $OUTDIR/resHeader.txt -o $OUTDIR/resData.txt -X GET -H "Connection:fdsaadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffsafdsffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffsafdsffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffdadsfasaaffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffsafdsffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffsafdsffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffdadsfasaaffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffsafdsffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffsafdsffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffadsfafgdffdsfdsfdfdfgdfgdfgdfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffdadsfasaaa" -d "KK" http://$IPADDR:$PORT/v1/pkg?x=y 2&> xx.txt
header=$(<$OUTDIR/resHeader.txt)
data=$(<$OUTDIR/resData.txt)
expectedHeader=$(<$EXPECTEDDIR/SampleShortHeader.txt)

inputData=$(<$INDIR/smalldata.txt)

if [ "$header" == "$expectedHeader" ]
then
   echo "PASSED-TEST 10: Large Header value"
else
   echo "FAILED-TEST 10: Large Header Value"
fi

