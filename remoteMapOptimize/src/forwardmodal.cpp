#include <cmath>
#include<vector>
#include <stdio.h>
#include "geometry.h"
#include "forwardmodal.h"
using namespace std;

int MLS_Mapping::getDataFromFile(vector<float> &x, vector<float> &y, vector<float>&th, vector<unsigned int> &sonar2, vector<unsigned int> &sonar3, vector<unsigned int> &sonar7,
		vector<unsigned int> &sonar8, vector<unsigned int> &sonar9, vector<unsigned int> &sonar10, vector<unsigned int> &sonar11, vector<unsigned int> &sonar12,
		vector<unsigned int> &sonar13)
{
	int counter = 0;
	FILE *fp;
	fp = fopen(m_originFilePath.c_str(), "r");
	if(fp == NULL)return -1;
	float data;
	while(fscanf(fp, "%f%*[^-^.^0-9]", &data) == 0)
	{
		fseek(fp, 1, SEEK_CUR);
	}
	do
	{
		switch(counter%12)
		{
		case 0:
			x.push_back(data);
			break;
		case 1:
			y.push_back(data);
			break;
		case 2:
			th.push_back(data);
			break;
		case 3:
			sonar2.push_back((unsigned int)data);
			break;
		case 4:
			sonar3.push_back((unsigned int)data);
			break;
		case 5:
			sonar7.push_back((unsigned int)data);
			break;
		case 6:
			sonar8.push_back((unsigned int)data);
			break;
		case 7:
			sonar9.push_back((unsigned int)data);
			break;
		case 8:
			sonar10.push_back((unsigned int)data);
			break;
		case 9:
			sonar11.push_back((unsigned int)data);
			break;
		case 10:
			sonar12.push_back((unsigned int)data);
			break;
		case 11:
			sonar13.push_back((unsigned int)data);
			break;
		default:
			break;
		}
		counter++;
	}
	while(fscanf(fp, "%f%*[^-^.^0-9]", &data) > 0);
	fclose(fp);
	return 0;
}


/*函数：机器人坐标系下的xy转为全局坐标*/
Point ToGlobalPoint(float co_robot_x, float co_robot_y, float Global_x, float Global_y, float th)
{
	Point co_global;
	double radian = th*Pi/180.0;
	co_global.x = co_robot_x*cos(radian) - co_robot_y*sin(radian) + Global_x;
	co_global.y = co_robot_x*sin(radian) + co_robot_y*cos(radian) + Global_y;
	return co_global;
}

/*函数：结合里程计超声波创建扇形*/
void AllSonarZ::CreateSonarCone(sonarCone &sonar_cone, UNIT_COOR coor_location, unsigned int sonarRange, unsigned short ID)
{
	float thtemp;
	if(ID>=0 && ID<8)
	{
		sonar_cone.th = coor_location.th - 45*ID;
		if(sonar_cone.th > 180.0)
			sonar_cone.th -= 360.0;
		if(sonar_cone.th <= -180.0)
			sonar_cone.th += 360.0;
		if(sonarRange >= MAX_R/10)
			sonar_cone.C.radius = MAX_R;
		else
			sonar_cone.C.radius = sonarRange*10.0;
		thtemp = (8.0-ID)*45.0*Pi/180.0;
		sonar_cone.C.centre = ToGlobalPoint(214.0*cos(thtemp), 214.0*sin(thtemp), coor_location.x, coor_location.y, coor_location.th);
	}
	else if(ID < 11)
	{
		sonar_cone.th = coor_location.th + 30.0*(9-ID);
		if(sonar_cone.th > 180.0)
			sonar_cone.th -= 360.0;
		if(sonar_cone.th <= -180.0)
			sonar_cone.th += 360.0;
		if(sonarRange >= MAX_R/10)
			sonar_cone.C.radius = MAX_R;
		else
			sonar_cone.C.radius = sonarRange*10.0;
		thtemp = (9-ID)*30.0*Pi/180.0;
		sonar_cone.C.centre = ToGlobalPoint(260.0*cos(thtemp), 260.0*sin(thtemp), coor_location.x, coor_location.y, coor_location.th);
	}
	else if(ID == 11)
	{
		sonar_cone.th = coor_location.th + 90.0;
		if(sonar_cone.th > 180.0)
			sonar_cone.th -= 360.0;
		if(sonar_cone.th <= -180.0)
			sonar_cone.th += 360.0;
		if(sonarRange >= MAX_R/10)
			sonar_cone.C.radius = MAX_R;
		else
			sonar_cone.C.radius = sonarRange*10.0;
		thtemp = 90.0;
		sonar_cone.C.centre = ToGlobalPoint(115, 230, coor_location.x, coor_location.y, coor_location.th);
	}
	else if(ID == 12)
	{
		sonar_cone.th = coor_location.th - 90.0;
		if(sonar_cone.th > 180.0)
			sonar_cone.th -= 360.0;
		if(sonar_cone.th <= -180.0)
			sonar_cone.th += 360.0;
		if(sonarRange >= MAX_R/10)
			sonar_cone.C.radius = MAX_R;
		else
			sonar_cone.C.radius = sonarRange*10.0;
		thtemp = -90.0;
		sonar_cone.C.centre = ToGlobalPoint(115, -230, coor_location.x, coor_location.y, coor_location.th);
	}
}

/*Construct relevant relationship_map&sonar*/
void MLS_Mapping::Construct_Relationship(miniMap &minimap, AllSonarZ &allsonarz)
{
	float disTemp;
	Point pointTemp;
	float thTemp;
	UNIT_SONAR_INDEX new_sonar_index;
	UNIT_GRID new_grid;
	float radian, radian_l, radian_r;
	Point O, A, B, C;
	float fxmax, fxmin, fymax, fymin;
	unsigned short uxmax, uxmin, uymax, uymin;
	for(unsigned short t=0; t<allsonarz.timeTotal; t++){
		for(unsigned short s=0; s<allsonarz.sonarNum; s++){
			radian = allsonarz.getval(t, s).th*Pi/180.0;
			radian_l = radian + BEAM_WIDTH*0.5*Pi/180.0;
			radian_r = radian - BEAM_WIDTH*0.5*Pi/180.0;
			O = allsonarz.getval(t, s).C.centre;
			A.x = O.x + allsonarz.getval(t, s).C.radius*cos(radian_l);
			A.y = O.y + allsonarz.getval(t, s).C.radius*sin(radian_l);
			B.x = O.x + allsonarz.getval(t, s).C.radius*cos(radian);
			B.y = O.y + allsonarz.getval(t, s).C.radius*sin(radian);
			C.x = O.x + allsonarz.getval(t, s).C.radius*cos(radian_r);
			C.y = O.y + allsonarz.getval(t, s).C.radius*sin(radian_r);

			fxmax = Max(O.x, A.x, B.x, C.x);
			fxmin = Min(O.x, A.x, B.x, C.x);
			fymax = Max(O.y, A.y, B.y, C.y);
			fymin = Min(O.y, A.y, B.y, C.y);

			uxmax = (fxmax + GRID_DEF*0.5)/GRID_DEF;
			uxmin = (fxmin + GRID_DEF*0.5)/GRID_DEF;
			uymax = (fymax + GRID_DEF*0.5)/GRID_DEF;
			uymin = (fymin + GRID_DEF*0.5)/GRID_DEF;

			uxmax = Min((unsigned short)(uxmax+3), minimap.xmax);
			uxmin = Max((unsigned short)(uxmin-3), minimap.xmin);
			uymax = Min((unsigned short)(uymax+3), minimap.ymax);
			uymin = Max((unsigned short)(uymin-3), minimap.ymin);

			for(unsigned short x=uxmin; x<=uxmax; x++){
				for(unsigned short y=uymin; y<=uymax; y++){
					pointTemp.x = x*GRID_DEF;
					pointTemp.y = y*GRID_DEF;

					thTemp = atan2(pointTemp.y - allsonarz.getval(t, s).C.centre.y, pointTemp.x - allsonarz.getval(t, s).C.centre.x)*180/Pi;
					thTemp = thTemp - allsonarz.getval(t, s).th;
					if(thTemp>180.0)
						thTemp -= 360.0;
					if(thTemp<=-180.0)
						thTemp += 360.0;
					if(fabs(thTemp)>BEAM_WIDTH/2)
						continue;

					disTemp = DistanceOfPoints(pointTemp, allsonarz.getval(t, s).C.centre);
					if(disTemp>allsonarz.getval(t, s).C.radius + DELTA + GRID_DEF)
						continue;
					//if(allsonarz.getval(t, s).C.radius >= 2900.0)
					//	continue;

					//Corelation
					new_sonar_index.sonarID = s; new_sonar_index.t = t;
					new_grid.x = x; new_grid.y = y;
					minimap.add_relevant_z_vec(x, y, new_sonar_index);
					allsonarz.add_relevant_grid_vec(t, s, new_grid);
				}
			}
		}
	}
}

/*calc_sound_pressure*/
int MLS_Mapping::calc_sound_pressure(double &sound_pressure, UNIT_GRID grid, sonarCone sonarcone)
{
	Point grid_to_coor;
	grid_to_coor.x = GRID_DEF*grid.x;
	grid_to_coor.y = GRID_DEF*grid.y;
	double r = DistanceOfPoints(grid_to_coor, sonarcone.C.centre);

	double th = atan2(grid_to_coor.y - sonarcone.C.centre.y, grid_to_coor.x - sonarcone.C.centre.x)*180.0/Pi - sonarcone.th;
	if(th > 180.0)
		th -= 360.0;
	if(th <= -180.0)
		th += 360.0;
	th = fabs(th);
	if(th > BEAM_WIDTH/2)
		return -1;
	if(r > MAX_R+100.0)
		return -1;

	float dr = r - sonarcone.C.radius;
	if(dr > BETA)
		return -1;
	else if(dr >= -BETA && dr <= BETA){
		//盲区重点处理
		if(sonarcone.C.radius<=400){
			r = r*0.5;
		}
		//change
		if(sonarcone.C.radius == MAX_R){
			sound_pressure = -0.00102*th*th + 0.00241*th - log10(r);
			return 0;
		}
		//change end
		sound_pressure = -0.00102*th*th + 0.00241*th - log10(r);
		return 1;
	}
	else{
		sound_pressure = -0.00102*th*th + 0.00241*th - log10(r);
		return 0;
	}
	//0-empty; 1-occupied; -1-error
}

//mSP map with height: 分层声压 0-下层 1-上层
int MLS_Mapping::mSP_map_construct(int heightStatus, AllSonarZ *p_allsonarz, mSP_Map *p_mSPmap_0, mSP_Map *p_mSPmap_1)
{
	std::vector<UNIT_GRID> *p_vec_relevant_grid;
	UNIT_GRID grid_now;
	double sound_pressure = 0.0;
	if(heightStatus == 0)
	{
		for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
			for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
				if(p_allsonarz->sonarID[s] == 2-1 || p_allsonarz->sonarID[s] == 3-1 || p_allsonarz->sonarID[s] == 7-1 || p_allsonarz->sonarID[s] == 8-1)
					continue;
				p_vec_relevant_grid = p_allsonarz->sonarZ[t*p_allsonarz->sonarNum + s].p_vec_relevant_grid;
				if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0)
					continue;

				for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
					grid_now = p_vec_relevant_grid->at(i);
					int ret = calc_sound_pressure(sound_pressure, grid_now, p_allsonarz->getval(t, s));
					if(ret != -1){
						double deltaSP = sound_pressure - p_mSPmap_0->getmSP(grid_now.x, grid_now.y);
						if(deltaSP > 0){
							p_mSPmap_0->setmSP(grid_now.x, grid_now.y, sound_pressure);
							p_mSPmap_0->setval(grid_now.x, grid_now.y, (char)ret);
						}
						else if(deltaSP == 0.0){
							p_mSPmap_0->setmSP(grid_now.x, grid_now.y, sound_pressure);
							if(ret == 1)
								p_mSPmap_0->setval(grid_now.x, grid_now.y, 1);
						}
					}
				}
			}
		}
	}
	if(heightStatus == 1)
	{
		for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
			for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
				if(p_allsonarz->sonarID[s] == 9-1 || p_allsonarz->sonarID[s] == 10-1 || p_allsonarz->sonarID[s] == 11-1 || p_allsonarz->sonarID[s] == 12-1 || p_allsonarz->sonarID[s] == 13-1)
					continue;
				p_vec_relevant_grid = p_allsonarz->sonarZ[t*p_allsonarz->sonarNum + s].p_vec_relevant_grid;
				if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0)
					continue;
				for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
					grid_now = p_vec_relevant_grid->at(i);
					int ret = calc_sound_pressure(sound_pressure, grid_now, p_allsonarz->getval(t, s));
					if(ret != -1){
						double deltaSP = sound_pressure - p_mSPmap_1->getmSP(grid_now.x, grid_now.y);
						if(deltaSP > 0){
							p_mSPmap_1->setmSP(grid_now.x, grid_now.y, sound_pressure);
							p_mSPmap_1->setval(grid_now.x, grid_now.y, (char)ret);
						}
						else if(deltaSP == 0.0){
							p_mSPmap_1->setmSP(grid_now.x, grid_now.y, sound_pressure);
							if(ret == 1)
								p_mSPmap_1->setval(grid_now.x, grid_now.y, 1);
						}
					}
				}
			}
		}
	}
	return 0;
}
/*mSP_map_merge by multi height*/
void MLS_Mapping::merge_mSP_map(mSP_Map *p_mSPmap, mSP_Map *p_mSPmap_0, mSP_Map *p_mSPmap_1, vector<float> &x_offline, vector<float> &y_offline)
{
	unsigned short x, y;
	for(y=p_mSPmap->ymin; y<p_mSPmap->ymax; y++){
		for(x=p_mSPmap->xmin; x<p_mSPmap->xmax; x++){
			if(p_mSPmap_0->getval(x, y) == 1 || p_mSPmap_1->getval(x, y) == 1)
				p_mSPmap->setval(x, y, 1);
		}
	}
	//delete road
	int gridX, gridY;
	for(int i=0; i<(int)x_offline.size(); i++){
		gridX = (x_offline[i] + GRID_DEF/2.0)/GRID_DEF;
		gridY = (y_offline[i] + GRID_DEF/2.0)/GRID_DEF;
		for(int x=gridX-2; x<=gridX+2; x++){
			for(int y=gridY-2; y<=gridY+2; y++){
				if(x>=p_mSPmap->xmin&&x<=p_mSPmap->xmax  && y>=p_mSPmap->ymin&&y<=p_mSPmap->ymax)
					p_mSPmap->setval(x, y, 0);
					//p_minimap->setval(x, y, 0);
			}
		}
	}
}

/*the classify of measurements: 1-correct, 0-incorrect*/
void MLS_Mapping::measure_status_desicion(sonarCone &sonarcone, mSP_Map *p_mSPmap, AllSonarZ *p_allsonarz, unsigned short time, unsigned short sonar)
{
	std::vector<UNIT_GRID> *p_vec_relevant_grid;
	UNIT_GRID grid_now;
	int hitFlag = 0;
	p_vec_relevant_grid = p_allsonarz->sonarZ[time*p_allsonarz->sonarNum + sonar].p_vec_relevant_grid;
	if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0){
		sonarcone.correctFlag = 0;
		return;
	}
	for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
		grid_now = p_vec_relevant_grid->at(i);
		Point grid_to_coor;
		grid_to_coor.x = GRID_DEF*grid_now.x;
		grid_to_coor.y = GRID_DEF*grid_now.y;
		double r = DistanceOfPoints(grid_to_coor, sonarcone.C.centre);
		if(sonarcone.C.radius == MAX_R){
			if(r < MAX_R && p_mSPmap->getval(grid_now.x, grid_now.y) == 1){
				sonarcone.correctFlag = 0;
				return;
			}
		}
		else if(r - sonarcone.C.radius < -BETA){
			if(p_mSPmap->getval(grid_now.x, grid_now.y) == 1){
				sonarcone.correctFlag = 0;
				return;
			}
		}
		else if(r - sonarcone.C.radius > -BETA && r - sonarcone.C.radius < BETA){
			if(p_mSPmap->getval(grid_now.x, grid_now.y) == 1){
				hitFlag = 1;
			}
		}
	}
	if(hitFlag == 1)
		sonarcone.correctFlag = 1;
	else if(sonarcone.C.radius == MAX_R)
		sonarcone.correctFlag = 1;
	else
		sonarcone.correctFlag = 0;
	return;
}
/*all measure classify*/
int MLS_Mapping::all_measure_classify(mSP_Map *p_mSPmap, AllSonarZ *p_allsonarz)
{
	for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
		for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
			measure_status_desicion(p_allsonarz->getval(t, s), p_mSPmap, p_allsonarz, t, s);
		}
	}
	return 0;
}
/*map by correct measure*/
int MLS_Mapping::map_by_correct_measure(miniMap *p_minimap, AllSonarZ *p_allsonarz)
{
	std::vector<UNIT_GRID> *p_vec_relevant_grid;
	UNIT_GRID grid_now;
	for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
		for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
			if(p_allsonarz->getval(t, s).correctFlag != 1)
				continue;
			p_vec_relevant_grid = p_allsonarz->sonarZ[t*p_allsonarz->sonarNum + s].p_vec_relevant_grid;
			if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0)
				continue;
			for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
				grid_now = p_vec_relevant_grid->at(i);
				Point grid_to_coor;
				grid_to_coor.x = GRID_DEF*grid_now.x;
				grid_to_coor.y = GRID_DEF*grid_now.y;
				double r = DistanceOfPoints(grid_to_coor, p_allsonarz->getval(t, s).C.centre);
				if(fabs(r - p_allsonarz->getval(t, s).C.radius) <= BETA && p_allsonarz->getval(t, s).C.radius < MAX_R){
					p_minimap->setval(grid_now.x, grid_now.y, 1);
				}
			}
		}
	}
	for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
		for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
			if(p_allsonarz->getval(t, s).correctFlag != 1)
				continue;
			p_vec_relevant_grid = p_allsonarz->sonarZ[t*p_allsonarz->sonarNum + s].p_vec_relevant_grid;
			if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0)
				continue;
			for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
				grid_now = p_vec_relevant_grid->at(i);
				Point grid_to_coor;
				grid_to_coor.x = GRID_DEF*grid_now.x;
				grid_to_coor.y = GRID_DEF*grid_now.y;
				double r = DistanceOfPoints(grid_to_coor, p_allsonarz->getval(t, s).C.centre);
				if(r - p_allsonarz->getval(t, s).C.radius < -BETA){
					p_minimap->setval(grid_now.x, grid_now.y, 0);
				}
			}
		}
	}
	return 0;
}
/*recalcu incorrect measure*/
int recalcu_incorrect_measure(AllSonarZ *p_allsonarz, miniMap *p_minimap)
{

	std::vector<UNIT_GRID> *p_vec_relevant_grid;
	UNIT_GRID grid_now;
	for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
		for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
			float recalcu_range = MAX_R;
			if(p_allsonarz->getval(t, s).correctFlag == 1)
				continue;
			p_vec_relevant_grid = p_allsonarz->sonarZ[t*p_allsonarz->sonarNum + s].p_vec_relevant_grid;
			if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0)
				continue;
			for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
				grid_now = p_vec_relevant_grid->at(i);
				if(p_minimap->getval(grid_now.x, grid_now.y) == 1){
					Point grid_to_coor;
					grid_to_coor.x = GRID_DEF*grid_now.x;
					grid_to_coor.y = GRID_DEF*grid_now.y;
					float r = DistanceOfPoints(grid_to_coor, p_allsonarz->getval(t, s).C.centre);
					if(r < recalcu_range){
						recalcu_range = r;
					}
				}
			}
			if(recalcu_range < p_allsonarz->getval(t, s).C.radius)
				p_allsonarz->getval(t, s).C.radius = recalcu_range;
		}
	}
	return 0;
}
/*map by incorrect measure*/
int map_by_incorrect_measure(miniMap *p_minimap, AllSonarZ *p_allsonarz)
{
	std::vector<UNIT_GRID> *p_vec_relevant_grid;
	UNIT_GRID grid_now;
	for(unsigned short t=0; t<p_allsonarz->timeTotal; t++){
		for(unsigned short s=0; s<p_allsonarz->sonarNum; s++){
			if(p_allsonarz->getval(t, s).correctFlag != 0)
				continue;
			p_vec_relevant_grid = p_allsonarz->sonarZ[t*p_allsonarz->sonarNum + s].p_vec_relevant_grid;
			if(p_vec_relevant_grid == NULL || p_vec_relevant_grid->size() == 0)
				continue;
			for(unsigned int i=0; i<p_vec_relevant_grid->size(); i++){
				grid_now = p_vec_relevant_grid->at(i);
				Point grid_to_coor;
				grid_to_coor.x = GRID_DEF*grid_now.x;
				grid_to_coor.y = GRID_DEF*grid_now.y;
				double r = DistanceOfPoints(grid_to_coor, p_allsonarz->getval(t, s).C.centre);
				if(r - p_allsonarz->getval(t, s).C.radius < -BETA){
					p_minimap->setval(grid_now.x, grid_now.y, 0);
				}
			}
		}
	}
	return 0;
}
//the area robot passby
void robot_passby(miniMap* p_minimap, vector<float> &x_offline, vector<float> &y_offline)
{
	int gridX, gridY, x, y;
	for(int i=0; i<(int)x_offline.size(); i++){
		gridX = (x_offline[i] + GRID_DEF/2.0)/GRID_DEF;
		gridY = (y_offline[i] + GRID_DEF/2.0)/GRID_DEF;
		for(x=gridX-2; x<=gridX+2; x++){
			for(y=gridY-2; y<=gridY+2; y++){
				if(x>=p_minimap->xmin&&x<=p_minimap->xmax  && y>=p_minimap->ymin&&y<=p_minimap->ymax)
				p_minimap->setval(x, y, 0);
			}
		}
	}
}
//all mapping process
bool MLS_Mapping::MLS_Mapping_Process(miniMap* p_minimap,string originFilePath)
{
	unsigned int xmin = 0,  xmax = 0,  ymin = 0, ymax = 0 ;
	if(NULL == p_minimap)
	{
		return false;
	}
	p_minimap->getMapSize(xmin,xmax,ymin, ymax);
	setOriginFilePath(originFilePath);
	//printf("forwardmal: the edge:  xmin:%u, xmax:%u, ymin:%u, ymax:%u\n", xmin, xmax, ymin, ymax);
	vector<float> x_offline, y_offline, th_offline;
	vector<unsigned int> sonar2_offline, sonar3_offline, sonar7_offline, sonar8_offline, sonar9_offline, sonar10_offline, sonar11_offline, sonar12_offline, sonar13_offline;

	if(getDataFromFile(x_offline, y_offline, th_offline, sonar2_offline, sonar3_offline, sonar7_offline, sonar8_offline,
			sonar9_offline, sonar10_offline, sonar11_offline, sonar12_offline, sonar13_offline) != 0)
	{
		return false;
	}
	//printf("forwardmal: file read-x:     %u\n", x_offline.size());
	//printf("forwardmal: file read-y:     %u\n", y_offline.size());
	//printf("forwardmal: file read-th:     %u\n", th_offline.size());
	//printf("forwardmal: file read-2:     %u\n", sonar2_offline.size());
	//printf("forwardmal: file read-3:     %u\n", sonar3_offline.size());
	//printf("forwardmal: file read-7:     %u\n", sonar7_offline.size());
	//printf("forwardmal: file read-8:     %u\n", sonar8_offline.size());
	//printf("forwardmal: file read-9:     %u\n", sonar9_offline.size());
	//printf("forwardmal: file read-10:     %u\n", sonar10_offline.size());
	//printf("forwardmal: file read-11:     %u\n", sonar11_offline.size());
	//printf("forwardmal: file read-12:     %u\n", sonar12_offline.size());
	//printf("forwardmal: file read-13:     %u\n", sonar13_offline.size());

	/*变量：指针创建*/
	mSP_Map* p_mSPmap = NULL;
	mSP_Map* p_mSPmap_0 = NULL;
	mSP_Map* p_mSPmap_1 = NULL;
	AllSonarZ *p_allsonarz = NULL;

	p_mSPmap = new mSP_Map(xmin, xmax, ymin, ymax, 0);
	p_mSPmap_0 = new mSP_Map(xmin, xmax, ymin, ymax, 0);
	p_mSPmap_1 = new mSP_Map(xmin, xmax, ymin, ymax, 0);
	p_allsonarz = new AllSonarZ(x_offline, y_offline, th_offline, sonar2_offline, sonar3_offline, sonar7_offline, sonar8_offline,
		sonar9_offline, sonar10_offline, sonar11_offline, sonar12_offline, sonar13_offline);

	Construct_Relationship(*p_minimap, *p_allsonarz);
	//printf("forwardmal: relationship constructed!\n");
	mSP_map_construct(0, p_allsonarz, p_mSPmap_0, p_mSPmap_1);
	mSP_map_construct(1, p_allsonarz, p_mSPmap_0, p_mSPmap_1);
	merge_mSP_map(p_mSPmap, p_mSPmap_0, p_mSPmap_1, x_offline, y_offline);
	//printf("forwardmal: mspMap constructed!\n");
	delete p_mSPmap_0;
	delete p_mSPmap_1;
	p_mSPmap_0 = NULL;
	p_mSPmap_1 = NULL;

	all_measure_classify(p_mSPmap, p_allsonarz);
	//printf("forwardmal: all measure classified!\n");

	delete p_mSPmap;
	p_mSPmap = NULL;

	map_by_correct_measure(p_minimap, p_allsonarz);
	//printf("forwardmal: map_by_correct_measure!\n");
	recalcu_incorrect_measure(p_allsonarz, p_minimap);
	map_by_incorrect_measure(p_minimap, p_allsonarz);
	//printf("forwardmal: map_by_incorrect_measure!\n");
	robot_passby(p_minimap, x_offline, y_offline);
	//printf("forwardmal: robot_pass_by!\n");

	delete p_allsonarz;
	p_allsonarz = NULL;
	//printf("forwardmal: p_allsonarz deleted!\n");

	return true;
}


