#!/bin/sh

pid=`netstat -anpt | grep LIST | grep -w $1 | awk '{print $7}' | awk -F / '{print $1}'`
kill -s 6 ${pid}


