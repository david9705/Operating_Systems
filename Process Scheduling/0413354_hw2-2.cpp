#include<iostream>
#include<vector>
#include<fstream>
#include<iomanip>
#include<sstream>
#include<algorithm>
using namespace std ;

struct p
{
    int n ;
    float burst , arr ;
    float wait , turn ;
    bool done ;
};

bool cmp(p a , p b)
{
   if(a.arr == b.arr && a.burst == b.burst)
   {
       return a.n < b.n ;
   }
   else return a.burst < b.burst ;
}
bool cmp1(p a , p b)
{
   //return a.arr < b.arr ;
   if(a.arr == b.arr) return a.burst < b.burst ;
   else return a.arr < b.arr ;

}

int main()
{
    ifstream infile("Q2.txt" , ios::in) ;
    vector<p> data ;
    int num = 0 ;
    float time= 0 ,avg_w = 0 , avg_t = 0 ;
    p temp ;
    infile >> num ;
    for(int i = 0 ; i < num ; i ++)
    {
        temp.n = i + 1 ;
        infile >> temp.arr ;
        data.push_back(temp) ;
    }
    for(int i = 0 ; i < num ; i ++)
    {
        float bur = 0;
        infile >> bur ;
        data[i].burst = bur ;
    }


    sort(data.begin() , data.end() , cmp1) ;
//    cout << data.begin() << " " << data.end() << endl ;
    for(int i = 0 ; i < num ; i ++)
    {cout << data[i].n << endl ;
        int fin = 0 ;
        if(i == 0)
        {
            time = data[i].arr ;
            data[i].wait = 0 ;
        }
        else data[i].wait = time - data[i].arr ;
        time += data[i].burst ;
        data[i].turn = time - data[i].arr ;
        for(int j = i+1 ;j < num ; j ++)
        {
            if(!data[j].done && data[j].arr < time)
            {
                if(j > fin ) fin = j ;
            }
        }
        //cout << fin << endl ;
        if(i != num -1) sort(data.begin()+i+1 , data.begin()+fin , cmp) ;

        data[i].done = 1;

    }

   cout<<std::left<<setw(8)<<"Process"<<setw(12)<<"Waitingtime"<<setw(15)<<"Turnaroundtime"<<endl;
   for(int i = 0 ; i < num ; i ++)
    {
        stringstream s ;
        for(int j = 0 ; j < num ; j ++)
        {
            if(i+1 == data[j].n)
            {
                s << "P[" << data[j].n << "]" ;
                cout <<std::left << setw(8) << s.str() << setw(12) << data[j].wait << setw(15) << data[j].turn << endl;
                avg_t += data[j].turn ;
                avg_w += data[j].wait ;
            }
        }



    }
    cout << endl ;
    cout << "Average waiting time : " << setprecision(7) << avg_w / num << endl ;
    cout << "Average turnaround time : " << setprecision(7) << avg_t / num << endl ;




    data.clear() ;


}
