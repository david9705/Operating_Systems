#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#define INPUT "Q4.txt"

using namespace std;

int num;
struct data *job;
int quan1 = 0, quan2 = 0;
int t = 0;
struct data *choice;

struct data { //創建儲存process data的資料結構
	int arrive;
	int burst;
	int remain;
	int level = 1;
	bool tak = false;
	int W_T;
	int T_T;
};

void first_level();
void second_level();
int third_level();

void first_level(){ //first level: RR
	for (int i = 0; i < num; i++) {
		if (job[i].arrive <= t && job[i].remain > 0 && job[i].level == 1 && job[i].tak == false) {
			if (job[i].remain > quan1) {
				job[i].remain -= quan1;
				t += quan1;
			}
			else{
				t += job[i].remain;
				job[i].remain = 0;
				job[i].tak = true;
				job[i].W_T = t - job[i].burst - job[i].arrive;
		        job[i].T_T = t - job[i].arrive;
			}
			job[i].level++;
		}
	}
	for (int i = 0; i < num; i++) //check first level is empty
		if (job[i].level == 1) {
			first_level();
			exit(0);
		}
	second_level(); //enter second level
}

void second_level(){ //second level: RR
	for (int i = 0; i < num; i++) {
		if (job[i].arrive <= t && job[i].remain > 0 && job[i].level == 2 && job[i].tak == false) {
			if (job[i].remain >= quan2) {
				job[i].remain -= quan2;
				t += quan2;
			}
			else{
				t += job[i].remain;
				job[i].remain = 0;
				job[i].tak = true;
				job[i].W_T = t - job[i].burst - job[i].arrive;
		        job[i].T_T = t - job[i].arrive;
			}
			job[i].level++;
			for (int i = 0; i < num; i++) //check first level is empty
				if (job[i].level == 1) {
					first_level();
					exit(0);
				}
		}
	}
	for (int i = 0; i < num; i++) //check second level is empty
		if (job[i].level == 2 && job[i].tak == false) {
			second_level();
			exit(0);
		}
	third_level(); //enter third level
}

int third_level(){ //third level: SJF
	while(1) {
		choice = &job[0];
		for (int j = 0; j < num; j++)
			if (job[j].arrive <= t && job[j].burst < (choice->burst) && job[j].level == 3 && job[j].tak == false)
		        choice = &job[j];
		(choice->tak) = true;
		t += (choice->remain);
		(choice->remain) = 0;
		(choice->W_T) = t - (choice->burst) - (choice->arrive);
		(choice->T_T) = t - (choice->arrive);

		for (int i = 0; i < num; i++) //check first level is empty
			if (job[i].level == 1 && job[i].tak == false) {
				first_level();
				exit(0);
			}
		for (int i = 0; i < num; i++) //check second level is empty
			if (job[i].level == 2 && job[i].tak == false) {
				second_level();
				exit(0);
			}

		for (int i = 0; i < num; i++) { //check every process is done
			if (job[i].tak == false)
			    break;
			else
			    ;
			if (i == num - 1)
			    return 0;
		}
	}
}

int main()
{
	FILE *fp = fopen(INPUT, "r"); //開啟檔案
	fscanf(fp, "%d\n",&num); //從檔案讀取資料，取得process數量
	job = new struct data[num];
	int total_WT = 0, total_TT = 0;
	double avg_WT = 0, avg_TT = 0;
	for (int i = 0; i < num; i++) //取得每個process的arrive time
		fscanf(fp, "%d ", &job[i].arrive);
	for (int i = 0; i < num; i++) { //取得每個process的burst time
		fscanf(fp, "%d ", &job[i].burst);
		job[i].remain = job[i].burst;
	}
	fscanf(fp, "%d ", &quan1); //取得first level RR的quantum time
	fscanf(fp, "%d ", &quan2); //取得second level RR的quantum time
	first_level(); //enter first level

	for (int i = 0; i < num; i++) {
		total_WT += job[i].W_T; //total waiting time calculate
		total_TT += job[i].T_T; //total turnaround time calculate
	}
	avg_WT = (double)total_WT/num; //average waiting time
	avg_TT = (double)total_TT/num; //average turnaround time

	cout << "Process\tWaiting Time\tTurnaround Time" << endl;
	for (int i = 0; i < num; i++)
	    cout << "P[" << i << "]\t" << job[i].W_T << "\t\t" << job[i].T_T << endl;

	cout << "\nAverage waiting time : " << avg_WT << "\nAverage turnaround time : " << avg_TT << endl;

	return 0;
}
