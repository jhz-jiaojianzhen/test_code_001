#ifndef __FORWARDMODEL____H
#define __FORWARDMODEL____H
#include<string>
#include <vector>
#include <algorithm>
#include "geometry.h"
using namespace std;
/*basic factor*/
#define Pi  3.14159265
#define MAX_R 2900
#define BEAM_WIDTH 40
#define GRID_DEF 120.0
//algorithm parametres
#define DELTA 100.0
#define BETA /*0.7**/70

/*函数，取最小值*/
template<class T>
T Min(T a, T b)
{
	return a<b?a:b;
}
template<class T>
T Min(T a, T b, T c, T d)
{
	return Min(Min(a,b), Min(c, d));
}
template<class T>
T Max(T a, T b)
{
	return a>=b?a:b;
}
template<class T>
T Max(T a, T b, T c, T d)
{
	return Max(Max(a,b), Max(c, d));
}

//an index for a sonar measurement
typedef struct{
	unsigned short t;
	char sonarID;
}UNIT_SONAR_INDEX;
/*a map grid with relevant measurements*/
typedef struct{
	char binaryValue;
	std::vector<UNIT_SONAR_INDEX> *p_vec_relevant_sonar;
}MAP_GRID;

class miniMap{
public:
	unsigned short xmin;
	unsigned short xmax;
	unsigned short ymin;
	unsigned short ymax;
	unsigned short lenX;
	unsigned short lenY;
	MAP_GRID *data;
	//Constructor
	miniMap()
	{
		data = NULL;
	}
	miniMap(unsigned short gxmin, unsigned short gxmax, unsigned short gymin, unsigned short gymax, char binaryValue)
	{
		data = NULL;
		init(gxmin, gxmax, gymin, gymax, binaryValue);
	}
	//Destructor
	~miniMap()
	{
		if(NULL != data)
		{
			for(int i=0; i<lenX*lenY; i++)
			{
				if(NULL != data[i].p_vec_relevant_sonar)
				{
					delete data[i].p_vec_relevant_sonar;
					data[i].p_vec_relevant_sonar = NULL;
				}
			}
			delete[] data;
			data = NULL;
		}
	}
	void delete_relevant_sonar()
	{
		for(int i=0; i<lenX*lenY; i++)
		{
			if(NULL != data[i].p_vec_relevant_sonar)
			{
				delete data[i].p_vec_relevant_sonar;
				data[i].p_vec_relevant_sonar = NULL;
			}
		}
	}
	void init(unsigned short gxmin, unsigned short gxmax, unsigned short gymin, unsigned short gymax, char binaryValue)
	{
		xmin = gxmin;
		xmax = gxmax;
		ymin = gymin;
		ymax = gymax;
		lenX = gxmax - gxmin + 1;
		lenY = gymax - gymin + 1;

		if(NULL != data)
		{
			delete[] data;
			data = NULL;
		}
		data = new MAP_GRID[lenX*lenY];
		for(int i=0; i<lenX*lenY; i++){
			data[i].binaryValue = binaryValue;
			data[i].p_vec_relevant_sonar = NULL;
		}
	}
	char getval(unsigned short x, unsigned short y) const
	{
		return data[(x - xmin)*lenY + (y - ymin)].binaryValue;
	}
	void setval(unsigned short x, unsigned short y, char binaryValue)
	{
		data[(x - xmin)*lenY + (y - ymin)].binaryValue = binaryValue;
	}
	void construct_relevant_z_vec(unsigned short x, unsigned short y)
	{
		data[(x - xmin)*lenY + (y - ymin)].p_vec_relevant_sonar = new std::vector<UNIT_SONAR_INDEX>;
	}
	void add_relevant_z_vec(unsigned short x, unsigned short y, UNIT_SONAR_INDEX new_sonar_index)
	{
		if(NULL == data[(x - xmin)*lenY + (y - ymin)].p_vec_relevant_sonar)
		{
			construct_relevant_z_vec(x, y);
		}
		data[(x - xmin)*lenY + (y - ymin)].p_vec_relevant_sonar->push_back(new_sonar_index);
	}
	std::vector<UNIT_SONAR_INDEX>* get_relevant_z_vec(unsigned short x, unsigned short y)
	{
		return data[(x - xmin)*lenY + (y - ymin)].p_vec_relevant_sonar;
	}
	void getMapSize(unsigned int &minX,unsigned int &minY,  unsigned int &maxX, unsigned int &maxY)
	{
		minX = xmin;
		minY = ymin;
		maxX = xmax;
		maxY = ymax;
	}
};

/*--------------------------------------measurement data construction----------------------------------------*/
//unit grid
typedef struct{
	unsigned short x;
	unsigned short y;
}UNIT_GRID;
//sonar cone
typedef struct{
	Circle C;
	float th;
	char correctFlag;  //0-incorrect;   1-correct
}sonarCone;
//a sonar record: cone & relevant_grids
typedef struct{
	sonarCone sonarcone;
	std::vector<UNIT_GRID> *p_vec_relevant_grid;
}SONAR_RECORD;
//coor - odometry
typedef struct{
	float x;
	float y;
	float th;
}UNIT_COOR;

//all sonar measurement information
class AllSonarZ
{
public:
	unsigned short timeTotal;
	unsigned short sonarNum;
	unsigned short *sonarID;
	SONAR_RECORD *sonarZ;
	//Constructor
	AllSonarZ()
	{
		sonarID = NULL;
		sonarZ = NULL;
	}
	AllSonarZ(vector<float> &x_offline, vector<float> &y_offline, vector<float> &th_offline, vector<unsigned int> &sonar2_offline, vector<unsigned int> &sonar3_offline,
			vector<unsigned int> &sonar7_offline, vector<unsigned int> &sonar8_offline, vector<unsigned int> &sonar9_offline, vector<unsigned int> &sonar10_offline,
			vector<unsigned int> &sonar11_offline, vector<unsigned int> &sonar12_offline, vector<unsigned int> &sonar13_offline)
	{
		UNIT_COOR coor_loc_temp;
		unsigned short time_temp;
		unsigned short sonar_temp;
		unsigned int range_temp=0;
		unsigned short ID_temp;

		sonarID = NULL;
		sonarZ = NULL;
		timeTotal = x_offline.size();
		sonarNum = 9;

		sonarID = new unsigned short[sonarNum];
		sonarID[0] = 1; sonarID[1] = 2;  sonarID[2] = 6; sonarID[3] = 7;
		sonarID[4] = 8;	sonarID[5] = 9; sonarID[6] = 10;  sonarID[7] = 11; sonarID[8] = 12;

		if(NULL != sonarZ)
		{
			delete[] sonarZ;
			sonarZ = NULL;
		}
		sonarZ = new SONAR_RECORD[timeTotal*sonarNum];
		for(int i=0; i<timeTotal*sonarNum; i++)
		{
			sonarZ[i].p_vec_relevant_grid = NULL;
			time_temp = i/sonarNum;
			sonar_temp = i%sonarNum;
			coor_loc_temp.x = x_offline[time_temp];
			coor_loc_temp.y = y_offline[time_temp];
			coor_loc_temp.th = th_offline[time_temp];
			ID_temp =  sonarID[sonar_temp];
			switch(sonar_temp)
			{
			case 0:
				range_temp = sonar2_offline[time_temp];
				break;
			case 1:
				range_temp = sonar3_offline[time_temp];
				break;
			case 2:
				range_temp = sonar7_offline[time_temp];
				break;
			case 3:
				range_temp = sonar8_offline[time_temp];
				break;
			case 4:
				range_temp = sonar9_offline[time_temp];
				break;
			case 5:
				range_temp = sonar10_offline[time_temp];
				break;
			case 6:
				range_temp = sonar11_offline[time_temp];
				break;
			case 7:
				range_temp = sonar12_offline[time_temp];
				break;
			case 8:
				range_temp = sonar13_offline[time_temp];
				break;
			default:
				break;
			}
			CreateSonarCone(sonarZ[i].sonarcone, coor_loc_temp, range_temp, ID_temp);
			sonarZ[i].sonarcone.correctFlag = 0;
		}
	}
	~AllSonarZ()
	{
		if(NULL != sonarZ)
		{
			for(int i=0; i<timeTotal*sonarNum; i++)
			{
				if(sonarZ[i].p_vec_relevant_grid != NULL)
				{
					delete sonarZ[i].p_vec_relevant_grid;
					sonarZ[i].p_vec_relevant_grid = NULL;
				}
			}
			delete[] sonarID;
			sonarID = NULL;
			delete[] sonarZ;
			sonarZ = NULL;
		}
	}
	void CreateSonarCone(sonarCone &sonar_cone, UNIT_COOR coor_location, unsigned int sonarRange, unsigned short ID);
	sonarCone& getval(unsigned short time, unsigned short sonar)
	{
		return sonarZ[time*sonarNum + sonar].sonarcone;
	}
	void construct_relevant_grid_vec(unsigned short time, unsigned short sonar)
	{
		sonarZ[time*sonarNum + sonar].p_vec_relevant_grid = new std::vector<UNIT_GRID>;
	}
	void add_relevant_grid_vec(unsigned short time, unsigned short sonar, UNIT_GRID new_grid_index)
	{
		if(sonarZ[time*sonarNum + sonar].p_vec_relevant_grid == NULL)
		{
			construct_relevant_grid_vec(time, sonar);
		}
		sonarZ[time*sonarNum + sonar].p_vec_relevant_grid->push_back(new_grid_index);
	}
	std::vector<UNIT_GRID>* get_relevant_grid_vec(unsigned short time, unsigned short sonar)
	{
		return sonarZ[time*sonarNum + sonar].p_vec_relevant_grid;
	}

};
typedef struct{
	double mSP;
	char binaryValue;
}mSP_GRID;

class mSP_Map{
public:
	unsigned short xmin;
	unsigned short xmax;
	unsigned short ymin;
	unsigned short ymax;
	unsigned short lenX;
	unsigned short lenY;
	mSP_GRID *data;

	//Constructor
	mSP_Map()
	{
		data = NULL;
	}
	mSP_Map(unsigned short gxmin, unsigned short gxmax, unsigned short gymin, unsigned short gymax, char binaryValue)
	{
		data = NULL;
		init(gxmin, gxmax, gymin, gymax, binaryValue);
	}
	~mSP_Map()
	{
		if(data != NULL){
			delete[] data;
			data = NULL;
		}
	}
	void init(unsigned short gxmin, unsigned short gxmax, unsigned short gymin, unsigned short gymax, char binaryValue)
	{
		xmin = gxmin;
		xmax = gxmax;
		ymin = gymin;
		ymax = gymax;
		lenX = gxmax - gxmin + 1;
		lenY = gymax - gymin + 1;

		if(NULL != data)
		{
			delete[] data;
			data = NULL;
		}
		data = new mSP_GRID[lenX*lenY];
		for(int i=0; i<lenX*lenY; i++){
			data[i].binaryValue = binaryValue;
			data[i].mSP = -1000.0;
		}
	}
	char getval(unsigned short x, unsigned short y) const
	{
		return data[(x - xmin)*lenY + (y - ymin)].binaryValue;
	}
	void setval(unsigned short x, unsigned short y, char binaryValue)
	{
		data[(x - xmin)*lenY + (y - ymin)].binaryValue = binaryValue;
	}
	double getmSP(unsigned short x, unsigned short y) const
	{
		return data[(x - xmin)*lenY + (y - ymin)].mSP;
	}
	void setmSP(unsigned short x, unsigned short y, double mSP)
	{
		data[(x - xmin)*lenY + (y - ymin)].mSP = mSP;
	}

};
class MLS_Mapping
{
private:
	string m_originFilePath;
public:
	MLS_Mapping():m_originFilePath(""){}
	void setOriginFilePath(char * strOriginFilePath)
	{
		m_originFilePath = string(strOriginFilePath);
	}
	void setOriginFilePath(string strOriginFilePath)
	{
		m_originFilePath = strOriginFilePath;
	}
	bool MLS_Mapping_Process(miniMap* p_minimap,string originFilePath);
private:
	int getDataFromFile(vector<float> &x, vector<float> &y, vector<float>&th, vector<unsigned int> &sonar2, vector<unsigned int> &sonar3, vector<unsigned int> &sonar7,
			vector<unsigned int> &sonar8, vector<unsigned int> &sonar9, vector<unsigned int> &sonar10, vector<unsigned int> &sonar11, vector<unsigned int> &sonar12,
			vector<unsigned int> &sonar13);
	//Construct relevant relationship!
	void Construct_Relationship(miniMap &minimap, AllSonarZ &allsonarz);
	int calc_sound_pressure(double &sound_pressure, UNIT_GRID grid, sonarCone sonarcone);

	int all_measure_classify(mSP_Map *p_mSPmap, AllSonarZ *p_allsonarz);
	int map_by_correct_measure(miniMap *p_minimap, AllSonarZ *p_allsonarz);
	int mSP_map_construct(int heightStatus, AllSonarZ *p_allsonarz, mSP_Map *p_mSPmap_0, mSP_Map *p_mSPmap_1);
	void merge_mSP_map(mSP_Map *p_mSPmap, mSP_Map *p_mSPmap_0, mSP_Map *p_mSPmap_1, vector<float> &x_offline, vector<float> &y_offline);
	void measure_status_desicion(sonarCone &sonarcone, mSP_Map *p_mSPmap, AllSonarZ *p_allsonarz, unsigned short time, unsigned short sonar);
};



#endif
