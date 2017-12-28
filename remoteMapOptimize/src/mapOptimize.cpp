/*
 * python_interface.cpp
 *
 *  Created on: 2017-12-26
 *      Author: root
 */
#include <iostream>
#include<boost/python.hpp>
#include"forwardmodal.h"
using namespace std;
using namespace boost::python;
bool mapOptimizeOffline(string originFilePath,string finalMapFileName)
{
	 unsigned int u32_min_x = 5;
	 unsigned int  u32_min_y = 5;
	 unsigned int  u32_max_x = 795;
	 unsigned int  u32_max_y = 795;
	 //todo:1 判断文件是否存在,解析文件获取地图边界
	 if(false)
	 {
		 return false;
	 }
	 printf("AAAAAAAAAAAAAAAAAAAAAAAAAAA :originFilePath = %s\n",originFilePath.c_str());
	 miniMap *p_minimap = new miniMap(u32_min_x, u32_max_x, u32_min_y, u32_max_y, 7);
	 MLS_Mapping mlsMapping;
//	 string originFilePath = "upload/ultrasonic_sla.txt";
	 bool ret = mlsMapping.MLS_Mapping_Process(p_minimap,originFilePath);
	 if(!ret)
	 {
		 printf("MLS_Mapping_Process failed\n");
		 return false;
	 }
	 else
	 {
		 //todo:1 封装处理后的文件,以便下传
			FILE *fp;
			fp = fopen(finalMapFileName.c_str(), "wb");
			if(fp == NULL)
			{
				return false;
			}
			unsigned int minX,minY,maxX,maxY;
			p_minimap->getMapSize(minX,maxX,minY,maxY);
			fwrite(&minX,sizeof(unsigned int),1,fp);
			fwrite(&minY,sizeof(unsigned int),1,fp);
			fwrite(&maxX,sizeof(unsigned int),1,fp);
			fwrite(&maxY,sizeof(unsigned int),1,fp);
			char binaryValue = 0;
			for(unsigned int x =minX;x<=maxX;x++ )
			{
				for(unsigned int y =minY;y<=maxY;y++ )
				{
					binaryValue = p_minimap->getval(x,y);
					fwrite(&binaryValue,sizeof(char),1,fp);
				}
			}
			fclose(fp);
			fp = NULL;
	 }
	 if(false)
	 {
		 return false;
	 }
	 printf("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB finalMapFileName = %s\n",finalMapFileName.c_str());
	 return true;
}

BOOST_PYTHON_MODULE(mapOptimize)
{
    def("mapOptimizeOffline", mapOptimizeOffline);
}


