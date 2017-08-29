#!/bin/sh
cd "$(dirname "$0")"

if [ -z "${CLANG_FORMAT_CMD}" ]
then
    if [ -x "$(which clang-format-4.0 2> /dev/null)" ]
    then
        CLANG_FORMAT_CMD=clang-format-4.0
    else
        CLANG_FORMAT_CMD=clang-format
    fi
fi

list=`git diff --name-only "HEAD" | grep '\.cpp$\|\.h$'`

for i in ${list}
do
  if [ -f "${i}" ] 
  then 
	  "${CLANG_FORMAT_CMD}" -i -style=file "${i}" 
  fi 
done
