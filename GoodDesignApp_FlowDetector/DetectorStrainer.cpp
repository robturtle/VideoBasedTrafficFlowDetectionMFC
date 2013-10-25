#include "stdafx.h"
#include "DetectorStrainer.h"
#include "rgb2hsi.h"

//****************************************************单个检测器的实现**********************************************************
int c_flow_detector::index_max=0;
int c_flow_detector::n_bytes_per_line=0;

c_flow_detector::c_flow_detector(byte *&buf, CPoint set, int intension/* =0 */)
:index(index_max++)
,p_data(buf)
,pt_set(set)
,n_current_intension(intension)
,n_select_times(0)
,n_mean_intension(intension){}

c_flow_detector::c_flow_detector(int intension) 
:n_current_intension(intension) 
,n_select_times(-1)
,p_data(NULL)
{}	//提供给int类自动调用，不允许手动调用 (用于填充的情况除外）

c_flow_detector::~c_flow_detector(void){}

int c_flow_detector::get_intense(int width, int height){
	if(NULL == p_data) return -127;
	int width_3 = width * 3;
	double multiple = 1.0 / ( width * height * 3);
	int n_sum=0;
	byte *p_set = p_data;//区域的起点
	byte *p_line_set = p_set;//某行的起点
	for(int j=0; j<height; j++, p_line_set += n_bytes_per_line){//每次计算完一行后跳到下一行
		byte *p_tmp = p_line_set;
		for(int k=0; k<width_3; k++)		//每行计算 n_wnd_width * 3 个byte
			n_sum += *(p_tmp++);
	}
	return n_current_intension = int(n_sum*multiple);
}

int c_flow_detector::get_rgb_intense(int width, int height, int& r, int&g, int&b){
	if(NULL == p_data) {r=g=b=-127;};
	int width_3 = width * 3;
	double multiple = 1.0 / ( width * height);
	int n_r, n_g, n_b;
	n_r = n_g = n_b = 0;
	byte *p_set = p_data;
	byte *p_line_set = p_set;
	for(int j=0; j<height; j++, p_line_set += n_bytes_per_line){
		byte *p_tmp = p_line_set;
		for(int i=0; i<width; i++){
			n_b += *(p_tmp++);
			n_g += *(p_tmp++);
			n_r += *(p_tmp++);
		}
	}
	r = int(n_r * multiple);
	g = int(n_g * multiple);
	b = int(n_b * multiple);
	return 0;
}


//***************************************************检测器筛选器实现***********************************************************
#define DEF_WND_WIDTH		5
#define DEF_WND_HEIGHT		5
#define DEF_QUE_CONTENT		10//指示优先队列的容量
#define DEF_LST_CONTENT		50//指示累积列表的容量
#define TOLERANT_CHANGE		8	//当变化小于此值时，认为检测器筛选已趋于稳定
#define MIN_LAST_FRAME		100	//指定最小刷新帧数
c_detector_strainer::c_detector_strainer(int bmp_weight/* =0 */, int bmp_height/* =0 */, int bytes_per_line/* =0 */, byte *p_data/* =NULL */)
:n_bmp_width(bmp_weight)
,n_bmp_height(bmp_height)
,n_bytes_per_line(bytes_per_line)
,p_bmp_data(p_data)
,b_valid(false)
,rc_zone(0,0,0,0)
,n_wnd_width(DEF_WND_WIDTH)
,n_wnd_height(DEF_WND_HEIGHT)
,n_detector_num(0)
,n_que_max(DEF_QUE_CONTENT)
,n_lst_max(DEF_LST_CONTENT)
,n_min_last(MIN_LAST_FRAME)
,n_tolerant_change(TOLERANT_CHANGE)
,b_flush_finish(false)
,n_last_frame(0)
,b_updated(false)
{
	if(n_bmp_width && n_bmp_height && n_bytes_per_line && p_bmp_data){
		b_valid=true;		//若所有变量均已赋值，则认为初始化完全
		c_flow_detector::set_bytes_per_line(n_bytes_per_line);	//创建类时便将c_flow_detector类初始化
	}
}

c_detector_strainer::~c_detector_strainer(void){}

#define swaping(a,b,T) {T tmp=a; a=b; b=tmp;}
//根据检测区域和图像参数生成检测器组群
//若生成足够检测器组则返回true，反之返回false
bool c_detector_strainer::set_detect_zone(CRect zone)
{
	//参数检查
	if(!b_valid) return false;
	if(zone.left > zone.right) swaping(zone.left, zone.right, int);
	if(zone.top > zone.bottom) swaping(zone.top, zone.bottom, int);
	int	n_left=zone.left,
		n_top=zone.top,
		n_zone_width=zone.Width(),
		n_zone_height=zone.Height();
	rc_zone=zone;
	//检测区划分
	n_detector_set_index = c_flow_detector::index_max;
	byte *p_set = p_bmp_data + n_bytes_per_line*n_top + n_left*3;//区域的起点
	for(int j=0; j<n_zone_height; j+=n_wnd_height)
	{
		byte *p_line_set = p_set + j*n_bytes_per_line;//该行的起点
		for(int i=0; i<n_zone_width; i+=n_wnd_width)
		{
			CPoint pt_set( n_left+i, n_top+j);
			byte *p_detector_set = p_line_set + i*3;//检测区的起点
			c_flow_detector newone(p_detector_set, pt_set);
			lst_detectors.push_back(newone);
		}
	}
	//检测是否存在足够的检测区
	if(lst_detectors.size() >= n_que_max*2){
		n_detector_num = c_flow_detector::index_max - n_detector_set_index + 1;
		return true;
	}else 
		return false;
}

//根据检测器组和图像参数筛选特征值最大/最小的检测器
bool c_detector_strainer::find_extremes(void)
{
	int n_det_num=lst_detectors.size();

	//优先队列初始化
	while(!que_tops.empty()) que_tops.pop();
	while(!que_bottoms.empty()) que_bottoms.pop();
	que_tops.push(c_flow_detector(0));
	que_bottoms.push(c_flow_detector(255));

	//遍历所有检测器
	for(int i=0; i<n_det_num; i++){
		int n_sum=0;//清零
		//计算第 i  个检测器内的区域强度
		c_flow_detector &tmp = lst_detectors.at(i);
		tmp.get_intense(n_wnd_width, n_wnd_height);

		//排序筛选
		if(que_tops.top() <= tmp) //top队列：若队列内最小的值比当前值小，当前值入栈
			que_tops.push(tmp);
		if(que_bottoms.top() >= tmp) //bottom队列：若队列内最大的值比当前值大，当前值入栈
			que_bottoms.push(tmp);
		if(que_tops.size() > n_que_max) que_tops.pop();
		if(que_bottoms.size() > n_que_max) que_bottoms.pop();
	}
	return true;
}

//根据获得的最大/最小检测器统计得出强度排序最稳定的检测器
bool FD_descending(c_flow_detector &a, c_flow_detector &b){return a.n_select_times > b.n_select_times;}
bool c_detector_strainer::statistic(void)
{
	find_extremes();

	//控制
	int change_times=0;
	n_last_frame++;

	//tops 列表
	while(!que_tops.empty()){
		c_flow_detector tmp = que_tops.top();
		que_tops.pop();

		vector<c_flow_detector>::iterator location=find(lst_tops.begin(), lst_tops.end(), tmp);
		if(lst_tops.end()==location){		//如果没有该检测器的记录
			lst_tops.push_back(tmp);
			change_times++;		//变更记录加1
		}
		else
			(*location)++;				//如果找到记录，则将累计次数加1
	}
	sort(lst_tops.begin(), lst_tops.end(), FD_descending);
	lst_tops.resize(n_lst_max, c_flow_detector(-1));

	//bottoms 列表
	while(!que_bottoms.empty()){
		c_flow_detector tmp = que_bottoms.top();
		que_bottoms.pop();

		vector<c_flow_detector>::iterator location=find(lst_bottoms.begin(), lst_bottoms.end(), tmp);
		if(lst_bottoms.end()==location){		//如果没有该检测器的记录
			lst_bottoms.push_back(tmp);
			change_times++;
		}
		else
			(*location)++;				//如果找到记录，则将累计次数加1
	}
	sort(lst_bottoms.begin(), lst_bottoms.end(), FD_descending);
	lst_bottoms.resize(n_lst_max, c_flow_detector(-1));

	if(n_last_frame > n_min_last && change_times < n_tolerant_change){
		n_last_frame = 0;
		b_flush_finish = true;
		reset_list();
		b_updated = true;
	} else {
		b_flush_finish = false;
	}

	return b_flush_finish;

}

void c_detector_strainer::reset_list(void)
{
	lst_tops.resize(n_que_max, c_flow_detector(-1));
	lst_bottoms.resize(n_que_max, c_flow_detector(-1));
	for(unsigned i=0; i<n_que_max; i++){
		lst_tops.at(i).n_select_times = 1;
		lst_bottoms.at(i).n_select_times = 1;
	}
}

//返回检测器刷新状态，供画图用
void c_detector_strainer::get_cur_top_rect(CRect *&prc) 
{
	delete [] prc;
	prc = new CRect[n_que_max];
	CRect *ptmp=prc;

	if(que_tops.empty())
		find_extremes();
	for(unsigned i=0; i<n_que_max; i++){
		c_flow_detector &tmp = que_tops.top();
		que_tops.pop();
		
		CPoint pt_end(tmp.pt_set.x+n_wnd_width, tmp.pt_set.y+n_wnd_height);
		CRect rc_zone(tmp.pt_set, pt_end);
		*(ptmp++) = rc_zone;
	}
	find_extremes();
}

void c_detector_strainer::get_cur_bottom_rect(CRect *&prc)
{
	delete [] prc;
	prc = new CRect[n_que_max];
	CRect *ptmp=prc;

	if(que_bottoms.empty())
		find_extremes();
	for(unsigned i=0; i<n_que_max; i++){
		c_flow_detector &tmp = que_bottoms.top();
		que_bottoms.pop();

		CPoint pt_end(tmp.pt_set.x+n_wnd_width, tmp.pt_set.y+n_wnd_height);
		CRect rc_zone(tmp.pt_set, pt_end);
		*(ptmp++) = rc_zone;
	}
	find_extremes();
}
void c_detector_strainer::get_sum_top_rect(CRect *&prc)
{
	delete [] prc;
	prc = new CRect[n_lst_max+1];
	CRect *ptmp = prc;

	unsigned n_size = lst_tops.size();
	for(unsigned i=0; i<n_size; i++){
		c_flow_detector &tmp = lst_tops.at(i);
		if(NULL == tmp.p_data) break;

		CPoint pt_end(tmp.pt_set.x+n_wnd_width, tmp.pt_set.y+n_wnd_height);
		CRect rc_zone(tmp.pt_set, pt_end);
		*(ptmp++) = rc_zone;
	}
	*ptmp = NULL;//用NULL来标识结尾
}
void c_detector_strainer::get_sum_bottom_rect(CRect *&prc)
{
	delete [] prc;
	prc = new CRect[n_lst_max+1];
	CRect *ptmp = prc;

	unsigned n_size = lst_bottoms.size();
	for(unsigned i=0; i<n_size; i++){
		c_flow_detector &tmp = lst_bottoms.at(i);
		if(NULL == tmp.p_data) break;

		CPoint pt_end(tmp.pt_set.x+n_wnd_width, tmp.pt_set.y+n_wnd_height);
		CRect rc_zone(tmp.pt_set, pt_end);
		*(ptmp++) = rc_zone;
	}
	*ptmp = NULL;//用NULL来标识结尾
}

//*******************************************************流量计数器**************************************************************
#define DEF_TOLERANT_ERR		2		//容许几个检测器偏差
c_flow_counter::c_flow_counter()
:n_flow_sum(0)
,n_tolerant_err(DEF_TOLERANT_ERR)
,n_status(RET_INITIALIZING)
,n_occupied_times(0)
,n_no_occupied_times(0)
{
#ifdef _DATA_COLLECT
	b_collect = false;
#endif
}

c_flow_counter::~c_flow_counter(){}

//c_flow_counter的主要调用入口
//返回值：
//		RET_INITIALIZING	指示detector_group尚未就绪
//		RET_OCCUPIED		指示该检测器组上有物体遮挡
//		RET_NO_OCCUPIED		指示检测器组状态正常
int c_flow_counter::is_occupied(c_detector_strainer &detector_group)
{
	//变量
	int que_max = detector_group.n_que_max;
	int	width = detector_group.n_wnd_width,
		height = detector_group.n_wnd_height;
	int intense;

	//控制
	if(detector_group.b_updated){//如果筛选器有更新，则载入更新
		lst_tops = detector_group.lst_tops;
		lst_bottoms = detector_group.lst_bottoms;
		detector_group.b_updated = false;
	} else if(lst_tops.empty() && lst_bottoms.empty()){//如果筛选器无更新且自身列表为空，说明尚未完成第一次刷新动作
		return RET_INITIALIZING;
	}

	//找出top区最小的前n_tolerant_err个强度值，以及bottom区最大的前n_tolerant个强度值
	while (!que_tops.empty()) que_tops.pop();
	while(!que_bottoms.empty()) que_bottoms.pop();
	que_tops.push(255);
	que_bottoms.push(0);

	for(int i=0; i<que_max; i++){
		c_flow_detector tmp = lst_tops.at(i);
		if(NULL == tmp.p_data) break;
		intense = tmp.get_intense(width, height);
		if(que_tops.top() > intense) que_tops.push(intense);
		if(que_tops.size() > n_tolerant_err) que_tops.pop();
	}
	for(int i=0; i<que_max; i++){
		c_flow_detector tmp = lst_bottoms.at(i);
		if(NULL == tmp.p_data) break;
		intense = tmp.get_intense(width, height);
		if(que_bottoms.top() < intense) que_bottoms.push(intense);
		if(que_bottoms.size() > n_tolerant_err) que_bottoms.pop();
	}

	n_status = (que_tops.top() > que_bottoms.top()) ? RET_NO_OCCUPIED : RET_OCCUPIED;

	flow_count();
	return n_status;
}

//若希望越过筛选器进行计数，可考虑实现此函数
int c_flow_counter::is_occupied(c_flow_detector *&detector_group, int detector_num){return false;}

//流量计数
#define VALID_OCCUPIED_TIMES	3	//连续2帧有车视为车辆到达
#define VALID_NO_OCCUPIED_TIMES	8	//连续8帧无车视为车辆离去
int c_flow_counter::flow_count()
{
	if(RET_OCCUPIED == n_status && 
		VALID_OCCUPIED_TIMES == ++n_occupied_times){
			n_flow_sum++;
			n_no_occupied_times = 0;
	} else if(RET_NO_OCCUPIED == n_status &&
		VALID_NO_OCCUPIED_TIMES < ++n_no_occupied_times){
			n_occupied_times = 0;
	}
	return n_flow_sum;
}

//数据输出
#ifdef _DATA_COLLECT
void c_flow_counter::write_data(c_detector_strainer &detector_group, char *filename)
{
	if(!b_collect) return;

	int que_max = detector_group.n_que_max;
	int	width = detector_group.n_wnd_width,
		height = detector_group.n_wnd_height;
	ofstream f_data;
	f_data.open(filename,ios_base::app);

	for(int i=0; i<que_max; i++){
		c_flow_detector tmp = lst_tops.at(i);
		if(NULL == tmp.p_data) break;
		int r, g, b;
		tmp.get_rgb_intense(width, height, r, g, b);
		char strtmp[50];
		int h = rgb2hsi_h(r,g,b);
		double s = rgb2hsi_s(r,g,b);
		sprintf_s(strtmp, sizeof(char)*50, 
			"%d,%d,%d,",r ,g ,b);
		f_data.write(strtmp, strlen(strtmp));
	}
	for(int i=0; i<que_max; i++){
		c_flow_detector tmp = lst_bottoms.at(i);
		if(NULL == tmp.p_data) break;
		int r, g, b;
		tmp.get_rgb_intense(width, height, r, g, b);
		char strtmp[50];
		int h = rgb2hsi_h(r,g,b);
		double s = rgb2hsi_s(r,g,b);
		sprintf_s(strtmp, sizeof(char)*50, 
			"%d,%d,%d,",r ,g ,b);
		f_data.write(strtmp, strlen(strtmp));
	}
	//f_data.write("\n",strlen("\n"));

	//b_collect = false;
}

void c_flow_counter::write_data(c_detector_strainer &detector_group, char *filename, int point_status){
	write_data(detector_group, filename);
	ofstream f_data;
	f_data.open(filename,ios_base::app);
	char *str_status;
	switch(point_status){
		case STATUS_NORMAL:
			str_status = "0,\n";
			break;
		case STATUS_CAR_PASSINGBY:
			str_status = "255,\n";
			break;
		case STATUS_SHADOW:
			str_status = "127,\n";
			break;
		default:
			str_status = "";
			break;
	}
	if("" != str_status)
		f_data.write(str_status, strlen(str_status));
}
#endif