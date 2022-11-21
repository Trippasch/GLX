#!/bin/sh

echo "Cleaning projects..."

echo "find . -name 'Makefile' -delete"
find . -name 'Makefile' -delete

echo "rm -rf ./bin ./vendor/Glad/bin ./vendor/GLFW/bin ./vendor/ImGui/bin"
rm -rf ./bin ./vendor/Glad/bin ./vendor/GLFW/bin ./vendor/ImGui/bin 

echo "rm -rf ./bin-int ./vendor/Glad/bin-int ./vendor/GLFW/bin-int ./vendor/ImGui/bin-int"
rm -rf ./bin-int ./vendor/Glad/bin-int ./vendor/GLFW/bin-int ./vendor/ImGui/bin-int 

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error: "$ERRORLEVEL && exit
fi
