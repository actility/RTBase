#!/bin/bash

LIC=$ROOTACT/rtbase/LICENSE

DoJob()
{
    while read file
	do
        cat ${LIC} > /tmp/my_temp
        cat $file >> /tmp/my_temp

        echo $file
        cat /tmp/my_temp > $file
    done
}



find . -type f -name '*.[ch]' -print | DoJob
