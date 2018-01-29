/*
 * python_interface.cpp
 *
 *  Created on: 2017-12-26
 *      Author: root
 */
#include <iostream>
#include<boost/python.hpp>
#include <unistd.h>
#include"forwardmodal.h"
using namespace std;
using namespace boost::python;
class MapSize{
public:
	 int  minX;
	 int  maxX;
	 int  minY;
	 int  maxY;
};
bool createRetFile( unsigned char statusCode,string finalMapFileName,miniMap *p_minimap = NULL)
{
	FILE *fp = NULL;
	fp = fopen(finalMapFileName.c_str(), "w");//wb
	if(fp == NULL)
	{
		printf("createRetFile:1 open file error!");
		return false;
	}
	if( 0 == statusCode) //处理成功
	{
		 // 封装处理后的文件,以便下传
			unsigned int minX,minY,maxX,maxY;
			fwrite((char*)&statusCode,sizeof(unsigned char),1,fp);
			p_minimap->getMapSize(minX,minY,maxX,maxY);
			fwrite(&minX,sizeof(unsigned int),1,fp);
			fwrite(&minY,sizeof(unsigned int),1,fp);
			fwrite(&maxX,sizeof(unsigned int),1,fp);
			fwrite(&maxY,sizeof(unsigned int),1,fp);
			printf("p_minimap size (%x,%x)-(%x,%x)\n",minX,minY,maxX,maxY);
			char binaryValue = 0;
			for(unsigned int x =minX;x<=maxX;x++ )
			{
				for(unsigned int y =minY;y<=maxY;y++ )
				{
					binaryValue = p_minimap->getval(x,y);
					fwrite(&binaryValue,sizeof(char),1,fp);
				}
			}
	}
	else //处理失败
	{
		fwrite((char*)&statusCode,sizeof(unsigned char),1,fp);
	}
	fclose(fp);
	fp = NULL;
	return true;
}

//
bool mapOptimizeOffline(string originFilePath,string finalMapFileName,
													unsigned int u32_min_x,
													unsigned int  u32_min_y,
													unsigned int  u32_max_x,
													unsigned int  u32_max_y)
{
	 printf("mapOptimizeOffline:%d, %d, %d, %d\n",u32_min_x,u32_min_y,u32_max_x,u32_max_y);
	 //todo:1 判断文件是否存在,解析文件获取地图边界
	 if(0 != access(originFilePath.c_str(), F_OK))
	 {
		 printf("mapOptimizeOffline: originFilePath is not exist!");
		 return createRetFile(1,finalMapFileName);
	 }
	 printf("AAAAAAAAAAAAAAAAAAAAAAAAAAA :originFilePath = %s\n",originFilePath.c_str());
	 miniMap *p_minimap = new miniMap(u32_min_x, u32_max_x, u32_min_y, u32_max_y, 7);
	 if(NULL == p_minimap)
	 {
		 printf("mapOptimizeOffline: new miniMap error!");
		 return createRetFile(2,finalMapFileName);
	 }
	 MLS_Mapping mlsMapping;
	 bool ret = mlsMapping.MLS_Mapping_Process(p_minimap,originFilePath);
	 if(!ret)
	 {
		 printf("MLS_Mapping_Process failed\n");
		 delete(p_minimap);
		 p_minimap = NULL;
		 return createRetFile(3,finalMapFileName);
	 }
	 else
	 {
		 printf("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB finalMapFileName = %s\n",finalMapFileName.c_str());
		 ret =  createRetFile(0,finalMapFileName,p_minimap);
		 delete(p_minimap);
		 p_minimap = NULL;
		 return ret;
	 }
}

BOOST_PYTHON_MODULE(mapOptimize)
{
    def("mapOptimizeOffline", mapOptimizeOffline);
}


