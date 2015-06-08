#! /usr/bin/env bash

set -e

TEST_PATHFILE="test_add_border_pathfile.txt"
TEST_IMG=$(realpath ../testdata/with_one_tag.jpeg)
WITH_BORDER="/tmp/add_border/with_one_tag_wb.jpeg"
OUTPUT_PATHFILE="add_border_pathfile.txt"
rm -f $WITH_BORDER
echo "Does $TEST_IMG exists?"
test -e $TEST_IMG

cd ../../source
echo $TEST_IMG > $TEST_PATHFILE
echo "Given a file of image path"
./add_border -o /tmp/add_border --output-pathfile $OUTPUT_PATHFILE $TEST_PATHFILE
echo "Then ./add_border will add a border to the image"
test -e $WITH_BORDER

echo "Then ./add_border will create an output pathfile"
test -e $OUTPUT_PATHFILE

echo "The output pathfile contains the path to the image with the new border"
test "`cat $OUTPUT_PATHFILE`" == "${WITH_BORDER}"

rm -f $TEST_PATHFILE $WITH_BORDER $OUTPUT_PATHFILE