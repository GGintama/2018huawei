#include "predict.h"





double third_Exponential_Smoothing(vector<double>  iv, int predice_T) {
	cout<<" third_Exponential_Smoothing"<<endl;
    if (iv.empty())  return 0.0;
	double result;
	int data_size = iv.size();
	double alpha = 0.179;              //平滑系数i
	double k1, k2, k3;               //平滑模型的系数
	vector<double> S1, S2, S3;   //分别 存储三次平滑结果   S1[1]对应iv[0]
	double temp = (iv[0] + iv[1] + iv[2]) / 3;
	S1.push_back(temp);               //设定初始值
	S2.push_back(temp);
	S3.push_back(temp);
	//计算一次平滑指数
	for (int i = 0;i != data_size;++i) {
		double temp = alpha*iv[i] + (1 - alpha)*S1[i];
		S1.push_back(temp);
	}
	//计算二次平滑指数
	for (int i = 0;i != data_size;++i) {
		double temp = alpha*S1[i + 1] + (1 - alpha)*S2[i];
		S2.push_back(temp);
	}
	//计算三次平滑指数
	for (int i = 0;i != data_size;++i) {
		double temp = alpha*S2[i + 1] + (1 - alpha)*S3[i];
		S3.push_back(temp);
	}
	//根据三次平滑指数预测模型计算出系数y(t+T) = a(t)+b(t)*T+c(t)*T*T;
	//a(t) = 3S1(t)-3S2(t)+S3(t);
	//b(t) = alpha/2(1-alpha)^2[(6-5alpha)S1(t)-2(5-4alpha)S2(t)+(4-3alpha)S3(t)];
	//c(t) = alpha^2/2(1-alpha)^2[S1(t)-2S2(t)+S3(t)]
	k1 = 3 * S1.back() - 3 * S2.back() + S3.back();
	k2 = alpha / (2 * (1 - alpha)*(1 - alpha)) * ((6 - 5 * alpha)*S1.back() - 2 * (5 - 4 * alpha)*S2.back() + (4 - 3 * alpha)*S3.back());
	k3 = alpha*alpha / (2 * (1 - alpha)*(1 - alpha))*(S1.back() - 2 * S2.back() + S3.back());
	result = k1 + k2*predice_T + k3*predice_T*predice_T;
	if (result < 0)
		return 0.0;
	return result;
}


//版本一：对某一种虚拟机进行预测。通过每7天求平均的方式//可以不用7天求平均，然后通过三次指数平滑得到预测值
//参数1 为待遇测虚拟机型号，参数2为该型号训练数据，即已有的数据集合，第三个、第四个参数表示要预测的起止天数
int oneflavor_predict(const string &flavor_name, vector<string> times, string train_begin, string train_end, string predict_begin, string predict_end) {
    cout<<"one falovr_predict"<<endl;
	vector<int> flavor_nums;
	vector<int> _y;       //存储训练数据变量
	tm temp_tm;     //中转变量，现将string转化为该变量再转化为tms
	int train_end_time, predict_begin_time, predict_end_time;   //作为输入x变量
	temp_tm.tm_hour = 0;
	temp_tm.tm_min = 0;
	temp_tm.tm_sec = 0;
	temp_tm.tm_isdst = 0;
	temp_tm.tm_year = atoi(train_begin.c_str()) - 1900;
	temp_tm.tm_mon = atoi(train_begin.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(train_begin.c_str() + 8);
	time_t first_day = mktime(&temp_tm);       //表示预测开始的第一天   
											   //训练结束天数
	temp_tm.tm_year = atoi(train_end.c_str()) - 1900;
	temp_tm.tm_mon = atoi(train_end.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(train_end.c_str() + 8);
	train_end_time = (mktime(&temp_tm) - first_day) / 86400 + 1;  //预测结束时间 ,这里转化成了x变量
																  //预测开始天数
	temp_tm.tm_year = atoi(predict_begin.c_str()) - 1900;
	temp_tm.tm_mon = atoi(predict_begin.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(predict_begin.c_str() + 8);
	predict_begin_time = (mktime(&temp_tm) - first_day) / 86400 + 1;  //预测结束时间 ,这里转化成了x变量
																	  //预测结束天数
	temp_tm.tm_year = atoi(predict_end.c_str()) - 1900;
	temp_tm.tm_mon = atoi(predict_end.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(predict_end.c_str() + 8);
	predict_end_time = (mktime(&temp_tm) - first_day) / 86400 + 1;  //预测结束时间 ,这里转化成了x变量

	for (int i = 0;i != train_end_time;i++)   //初始化为全0
		_y.push_back(0);
	//_y[0] = train_end_time;           //0下标不存数据 用于记录该数据的组数
	for (auto str : times) {
		temp_tm.tm_year = atoi(str.c_str()) - 1900;
		temp_tm.tm_mon = atoi(str.c_str() + 5) - 1;    //+5+8是将指针后移
		temp_tm.tm_mday = atoi(str.c_str() + 8);
		_y[(mktime(&temp_tm) - first_day) / 86400]++;
		//cout << str << " "<<  endl;
	}

	//	cout << train_end_time << " "<<predict_begin_time<< " "<<predict_end_time << endl;

	//去除异常点
	double average_normal = 0.0, normal_num = 0.0;
	for (int i = 1;i != train_end_time;i++)
		if (_y[i] != 0) {
			average_normal += _y[i];
			normal_num++;
		}
	average_normal = average_normal / normal_num;   //求平均值
	for (int i = 1;i != train_end_time;i++) {
		if (_y[i] > 5 * average_normal)
			_y[i] = average_normal + 1;
	}

	//积累申请量
	for (int i = 1;i != train_end_time;i++)
 		_y[i] += _y[i - 1];
	//做一下平均

	vector<double>  average_y;
	int interval = predict_end_time - predict_begin_time;
	int data_size = _y.size();
//	for (int i = 0;i != interval / 2;i++) {
//		_y.insert(_y.begin(), _y[0]);
  //  	_y.insert(_y.end(), _y.back());
//	}
//	for (int i = 0;i != data_size;i++) {
  // 	int average_temp = 0;
	//	for (int k = i;k != i + interval;k++)
//			average_temp += _y[k];
//		average_y.push_back(average_temp / interval);
//	}

   for (int i = 0;i != _y.size();i++)

        average_y.push_back(_y[i]);

	int predict_result = 0;

//    for(int i = predict_begin_time-train_end_time;i!=predict_end_time - train_end_time;++i)
  //      predict_result+=third_Exponential_Smoothing(average_y,i+1);

   //  return predict_result;


   	predict_result = third_Exponential_Smoothing(average_y, predict_end_time - train_end_time-1);
   predict_result -= third_Exponential_Smoothing(average_y, predict_begin_time - train_end_time-1);

    //  	predict_result = predict_result - _y.back();
	return predict_result>0 ? predict_result : 0;

}

//分配策略初版  单纯的将预测到的虚拟机 一个一个压入服务器中，当一个服务器满，转战下一个。
//这里假设result_vm里面都是分开的，每个vm类的count都是
//第一个参数表示要分配的服务器 ，第二个参数表示预测结果



class Compare_cpu {
public:
    bool operator()(const VirtualMachine& lhs, const VirtualMachine& rhs)  const {
        if (lhs.get_cpu() < rhs.get_cpu())
            return true;
        else if (lhs.get_cpu() == rhs.get_cpu()) {
            return lhs.get_memory() < rhs.get_memory();
        }
        else {
            return false;
        }
    }
};

class Compare_mem {
public:
    bool operator()(const VirtualMachine& lhs, const VirtualMachine& rhs)  const {
        if (lhs.get_memory() < rhs.get_memory())
            return true;
        else if (lhs.get_memory() == rhs.get_memory()) {
            return lhs.get_cpu() < rhs.get_cpu();
        }
        else {
            return false;
        }
    }
};

vector<map<string, int> > distribute_new(const Server &s, vector<VirtualMachine> &predict_vm, string prepare_to_optimization) {
    vector<map<string, int> >  result;
    if(prepare_to_optimization[0]=='C')
        sort(predict_vm.begin(), predict_vm.end(), Compare_cpu());
    else
        sort(predict_vm.begin(), predict_vm.end(), Compare_mem());   //靠靠靠
    
    int cpu = s.get_cpu();
    int memory = s.get_memory();
    int i, j;
    while (!predict_vm.empty()) {
        i = 0;
        j = predict_vm.size() - 1;
        int tmp_cpu = 0;
        int tmp_memory = 0;
        map<string, int> tmp_vm;
        place_again:
        while (i<=j) {
            if (predict_vm.empty())
                    break;
            tmp_cpu += predict_vm[j].get_cpu();
            tmp_memory += (predict_vm[j].get_memory()) / 1024;
            if (tmp_cpu <= cpu && tmp_memory <= memory) {
                ++tmp_vm[predict_vm[j].get_name()];
                predict_vm.erase(predict_vm.begin()+j);
                --j;
            }
            else {
                tmp_cpu -= predict_vm[j].get_cpu();
                tmp_memory -= (predict_vm[j].get_memory()) / 1024;
                --j;
                goto place_again;
            }
            if (predict_vm.empty())
                break;
            tmp_cpu += predict_vm[i].get_cpu();
            tmp_memory += (predict_vm[i].get_memory()) / 1024;
            if (tmp_cpu <= cpu && tmp_memory <= memory) {
                ++tmp_vm[predict_vm[i].get_name()];
                predict_vm.erase(predict_vm.begin() + i);
                --j;
            }
            else 
                break;
        }
        result.push_back(tmp_vm);
    }
    return result;

}

//你要完成的功能总入口
//info是输入信息（包括物理机和虚拟机信息之类的，即input文件）  ，data是训练数据， data_num代表数据的行数 data[k] 代表第k行数据 ，也就是第k个虚拟机
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	/*-------------------------处理函数第一个参数info---------------------*/

	//vector<double> test_iv = { 225.2,249.9,263.2,293.6,318.9,356.7,363.3,424.2,466.5,582.0,750.0 };
	//cout << third_Exponential_Smoothing(test_iv, 2);
    cout <<"11111111111111111  "<<endl;
	int s1, s2, s3;
	istringstream is(info[0]);
	is >> s1 >> s2 >> s3;
	Server server(s1, s2, s3);       //每台服务器的性能
cout<<"duancuowu"<<endl;
	//将输入的需要预测的虚拟机存到一个vm类的容器中去，方便使用
	vector<VirtualMachine> need_to_pridect;     //要预测的虚拟机集合
	int i = 3;
	while (*info[i] != '\r') {
		string s1;
		int cpu, memory;
		istringstream is(info[i]);
		is >> s1 >> cpu >> memory;
		VirtualMachine vm(s1,cpu,memory);
		need_to_pridect.push_back(vm);
		i++;
	}

cout<<"23445566677"<<endl;
	string prepare_to_optimization = info[i+1];
	string train_begin;              //训练开始时间
	string train_end;               //训练结束时间
	string predict_begin = info[i + 3];    //预测开始时间
	string predict_end = info[i + 4];     //预测结束时间
	/*-----------------------------------------------------------------------*/
	/*-------------------------处理函数第二个参数data---------------------*/
	map<string, vector<string>> train_vms;     //表示从虚拟机规格到该规格申请时间集合的映射，为训练数据
	int k = 0;
	while (k!=data_num) {
		string s1 ,s2,s3,s4;
		istringstream is(data[k]);
		is >> s1 >> s2 >> s3 >> s4;
		if (k == 0)
			train_begin = s3;
		if (k == data_num-1)
			train_end = s3;
		train_vms[s2].push_back(s3);
		k++;
	}
	/*-----------------------------------------------------------------------*/
cout<<"1111111111111111111111222222222222222222"<<endl;

	/*-------------------------实际预测阶段---------------------*/
	vector<VirtualMachine> predict_vms;  //预测结果的虚拟机集合
	map<string, int> predict_vms2;  //表示虚拟机规格到数量的映射
	int total_num=0;   //预测得到的总数

	for (int i = 0;i != need_to_pridect.size();i++) {
		string name = need_to_pridect[i].get_name();
		int number = 0;
		number = oneflavor_predict(name, train_vms[name], train_begin, train_end, predict_begin, predict_end);
		if (number != 0) {
			for (int k = 0;k != number;k++) {
				predict_vms.push_back(need_to_pridect[i]);
			}
		}
		predict_vms2[name] = number;
		total_num += number;
	}
	/*-----------------------------------------------------------------------*/
cout<<"222222222222222222222222222222"<<endl;
	/*-------------------------虚拟机分配阶段---------------------*/
	//返回类型为一个vector,容器中每个元素为一个map，代表虚拟机规格以及数量的映射。
	vector<map<string, int> > result = distribute_new(server, predict_vms, prepare_to_optimization);
	char p[2000] = {0};        //存储结果
	strcat(p, to_string(total_num).c_str());
    p[strlen(p)]= '\r';
	p[strlen(p)] = '\n';
	for (auto &s : need_to_pridect) {
		char temp[20] = { 0 };
//		char temp_num[10] = { 0 };
		strncpy(temp, s.get_name().c_str(), s.get_name().size());
		temp[s.get_name().size()] = ' ';
		strcat(temp, to_string(predict_vms2[s.get_name()]).c_str());
		
        temp[strlen(temp)]= '\r';
        temp[strlen(temp)] = '\n';
		temp[strlen(temp)] = '\0';
		strcat(p, temp);
	}


	int servers_num = result.size();
 p[strlen(p)]= '\r';

	p[strlen(p)] = '\n';
	strcat(p, to_string(servers_num).c_str());
 p[strlen(p)]= '\r';
	p[strlen(p)] = '\n';
	int kk = 1;
	for (auto &s : result) {
//		char temp_num[10] = {0};
		strcat(p, to_string(kk++).c_str());
		p[strlen(p)] = ' ';
		map<string,int>::iterator it=s.begin();

		for (; it != s.end(); it++) {
			char temp[20] = {0};
//			char temp_num[10] = {0};
			strncpy(temp, it->first.c_str(), it->first.size());
			temp[it->first.size()] = ' ';
			strcat(temp, to_string(it->second).c_str());
			temp[strlen(temp)] = ' ';
			strcat(p, temp);
		}

 p[strlen(p)-1]= '\r';
	p[strlen(p)] = '\n';
		p[strlen(p)] = '\0';
	}
	char * result_file = p;
	//cout << result_file << endl;


	// 需要输出的内容
	//char * result_file = (char *)"17\n\n0 8 0 20";    //把所有结果写在一个指针里

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(result_file, filename);
}
