#include<iostream>
#include<vector>
#include<fstream>
#include<iomanip>
#include<sstream>
using namespace std ;

struct p
{
    int num ;
    float burst ;
};

int main()
{
    vector<p> data ;
    p temp ;
    int process = 0 ;
    float wait = 0 , turn = 0 , avg_w = 0 , avg_t = 0;

    ifstream infile("Q1.txt" , ios::in) ;
    infile >> process ;
    for(int i = 0; i < process ; i ++)
    {
        temp.num = i + 1 ;
        infile >> temp.burst ;
        data.push_back(temp) ;
    }

    //cout << "Process Waiting Time Turnaround Time" << endl ;
    cout<<std::left<<setw(8)<<"Process"<<setw(12)<<"Waitingtime"<<setw(15)<<"Turnaroundtime"<<endl;
    for(int i = 0 ; i < process ; i ++)
    {
        stringstream s ;
        s << "P[" << i + 1 << "]" ;
        turn += data[i].burst ;
        avg_t += turn ;
        cout <<std::left << setw(8) << s.str() << setw(12) << wait << setw(15) << turn << endl;
        //cout <<  "P[" << i + 1 << left << setw(5) <<"]" << left << setw(11) << wait << left << setw(15) << turn << endl ;
        //cout<<std::left <<"P["  <<i + 1<<setw(7)<< "]"<<setw(11)<<wait<<setw(15)<<turn<<endl;
        if(i > 0) avg_w += wait ;
        wait += data[i].burst ;

        //cout << avg_w << " " << avg_t << endl ;
    }
    cout << endl ;
    cout << "Average waiting time : " << setprecision(7) << avg_w / process << endl ;
    cout << "Average turnaround time : " << setprecision(7) << avg_t / process << endl ;

    data.clear() ;





}
