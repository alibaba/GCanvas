#!/usr/bin/env bash

echo "start copy android publish build scripts"
mv ./android/build.gradle ../local.gradle
mv ../rn-publish.gradle ./android/build.gradle

echo "start npm publish"

npm publish

echo "finish npm publish. reverse android build scripts"

mv ./android/build.gradle ../rn-publish.gradle
mv ../local.gradle ./android/build.gradle
