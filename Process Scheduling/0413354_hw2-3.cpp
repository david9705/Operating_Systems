#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#define INPUT "Q3.txt"

using namespace std;

struct data { //�Ы��x�sprocess data����Ƶ��c
	int arrive;
	int remain;
	int burst;
	bool tak = false;
	int W_T;
	int T_T;
};

int main()
{
	FILE *fp = fopen(INPUT, "r"); //�}���ɮ�
	int num;
	fscanf(fp, "%d\n",&num); //�q�ɮ�Ū����ơA���oprocess�ƶq
	struct data *job = new struct data[num];
	int time = 0;
	struct data *choice;
	int total_WT = 0, total_TT = 0;
	double avg_WT = 0, avg_TT = 0;
	for (int i = 0; i < num; i++) { //���o�C��process��arrive time
		fscanf(fp, "%d ", &job[i].arrive);
	}
	for (int i = 0; i < num; i++) { //���o�C��process��burst time
		fscanf(fp, "%d ", &job[i].burst);
		time += job[i].burst;
		job[i].remain = job[i].burst;
	}
	for (int i = 0; i < time;) { //SRTF
		choice = &job[0];
		int t = 0;
		for (int j = 0; j < num; j++)
			if (job[j].arrive <= i && job[j].remain < choice->remain && job[j].tak == false) {
				choice = &job[j];
		        t = j;
		    }
		i++;
		if (--choice->remain == 0) {
			choice->tak = true;
			choice->W_T = i - choice->burst - choice->arrive;
			choice->T_T = i - choice->arrive;
			choice->remain = time;
	    }
	}

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
