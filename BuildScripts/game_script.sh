#!/bin/bash
if [ ! -d Game_Build/ ]
	then
		mkdir Game_Build	
fi

cp -r ECS_Engine/Data Game_Build/ 

cd Game_Build/
cmake -DCMAKE_BUILD_TYPE=Game ..
make -j $(nproc)
