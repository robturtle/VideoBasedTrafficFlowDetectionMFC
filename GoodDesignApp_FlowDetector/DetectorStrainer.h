/***********************************************************************************
	可移植性: 用到的MFC类-CPoint, CRect
************************************************************************************/
#pragma once
#ifndef DETECTOR_STRAINER_LIUYANG_H
#define DETECTOR_STRAINER_LIUYANG_H
#include<vector>
#include<queue>
#include<fstream>
#include<string>
#include<stdlib.h>
#include<functional>
using namespace std;

//***********************************************单个检测器************************************************************
class c_flow_detector
{
	friend class c_detector_strainer;
	friend class c_flow_counter;
	friend bool FD_descending(c_flow_detector&, c_flow_detector&);
	//运算符重载
	//比较运算均比较强度
	friend bool operator>(c_flow_detector a, c_flow_detector b){ return a.n_current_intension > b.n_current_intension; }
	friend bool operator<(c_flow_detector a, c_flow_detector b){ return a.n_current_intension < b.n_current_intension; }
	friend bool operator>=(c_flow_detector a, c_flow_detector b){ return a.n_current_intension >= b.n_current_intension; }
	friend bool operator<=(c_flow_detector a, c_flow_detector b){ return a.n_current_intension <= b.n_current_intension; }
	//自加运算增加累计次数
	c_flow_detector& operator++(int){n_select_times++; return *this;}
	c_flow_detector& operator++(){++n_select_times; return *this;}
	//等于运算比较index
	friend bool operator==(c_flow_detector a, c_flow_detector b){return a.index == b.index;}
	//实现
public:
	c_flow_detector(byte *&p_data, CPoint set, int intension=0);//真正的构造函数
	c_flow_detector(int intension);//用于与int类型运算//提供给int类自动调用，不允许手动调用（仅用于填充时除外）
	virtual ~c_flow_detector(void);
	//操作
	inline int get_intense(int width, int height);
	inline int get_rgb_intense(int width, int height, int& r, int& g, int& b);
	static void set_bytes_per_line(int value){n_bytes_per_line=value;}//操作前及换视频后必须调用此函数
private:
	static int index_max;
	int index;
	CPoint pt_set;//指示区域的左下角点（值最小的点）, PS：与图像显示有关
	byte *p_data;//指示区域的数据起点
	int n_current_intension;//指示当前强度
	//与统计数据有关
	int n_select_times;//指示被选中次数
	int n_mean_intension;//指示平均强度，与背景刷新有关
	static int n_bytes_per_line;
};


//*****************************************************过滤器生成检测器组，执行统计筛选的工作**************************************************
class c_detector_strainer
{
	friend class c_flow_counter;
public:
	c_detector_strainer(int bmp_weight=0, int bmp_height=0, int bytes_per_line=0, byte *p_data=NULL);
	~c_detector_strainer(void);
	//操作
	bool set_detect_zone(CRect zone);//根据检测区域和图像参数生成检测器组群
	bool find_extremes(void);//根据检测器组和图像参数筛选特征值最大/最小的检测器
	bool statistic(void);//根据获得的最大/最小检测器统计得出强度排序最稳定的检测器
	bool switching_flush_status(void) { return b_flush_finish = !b_flush_finish;}
	//属性
	bool is_valid(void) const {return b_valid;}
	bool is_flush_finished(void) const {return b_flush_finish;}
	int get_que_max(void) const {return n_que_max;}
	int get_lst_max(void) const {return n_lst_max;}
	void get_cur_top_rect(CRect *&prc);
	void get_cur_bottom_rect(CRect *&prc);
	void get_sum_top_rect(CRect *&prc);
	void get_sum_bottom_rect(CRect *&prc);
private:
	bool b_valid;		//指示是否完全初始化
	CRect rc_zone;	//imitated_afx.h中声明的类名与MFC同，下不赘述//指示滤器作用的范围（在BMP上）
	//与图像信息有关
	int n_bytes_per_line;//每行比特数
	int n_bmp_width;
	int n_bmp_height;
	byte *p_bmp_data;		//指向数据区
	//与检测器有关
	int n_wnd_width;
	int n_wnd_height;		//指示每个检测器窗口
	int n_detector_set_index;//检测器起始序号
	int n_detector_num;	//检测器总数
	vector<c_flow_detector> lst_detectors;
	//与筛选操作有关
	priority_queue<c_flow_detector, vector<c_flow_detector>, greater<c_flow_detector> > que_tops;				//栈顶为最小值
	priority_queue<c_flow_detector, vector<c_flow_detector>, less<c_flow_detector> > que_bottoms;//每帧排序	//栈顶为最大值
	unsigned n_que_max;
	vector<c_flow_detector> lst_tops;
	vector<c_flow_detector> lst_bottoms;//累积排序
	unsigned n_lst_max;
	//刷新操作控制
	int n_min_last;			//刷新操作持续的最小帧数
	int n_tolerant_change;	//当变化小于此值时认为刷新完成
	bool b_flush_finish;		//指示是否在刷新中
	int n_last_frame;		//指示已持续的帧数
	void reset_list(void);
	bool b_updated;		//通知流量计数器更新检测器
};

//******************************************************流量计数器*****************************************************************
#define RET_INITIALIZING    0
#define RET_OCCUPIED	    1
#define RET_NO_OCCUPIED	    2
class c_flow_counter
{
public:
	c_flow_counter();
	~c_flow_counter(void);
	//操作
	int is_occupied(c_detector_strainer &detector_group);
	int is_occupied(c_flow_detector *&detector_group, int detector_num);
	//属性
	int get_flow_sum(void) const {return n_flow_sum;}
private:
	int n_flow_sum;
	//强度排序操作有关
	vector<c_flow_detector> lst_tops;
	vector<c_flow_detector> lst_bottoms;
	priority_queue<int, vector<int>, less<int> > que_tops;			//栈顶是最大值
	priority_queue<int, vector<int>, greater<int> > que_bottoms;	//栈顶是最小值
	unsigned n_tolerant_err;
	//流量计数
	int n_status;	//当前帧状态
	int n_occupied_times;
	int n_no_occupied_times;
	int flow_count(void);
	//手工数据采集：使用键盘1、2、3进行人工判断无车、有车或阴影
//#define _DATA_COLLECT
#ifdef _DATA_COLLECT
#define STATUS_NORMAL			1
#define STATUS_CAR_PASSINGBY	2
#define STATUS_SHADOW			3
private:
	bool b_collect;
public:
	void write_data(c_detector_strainer &detector_group, char *filename);
	void write_data(c_detector_strainer &detector_group, char *filename, int point_status);
	void set_collect(bool val) {b_collect = val;}
#endif
};

#endif