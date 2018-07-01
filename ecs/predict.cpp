#include "predict.h"





double third_Exponential_Smoothing(vector<double>  iv, int predice_T) {
	cout<<" third_Exponential_Smoothing"<<endl;
    if (iv.empty())  return 0.0;
	double result;
	int data_size = iv.size();
	double alpha = 0.179;              //ƽ��ϵ��i
	double k1, k2, k3;               //ƽ��ģ�͵�ϵ��
	vector<double> S1, S2, S3;   //�ֱ� �洢����ƽ�����   S1[1]��Ӧiv[0]
	double temp = (iv[0] + iv[1] + iv[2]) / 3;
	S1.push_back(temp);               //�趨��ʼֵ
	S2.push_back(temp);
	S3.push_back(temp);
	//����һ��ƽ��ָ��
	for (int i = 0;i != data_size;++i) {
		double temp = alpha*iv[i] + (1 - alpha)*S1[i];
		S1.push_back(temp);
	}
	//�������ƽ��ָ��
	for (int i = 0;i != data_size;++i) {
		double temp = alpha*S1[i + 1] + (1 - alpha)*S2[i];
		S2.push_back(temp);
	}
	//��������ƽ��ָ��
	for (int i = 0;i != data_size;++i) {
		double temp = alpha*S2[i + 1] + (1 - alpha)*S3[i];
		S3.push_back(temp);
	}
	//��������ƽ��ָ��Ԥ��ģ�ͼ����ϵ��y(t+T) = a(t)+b(t)*T+c(t)*T*T;
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


//�汾һ����ĳһ�����������Ԥ�⡣ͨ��ÿ7����ƽ���ķ�ʽ//���Բ���7����ƽ����Ȼ��ͨ������ָ��ƽ���õ�Ԥ��ֵ
//����1 Ϊ������������ͺţ�����2Ϊ���ͺ�ѵ�����ݣ������е����ݼ��ϣ������������ĸ�������ʾҪԤ�����ֹ����
int oneflavor_predict(const string &flavor_name, vector<string> times, string train_begin, string train_end, string predict_begin, string predict_end) {
    cout<<"one falovr_predict"<<endl;
	vector<int> flavor_nums;
	vector<int> _y;       //�洢ѵ�����ݱ���
	tm temp_tm;     //��ת�������ֽ�stringת��Ϊ�ñ�����ת��Ϊtms
	int train_end_time, predict_begin_time, predict_end_time;   //��Ϊ����x����
	temp_tm.tm_hour = 0;
	temp_tm.tm_min = 0;
	temp_tm.tm_sec = 0;
	temp_tm.tm_isdst = 0;
	temp_tm.tm_year = atoi(train_begin.c_str()) - 1900;
	temp_tm.tm_mon = atoi(train_begin.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(train_begin.c_str() + 8);
	time_t first_day = mktime(&temp_tm);       //��ʾԤ�⿪ʼ�ĵ�һ��   
											   //ѵ����������
	temp_tm.tm_year = atoi(train_end.c_str()) - 1900;
	temp_tm.tm_mon = atoi(train_end.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(train_end.c_str() + 8);
	train_end_time = (mktime(&temp_tm) - first_day) / 86400 + 1;  //Ԥ�����ʱ�� ,����ת������x����
																  //Ԥ�⿪ʼ����
	temp_tm.tm_year = atoi(predict_begin.c_str()) - 1900;
	temp_tm.tm_mon = atoi(predict_begin.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(predict_begin.c_str() + 8);
	predict_begin_time = (mktime(&temp_tm) - first_day) / 86400 + 1;  //Ԥ�����ʱ�� ,����ת������x����
																	  //Ԥ���������
	temp_tm.tm_year = atoi(predict_end.c_str()) - 1900;
	temp_tm.tm_mon = atoi(predict_end.c_str() + 5) - 1;
	temp_tm.tm_mday = atoi(predict_end.c_str() + 8);
	predict_end_time = (mktime(&temp_tm) - first_day) / 86400 + 1;  //Ԥ�����ʱ�� ,����ת������x����

	for (int i = 0;i != train_end_time;i++)   //��ʼ��Ϊȫ0
		_y.push_back(0);
	//_y[0] = train_end_time;           //0�±겻������ ���ڼ�¼�����ݵ�����
	for (auto str : times) {
		temp_tm.tm_year = atoi(str.c_str()) - 1900;
		temp_tm.tm_mon = atoi(str.c_str() + 5) - 1;    //+5+8�ǽ�ָ�����
		temp_tm.tm_mday = atoi(str.c_str() + 8);
		_y[(mktime(&temp_tm) - first_day) / 86400]++;
		//cout << str << " "<<  endl;
	}

	//	cout << train_end_time << " "<<predict_begin_time<< " "<<predict_end_time << endl;

	//ȥ���쳣��
	double average_normal = 0.0, normal_num = 0.0;
	for (int i = 1;i != train_end_time;i++)
		if (_y[i] != 0) {
			average_normal += _y[i];
			normal_num++;
		}
	average_normal = average_normal / normal_num;   //��ƽ��ֵ
	for (int i = 1;i != train_end_time;i++) {
		if (_y[i] > 5 * average_normal)
			_y[i] = average_normal + 1;
	}

	//����������
	for (int i = 1;i != train_end_time;i++)
 		_y[i] += _y[i - 1];
	//��һ��ƽ��

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

//������Գ���  �����Ľ�Ԥ�⵽������� һ��һ��ѹ��������У���һ������������תս��һ����
//�������result_vm���涼�Ƿֿ��ģ�ÿ��vm���count����
//��һ��������ʾҪ����ķ����� ���ڶ���������ʾԤ����



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
        sort(predict_vm.begin(), predict_vm.end(), Compare_mem());   //������
    
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

//��Ҫ��ɵĹ��������
//info��������Ϣ��������������������Ϣ֮��ģ���input�ļ���  ��data��ѵ�����ݣ� data_num�������ݵ����� data[k] �����k������ ��Ҳ���ǵ�k�������
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	/*-------------------------��������һ������info---------------------*/

	//vector<double> test_iv = { 225.2,249.9,263.2,293.6,318.9,356.7,363.3,424.2,466.5,582.0,750.0 };
	//cout << third_Exponential_Smoothing(test_iv, 2);
    cout <<"11111111111111111  "<<endl;
	int s1, s2, s3;
	istringstream is(info[0]);
	is >> s1 >> s2 >> s3;
	Server server(s1, s2, s3);       //ÿ̨������������
cout<<"duancuowu"<<endl;
	//���������ҪԤ���������浽һ��vm���������ȥ������ʹ��
	vector<VirtualMachine> need_to_pridect;     //ҪԤ������������
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
	string train_begin;              //ѵ����ʼʱ��
	string train_end;               //ѵ������ʱ��
	string predict_begin = info[i + 3];    //Ԥ�⿪ʼʱ��
	string predict_end = info[i + 4];     //Ԥ�����ʱ��
	/*-----------------------------------------------------------------------*/
	/*-------------------------�������ڶ�������data---------------------*/
	map<string, vector<string>> train_vms;     //��ʾ���������񵽸ù������ʱ�伯�ϵ�ӳ�䣬Ϊѵ������
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

	/*-------------------------ʵ��Ԥ��׶�---------------------*/
	vector<VirtualMachine> predict_vms;  //Ԥ���������������
	map<string, int> predict_vms2;  //��ʾ��������������ӳ��
	int total_num=0;   //Ԥ��õ�������

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
	/*-------------------------���������׶�---------------------*/
	//��������Ϊһ��vector,������ÿ��Ԫ��Ϊһ��map���������������Լ�������ӳ�䡣
	vector<map<string, int> > result = distribute_new(server, predict_vms, prepare_to_optimization);
	char p[2000] = {0};        //�洢���
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


	// ��Ҫ���������
	//char * result_file = (char *)"17\n\n0 8 0 20";    //�����н��д��һ��ָ����

	// ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
	write_result(result_file, filename);
}
