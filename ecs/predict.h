#pragma once
#ifndef __ROUTE_H__
#define __ROUTE_H__
#include<iostream>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include <stdio.h>
#include <sstream>
#include<algorithm>
#include<string>
#include"unistd.h"
#include "lib_io.h"
#include"time.h"
#include "lib_time.h"


class Server;
class VirtualMachine;
using namespace std;
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename);
vector<map<string, int>> distribute(const Server &s, vector<VirtualMachine> &result_vm,string prepare_to_optimization);
int oneflavor_predict(const string &flavor_name, vector<string> times, string train_begin, string train_end, string predict_begin, string predict_end);
double third_Exponential_Smoothing(vector<double>  iv, int predice_T);

vector<map<string, int> > distribute_new(const Server &s, vector<VirtualMachine> &predict_vm, string prepare_to_optimization);




class Server {
public:
	Server() = default;
	Server(int i , int j , int k ) : cpu(i), memory(j), disk_cap(k) {}
	int get_cpu() const { return cpu; }
	int get_memory() const { return memory; }
	int get_disk_cap() const { return disk_cap; }
private:
	int cpu;
	int memory;
	int  disk_cap;
};

class VirtualMachine {
public:
	VirtualMachine()=default;
	VirtualMachine(string s, int i, int j,int k = 1) : name(s), cpu(i), memory(j), count(k) {}
	string get_name()const { return name; }
	int get_memory()const { return memory; }
	int get_cpu()const { return cpu; }
private:
	string name;
	int cpu;
	int memory;
	int count;
};



#endif
