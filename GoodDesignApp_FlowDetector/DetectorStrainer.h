/***********************************************************************************
	����ֲ��: �õ���MFC��-CPoint, CRect
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

//***********************************************���������************************************************************
class c_flow_detector
{
	friend class c_detector_strainer;
	friend class c_flow_counter;
	friend bool FD_descending(c_flow_detector&, c_flow_detector&);
	//���������
	//�Ƚ�������Ƚ�ǿ��
	friend bool operator>(c_flow_detector a, c_flow_detector b){ return a.n_current_intension > b.n_current_intension; }
	friend bool operator<(c_flow_detector a, c_flow_detector b){ return a.n_current_intension < b.n_current_intension; }
	friend bool operator>=(c_flow_detector a, c_flow_detector b){ return a.n_current_intension >= b.n_current_intension; }
	friend bool operator<=(c_flow_detector a, c_flow_detector b){ return a.n_current_intension <= b.n_current_intension; }
	//�Լ����������ۼƴ���
	c_flow_detector& operator++(int){n_select_times++; return *this;}
	c_flow_detector& operator++(){++n_select_times; return *this;}
	//��������Ƚ�index
	friend bool operator==(c_flow_detector a, c_flow_detector b){return a.index == b.index;}
	//ʵ��
public:
	c_flow_detector(byte *&p_data, CPoint set, int intension=0);//�����Ĺ��캯��
	c_flow_detector(int intension);//������int��������//�ṩ��int���Զ����ã��������ֶ����ã����������ʱ���⣩
	virtual ~c_flow_detector(void);
	//����
	inline int get_intense(int width, int height);
	inline int get_rgb_intense(int width, int height, int& r, int& g, int& b);
	static void set_bytes_per_line(int value){n_bytes_per_line=value;}//����ǰ������Ƶ�������ô˺���
private:
	static int index_max;
	int index;
	CPoint pt_set;//ָʾ��������½ǵ㣨ֵ��С�ĵ㣩, PS����ͼ����ʾ�й�
	byte *p_data;//ָʾ������������
	int n_current_intension;//ָʾ��ǰǿ��
	//��ͳ�������й�
	int n_select_times;//ָʾ��ѡ�д���
	int n_mean_intension;//ָʾƽ��ǿ�ȣ��뱳��ˢ���й�
	static int n_bytes_per_line;
};


//*****************************************************���������ɼ�����飬ִ��ͳ��ɸѡ�Ĺ���**************************************************
class c_detector_strainer
{
	friend class c_flow_counter;
public:
	c_detector_strainer(int bmp_weight=0, int bmp_height=0, int bytes_per_line=0, byte *p_data=NULL);
	~c_detector_strainer(void);
	//����
	bool set_detect_zone(CRect zone);//���ݼ�������ͼ��������ɼ������Ⱥ
	bool find_extremes(void);//���ݼ�������ͼ�����ɸѡ����ֵ���/��С�ļ����
	bool statistic(void);//���ݻ�õ����/��С�����ͳ�Ƶó�ǿ���������ȶ��ļ����
	bool switching_flush_status(void) { return b_flush_finish = !b_flush_finish;}
	//����
	bool is_valid(void) const {return b_valid;}
	bool is_flush_finished(void) const {return b_flush_finish;}
	int get_que_max(void) const {return n_que_max;}
	int get_lst_max(void) const {return n_lst_max;}
	void get_cur_top_rect(CRect *&prc);
	void get_cur_bottom_rect(CRect *&prc);
	void get_sum_top_rect(CRect *&prc);
	void get_sum_bottom_rect(CRect *&prc);
private:
	bool b_valid;		//ָʾ�Ƿ���ȫ��ʼ��
	CRect rc_zone;	//imitated_afx.h��������������MFCͬ���²�׸��//ָʾ�������õķ�Χ����BMP�ϣ�
	//��ͼ����Ϣ�й�
	int n_bytes_per_line;//ÿ�б�����
	int n_bmp_width;
	int n_bmp_height;
	byte *p_bmp_data;		//ָ��������
	//�������й�
	int n_wnd_width;
	int n_wnd_height;		//ָʾÿ�����������
	int n_detector_set_index;//�������ʼ���
	int n_detector_num;	//���������
	vector<c_flow_detector> lst_detectors;
	//��ɸѡ�����й�
	priority_queue<c_flow_detector, vector<c_flow_detector>, greater<c_flow_detector> > que_tops;				//ջ��Ϊ��Сֵ
	priority_queue<c_flow_detector, vector<c_flow_detector>, less<c_flow_detector> > que_bottoms;//ÿ֡����	//ջ��Ϊ���ֵ
	unsigned n_que_max;
	vector<c_flow_detector> lst_tops;
	vector<c_flow_detector> lst_bottoms;//�ۻ�����
	unsigned n_lst_max;
	//ˢ�²�������
	int n_min_last;			//ˢ�²�����������С֡��
	int n_tolerant_change;	//���仯С�ڴ�ֵʱ��Ϊˢ�����
	bool b_flush_finish;		//ָʾ�Ƿ���ˢ����
	int n_last_frame;		//ָʾ�ѳ�����֡��
	void reset_list(void);
	bool b_updated;		//֪ͨ�������������¼����
};

//******************************************************����������*****************************************************************
#define RET_INITIALIZING    0
#define RET_OCCUPIED	    1
#define RET_NO_OCCUPIED	    2
class c_flow_counter
{
public:
	c_flow_counter();
	~c_flow_counter(void);
	//����
	int is_occupied(c_detector_strainer &detector_group);
	int is_occupied(c_flow_detector *&detector_group, int detector_num);
	//����
	int get_flow_sum(void) const {return n_flow_sum;}
private:
	int n_flow_sum;
	//ǿ����������й�
	vector<c_flow_detector> lst_tops;
	vector<c_flow_detector> lst_bottoms;
	priority_queue<int, vector<int>, less<int> > que_tops;			//ջ�������ֵ
	priority_queue<int, vector<int>, greater<int> > que_bottoms;	//ջ������Сֵ
	unsigned n_tolerant_err;
	//��������
	int n_status;	//��ǰ֡״̬
	int n_occupied_times;
	int n_no_occupied_times;
	int flow_count(void);
	//�ֹ����ݲɼ���ʹ�ü���1��2��3�����˹��ж��޳����г�����Ӱ
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