#!/bin/sh

echo "premake5 gmake2 --cc=clang"
echo "Generating projects..."
vendor/premake/linux/premake5 gmake2 --cc=clang

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error: "$ERRORLEVEL && exit
fi
