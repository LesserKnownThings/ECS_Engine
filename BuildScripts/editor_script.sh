#!/bin/bash
if [ ! -d Editor_Build/ ]
	then
		mkdir Editor_Build	
fi

cp -r ECS_Engine/Data Editor_Build/ 

cd Editor_Build/
cmake -DCMAKE_BUILD_TYPE=Editor ..
make -j $(nproc)
