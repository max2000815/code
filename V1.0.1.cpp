#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <iomanip>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <vector>
using namespace std;
#define MAXPLACES 200               //最大地点数
#define MAXROADS 300                //最大道路数
#define MAXNODES 100                //最大节点数
#define MAXNEIGHBOR 20              //最大附近建筑数
#define MAXNEIGHBORLENGTH 1000      //搜索最近建筑最大距离
#define MAXEVENTS 60                //最大事件数
#define MAXPAST 10                  //等待时间
#define INF 0x3f3f3f3f              //
#define HUMANSPD 200                //行走速度系数
#define BUST 50                     //校车行驶时间（分钟）
#define BUSST 100                   //公交行驶时间（分钟）
#define WAIT 10                     //等待公交的时间（分钟）
class Place         //存储地点信息的类
{
    public:
    int ID;         //地点ID，正常地点ID为1~99的整数，路口ID为100以上的整数
    string Name;    //地点名称**开头不能是数字**
    int Event[8];   //地点事件（以事件ID存储）
    int Road[8];    //地点所连接的道路，以道路ID存储
    void operator=(const Place &P ); //赋值运算符重载
};        
class Road          //存储道路信息的类
{
    public:
    int ID;         //道路ID
    int EndA;       //道路起点A
    int EndB;       //道路终点B
    int Length;     //道路长度
    int Cro;        //道路拥堵权值，为10~99的整数；
    int Dir;        //道路方向，以A到B的方向为准，1~8方向分别为N，NE，E，SE，S ,SW，W，NW
    int Bik = 1;    //道路是否能通过自行车，不能为0，能则为1
};
class Event         //存储事件信息的类
{
    public:
    int ID;         //事件ID
    string Info;    //事件内容
};
class Clock         //时钟类
{
public:
    void SetTime(int newH = 0, int newM = 0, int newS = 0);
    void ShowTime();
    void Run();          //Run()控制计时
    int getminutes();
private:
    int hour, minute, second;
};
Place Places[MAXPLACES];            //存储地图中地点信息
Road Roads[MAXROADS];               //存储地图中道路信息
Event Events[MAXEVENTS];            //存储事件信息
Place * Now = &Places[59];                          //当前位置
Road * TempRoad = &Roads[194];
void PrintInfo();                                                 //输出调试信息
void ReadEvent();                                                 //初始化-读取事件列表
void ReadRoad();                                                  //初始化-读取道路列表
void ReadPlace();                                                 //初始化-读取地点信息列表
void Navigation(int start,int final,int * Path);                  //获得start到final的路径，并存储到Path中 
void Navigationer(int method);                                    //导航系统
void Inquire(int s,int * Inrange);                                //获得s附近所有距离小于MAXNEIGHBORLENGTH的地点并存于Inrange中
void Inquirer();                                                  //查询系统
int LogicF();                                                     //逻辑导航确定终点
void Instruction();                                               //打印地点ID对照表
void InstructionE();                                              //打印事件ID对照表
void PrintPlace(int id);                                          //根据地点ID输出地点名称
void PrintPath(int * Path , int * Table , int is2);               //打印路径
int FindRoads(int s,int f);                                       //返回以s为起点，f为终点的道路ID
void GetTable(int * Path , int *Table);                           //根据Path存储的地点获得路径经过的道路ID，并存储在Table中
string Direction(int rid);                                        //获得对应ID道路的方向  
int Findid(int id);                                               //获得对应ID道路在道路表中的下标
void writeFile(string record);                                    //将信息写入日志文件
int GetTimeM(int * Table);                                        //获得路径所需的总时间
int CanBus(int Ntime,int Ptime);                                  //判断是否能够乘坐校车
void Guide(int * Table, int isbike);                              //根据路径开始导航
void ShowResult(int s,int * result);                              //输出查询结果
int Errorflag = 0;                                                //错误标志
int second1;                                                      //计时器临时变量
string time1;                                                     //时间
Clock Timer;                                                      //计时器
int BusClock[5] = {480,720,780,1080,2520};                        //校车时刻表
int main(void)
{
    ReadPlace();
    ReadRoad();
    ReadEvent();
    //PrintInfo();
    printf("***************************\n");
    printf("***     校园导览系统     ***\n");
    printf("***   输入1进入导航系统  ***\n");
    printf("***   输入2进入查询系统  ***\n");
    printf("***     输入0退出系统    ***\n");
    printf("***************************\n");
    string input;
    int quit_flag = 1;
    Timer.SetTime(8,0,0);
    int meth;
    printf("请输入：");
    while (quit_flag && cin >> input) {
        if (input.length() > 1) {
            printf("非法输入！\n");
            writeFile("用户输入错误");
        }
        switch(input[0]) {
            case '1':
                writeFile("用户进入了导航系统");
                cout << "请输入导航策略（1：最短距离 2：最短时间 3：自行车出行 4：途经多地点路线 5：逻辑导航）：" ;
                cin >> meth;
                Navigationer(meth);
                break;
            case '2':
                writeFile("用户进入了查询系统");
                Inquirer();
                break;
            case '0':
                writeFile("用户推出了程序");
                quit_flag = 0;
                break;
            default:
                writeFile("用户输入错误");
                printf("非法输入！\n");
                break;
        }
        if (quit_flag) {
            printf("***************************\n");
            printf("***     校园导览系统     ***\n");
            printf("***   输入1进入导航系统  ***\n");
            printf("***   输入2进入查询系统  ***\n");
            printf("***     输入0退出系统    ***\n");
            printf("***************************\n");
            printf("请输入：");
        }
    }
    // PrintInfo();
}

void PrintInfo()
{
    cout << "Places:" << endl;
    for(int i = 0;i < MAXPLACES;i ++)
    { 
        if(Places[i].ID != 0)
        {
        cout << "ID " << Places[i].ID << " " << Places[i].Name << " ";
        for(int j = 0; j < 8;j ++)
        {
            cout << Places[i].Event[j] << " ";

        }
        for(int j = 0; j < 8;j ++)
        {
            cout << Places[i].Road[j] << " ";
            
        }
        cout << endl;
        }
    }
    cout << "Roads" << endl;
    for(int i = 0;i < MAXPLACES;i ++)
    {
        if(Roads[i].ID != 0)
        {
            cout << "ID " << Roads[i].ID << " "  << Roads[i].EndA << " " << Roads[i].EndB << " " << Roads[i].Length << " " << Roads[i].Cro << " " << Roads[i].Dir << endl;
        }
    }
}

void ReadEvent()
{
    ifstream EventFile("EventFile.txt");
    int number = 0;
    int temp = 0;
    string buffer = "\0";
    int k = 0;
    while (getline(EventFile,buffer))
    {
        Events[k].ID = stoi(buffer);
        getline(EventFile,buffer);
        Events[k].Info = buffer;
        k++;
    }
    
}

void ReadRoad()
{
    ifstream RoadFile("RoadFile.txt");
    int number = 0;
    int temp = 0;
    char buf;
    char lbuf;
    char text[20];
    for(int k = 0; k < MAXROADS; k ++)
    {
        while (RoadFile.get(buf))
        {
            if((buf != '\n') && (buf != ' '))
            {
                lbuf = buf;
                if ('0' <= buf && buf <= '9')
                {
                    number = 10 * number + (buf - '0');
                }
                else
                {
                    text[temp] = buf;
                    temp ++;
                }
            }
            else if ('0' <= lbuf && lbuf <= '9')
            {
                if (number != 0)
                {
                    if (Roads[k].ID == 0)
                    {
                        Roads[k].ID = number;
                    }
                    else if (Roads[k].EndA == 0)
                    {
                        Roads[k].EndA = number;
                    }
                    else if (Roads[k].EndB == 0)
                    {
                        Roads[k].EndB = number;
                    }
                    else if (Roads[k].Length == 0)
                    {
                        Roads[k].Length = number;
                    }
                    else if (Roads[k].Cro == 0)
                    {
                        Roads[k].Cro = number;
                    }
                    else if (Roads[k].Dir == 0)
                    {
                        Roads[k].Dir = number;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
                number = 0;
            }
            else 
            break;
        }
    }
}

void ReadPlace()
{
    ifstream PlaceFile("PlaceFile.txt");
    int number = 0;
    int temp = 0;
    int isEread = 0;
    int e = 0;
    int r = 0;
    char buf;
    char lbuf;
    char text[200];
    for (int i = 0;i < 200;i ++)
    text[i] = '\000';
    for(int k = 0; k < MAXPLACES; k ++)
    {
        e = 0;
        r = 0;
        while (PlaceFile.get(buf))
        {
            if((buf != '\n') && (buf != ' '))
            {
                lbuf = buf;
                if ('0' <= buf && buf <= '9' && text[0] == '\000')
                {
                    number = 10 * number + (buf - '0');
                }
                else
                {
                    text[temp] = buf;
                    temp ++;
                }
            }
            else if ('0' <= lbuf && lbuf <= '9' && text[0] == '\000')
            {
                if (number != 0)
                {
                    if (Places[k].ID == 0)
                    {
                        Places[k].ID = number;
                    }
                    else if (isEread == 0)
                    {
                        Places[k].Event[e] = number;
                        e ++ ;
                    }
                    else
                    {
                        Places[k].Road[r] = number;
                        r ++;
                    }
                }
                else
                {
                    isEread = 1;    
                } 
                number = 0;
            }
            else
            {
                Places[k].Name = text;
                for (int i = 0;i < 200;i ++)
                text[i] = '\0';
                temp = 0;
                isEread = 0;
                break;
            }
        }
    }
}

void Navigation(int start,int final,int * Path)                         //获得start到final的路径，并存储到Path中
{
    int shortest_path[500];
    int path[500];
    int k = 1;
    int p = 0;
    //求解从s出发到所有点的最短距离,O(m * n),无负圈
        for (int i = 0; i < 500; i ++) {
        	shortest_path[i] = INF;
        	path[i] = INF;
		}
            shortest_path[start] = 0;
            while (true) {
                bool update = false;
                for (int i = 0; i < MAXROADS && Roads[i].ID != 0; i ++) {
                Road road = Roads[i];
                if ((shortest_path[road.EndA] != INF) && (shortest_path[road.EndB] > shortest_path[road.EndA] + road.Length)) {
                path[road.EndB] = road.EndA;
                shortest_path[road.EndB] = shortest_path[road.EndA] + road.Length;
                update = true;
                }
            }
        if (!update) break;
	}  
    stack <int> st;
    while(final && final != start)
    {
    	st.push(final);
    	final = path[final];
	}
	Path[p] = final;
    p++;
	while (!st.empty()) {
		int next;
		next = st.top();
		st.pop();
		Path[p] = next;
        p++;
	}
}

void bike_Navigation(int start, int final, int *Path)
{
    Road roads[MAXROADS];
    for(int i = 0; i < MAXROADS; i++)
    {
        roads[i].Bik = Roads[i].Bik;
        roads[i].Cro = Roads[i].Cro;
        roads[i].Dir = Roads[i].Dir;
        roads[i].EndA = Roads[i].EndA;
        roads[i].EndB = Roads[i].EndB;
        roads[i].ID = Roads[i].ID;
        roads[i].Length = Roads[i].Length;
    }
    for(int i = 0; i < MAXROADS; i ++)
    {
        if(roads[i].Bik == 0)
        {
            roads[i].Length = INF;
        }
        else
        {
            roads[i].Length= Roads[i].Length * Roads[i].Cro;
        }
    }
    int shortest_path[500];
    int path[500];
    int k = 1;
    int p = 0;
    //求解从s出发到所有点的最短距离,O(m * n),无负圈
    for (int i = 0; i < 500; i ++) 
    {
        shortest_path[i] = INF;
        path[i] = INF;
	}
    shortest_path[start] = 0;
    while (true) 
    {
        bool update = false;
        for (int i = 0; i < MAXROADS && roads[i].ID != 0; i ++) 
        {
            Road road = roads[i];
            if ((shortest_path[road.EndA] != INF) && (shortest_path[road.EndB] > shortest_path[road.EndA] + road.Length)) 
            {
                path[road.EndB] = road.EndA;
                shortest_path[road.EndB] = shortest_path[road.EndA] + road.Length;
                update = true;
            }
        }
        if (!update) break;
	}  
    stack <int> st;
    while(final && final != start)
    {
    	st.push(final);
    	final = path[final];
	}
	Path[p] = final;
    p++;
	while (!st.empty()) 
    {
		int next;
		next = st.top();
		st.pop();
		Path[p] = next;
        p++;
	}
}

void shortesttime(int start, int final, int *Path){

Road roads[MAXROADS];
for(int i = 0; i < MAXROADS; i++){
roads[i].Bik = Roads[i].Bik;
roads[i].Cro = Roads[i].Cro;
roads[i].Dir = Roads[i].Dir;
roads[i].EndA = Roads[i].EndA;
roads[i].EndB = Roads[i].EndB;
roads[i].ID = Roads[i].ID;
roads[i].Length = Roads[i].Length;
}
for(int i = 0; i < MAXROADS; i ++){
    roads[i].Length= Roads[i].Length * Roads[i].Cro;
}
 int shortest_path[500];
    int path[500];
    int k = 1;
    int p = 0;
    //求解从s出发到所有点的最短距离,O(m * n),无负圈
        for (int i = 0; i < 500; i ++) {
        	shortest_path[i] = INF;
        	path[i] = INF;
		}
            shortest_path[start] = 0;
            while (true) {
                bool update = false;
                for (int i = 0; i < MAXROADS && roads[i].ID != 0; i ++) {
                Road road = roads[i];
                if ((shortest_path[road.EndA] != INF) && (shortest_path[road.EndB] > shortest_path[road.EndA] + road.Length)) {
                path[road.EndB] = road.EndA;
                shortest_path[road.EndB] = shortest_path[road.EndA] + road.Length;
                update = true;
                }
            }
        if (!update) break;
	}  
    stack <int> st;
    while(final && final != start)
    {
    	st.push(final);
    	final = path[final];
	}
	Path[p] = final;
    p++;
	while (!st.empty()) {
		int next;
		next = st.top();
		st.pop();
		Path[p] = next;
        p++;
	}
}

void ShortestMultiplePath(int u, int n, int a[], int dis[200][200], vector <int> & path) {

    bool ok[n + 10];
    for (int i = 0; i < n + 10; i ++) ok[i] = false;
    while (path.size() != n - 1) 
    {
        int minn = INF, minp;
        for (int i = 0; i <n - 1; i ++)
         {
            if (ok[i]) continue;
            if (dis[u][n] < minn) 
            {
                minn = dis[u][n];
                minp = i;
            }
        }
        path.push_back(a[minp]);
        u = a[minp];
        ok[minp] = true;
    }
    path.push_back(a[n - 1]);
}

void past(int * Path)
{
    int minall[MAXPLACES][MAXPLACES];
    for (int i = 0; i < MAXPLACES; i ++) 
    {
            for (int j = 0; j < MAXPLACES; j ++) {
                minall[i][j] = INF;
                if (i == j) minall[i][j] = 0;
            }
        }
        for (int i = 0; i < MAXROADS; i ++) {
            minall[Roads[i].EndA][Roads[i].EndB] = Roads[i].Length;
        }
     for (int k = 1; k < MAXPLACES; k ++) 
     {
            for (int i = 1; i < MAXPLACES; i ++) 
            {
                for (int j = 1; j < MAXPLACES; j ++) 
                {
                    minall[i][j] = min(minall[i][j], minall[i][k] + minall[k][j]);
                }
            }
        }
        int start;
        int n;
        cout << "请输入途经地点数（不包括起点）: ";
        cin >> n;
        printf("请输入起点：");
        scanf("%d", &start);
        int a[n];
        printf("请输入途经的所有地点，最后一个为终点：");
        for(int i = 0; i < n; i++)
        {
            scanf("%d", &a[i]);
        }
        vector <int> path;
        ShortestMultiplePath(start,n,a,minall,path);
        int m = 0;
        Path[0] = start;
        int Path1[MAXPLACES] ={0};
        int l = 0;
        Navigation(start,path[0],Path1);
        for(int k = 0;Path1[k] != 0;k++)
        {
            Path[l]=Path1[k];
            l++;
        }
        for(int i = 0 ;i<n-1;i++)
        {
            l--;
            for(int k = 0;k<200;k++)
            {
                Path1[k] = 0;
            }
            Navigation(path[i],path[i+1],Path1);
            for(int k = 0;Path1[k] != 0;k++)
            {
                Path[l]=Path1[k];
                l++;
            }
        }   
}

void Navigationer(int method)
{
    int start,final,ByCar,bust;
    int path1[MAXPLACES] = {0},path2[MAXPLACES] = {0};
    int table1[MAXROADS] = {0},table2[MAXROADS] = {0};
    int Rtime1,Rtime2;
    int Bus;
    Instruction();
    if(method != 4 && method != 5)
    {
        cout << "请输入您当前的位置ID：" ;
        cin >> start;
        cout << "请输入您的目的地位置ID：";
        cin >> final;
    }
    else if(method == 5)
    {
        cout << "请输入您当前的位置ID：" ;
        cin >> start;
        final = LogicF();
    }
    else
    {
        past(path1);
        start = path1[0];
        int f;
        for(int i = 0; path1[i] != 0;i++)
        {
            f = path1[i];
        }
        final = f;
    }
    string logger = "用户将导航起点设置为 " + Places[start-1].Name + "，将导航终点设置为 " + Places[final-1].Name ;
    writeFile(logger);
    if((start <= 25 && final <= 25 )||(start > 25 && final > 25))
        {
            if(method == 1 )
            Navigation(start, final, path1);
            else if(method == 2 || method == 5)
            shortesttime(start,final,path1);
            else if(method == 3)
            bike_Navigation(start,final,path1);
            GetTable(path1,table1);
            PrintPath(path1,table1,0);
            Rtime1 = GetTimeM(path1);
            writeFile("开始导航");
            if(method != 3)
            Guide(table1,0);
            else
            Guide(table1,1);
            cout << "您已到达导航目标，欢迎下次使用！" << endl ;
            writeFile("完成了导航目标");
            exit(0);           
        }
        else
        {
            if(start > 25)
            {
                if(method == 1 )
                {
                    Navigation(start, 26, path1);
                    Navigation(1, final , path2);
                }
                else if(method == 2 || method == 5)
                {
                    shortesttime(start, 26, path1);
                    shortesttime(1, final , path2);
                }
                else if(method == 3)
                {
                    bike_Navigation(start, 26, path1);
                    bike_Navigation(1, final , path2);
                }
                GetTable(path1,table1);
                GetTable(path2,table2);
                Rtime1 = GetTimeM(path1);
                Rtime2 = GetTimeM(path2);
            }
            else
            {
                if(method == 1 )
                {
                    Navigation(start, 1, path1);
                    Navigation(26, final , path2);
                }
                else if(method == 2 || method == 5)
                {
                    shortesttime(start, 1, path1);
                    shortesttime(26, final , path2);
                }
                else if(method == 3)
                {
                    bike_Navigation(start, 1, path1);
                    bike_Navigation(26, final , path2);
                }
                GetTable(path1,table1);
                GetTable(path2,table2);
                Rtime1 = GetTimeM(path1);
                Rtime2 = GetTimeM(path2);
            }
            if((bust=CanBus(Timer.getminutes(),Rtime1)) == -1)
            {
                ByCar = 1;
            }
            else
            {
                ByCar = 0;
            }
            writeFile("开始导航");
            if(method != 3)
            Guide(table1,0);
            else
            Guide(table1,1);
            if(ByCar == 1)
            {
                cout << "您需要乘坐校车 " << endl;
                while((BusClock[bust]-(Timer.getminutes()))+BUST != 0)
                {
                    cout << "预计校车将在 " << (BusClock[bust]-(Timer.getminutes()))+BUST << " 分后到达另一个校区" << endl;
                    Timer.Run();
                }

            }
            else
            {
                int t = BUSST + WAIT;
                cout << "您需要乘坐公交车" << endl;
                while(t != 0)
                {
                    cout << "预计公交车将在 " << t << " 分后到达另一个校区" << endl;
                    Timer.Run();
                    t --;
                }
            }
            cout << "您已到达另一个校区 " << endl;
            if(method != 3)
            Guide(table2,0);
            else
            Guide(table2,1);
            cout << "您已到达导航目标，欢迎下次使用！" << endl;
            writeFile("完成了导航目标");
            exit(0);
        }

}

void Inquire(int s,int * Inrange) {
    int shortest_path[500];
    int r = 0;
    int neighbors[MAXNEIGHBOR]; //求解从s出发到所有点的最短距离,O(m * n),无负圈
    for (int i = 0; i < 500; i ++) {
        shortest_path[i] = INF;
        }
        shortest_path[s] = 0;
        while (true) {
            bool update = false;
            for (int i = 0; (i < MAXROADS && Roads[i].ID != 0); i ++) { 
            Road road = Roads[i];
            if (shortest_path[road.EndA] != INF && shortest_path[road.EndB] > shortest_path[road.EndA] + road.Length) {
            shortest_path[road.EndB] = shortest_path[road.EndA] + road.Length;
            update = true;
        	}
        }
        if (!update) break;
    }
    for (int i = 0; i < MAXNODES; i ++) {
        if (shortest_path[i] <= MAXNEIGHBORLENGTH)
        {
            Inrange[r] = i ;
            r++;
        }
    }
}

void Inquirer()
{
    Instruction();
    cout << "请输入您要查询的位置ID：";
    int s;
    cin >> s;
    int result[MAXNEIGHBOR] = {0};
    string logger = "用户查询了距离 " + Places[s-1].Name + " " + to_string(MAXNEIGHBORLENGTH) + " 米的建筑物";
    writeFile(logger);
    Inquire(s,result);
    ShowResult(s,result);
}

int LogicF()
{
    InstructionE();
    cout << "请输入需要查询的事件ID：";
    int a;
    cin >> a;
    int i,k=0;
    int sResult[50] = {0};
    for(i = 0; i < MAXPLACES && Places[i].ID!=0; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(Places[i].Event[j] == a)
            {
                sResult[k] = Places[i].ID;
                k ++;
                break;
            }
        }
    }
    cout << "查询到相关地点：" << endl;
    for(i = 0; i < 20 && sResult[i] != 0;i++)
    {   
        cout << i+1 << " : ";
        PrintPlace(sResult[i]);
        cout << endl;
    }
    cout << "请选择您要前往的地点：" ;
    cin >> a;
    return sResult[a-1];
}

void Instruction()
{
    printf("地点ID对照表\n");
    for(int i = 0 ; i < MAXPLACES && Places[i].ID != 0 ; i++)
    {
        cout << "ID : " << Places[i].ID << "    地点: " << Places[i].Name << endl; 
    }
}

void InstructionE()

{
    cout << "事件ID对照表" << endl;
    for(int i = 0 ; i < MAXEVENTS && Events[i].ID != 0 ; i++)
    {
        cout << "ID : " << Events[i].ID << "    地点: " << Events[i].Info << endl; 
    }
}

string PrintDirection(int rid)
{
    int t = Roads[Findid(rid)].Dir;
    switch(t)
    {
        case 1:
            return "北";
            break;
        case 2:
            return "东北";
            break;
        case 3:
            return "东";
            break;
        case 4:
            return "东南";
            break;
        case 5:
            return "南";
            break;
        case 6:
            return "西南";
            break;
        case 7:
            return "西";
            break;
        case 8:
            return "西北";
            break;
        default:
            Errorflag = 1;
            break;

    }
}

void PrintPlace(int id)
{
    cout << Places[id-1].Name;
}

int FindRoads(int s,int f)
{
    int i = 0;
    for(i = 0; i < MAXROADS; i++)
    {
        if(Roads[i].EndA == s && Roads[i].EndB == f)
        return Roads[i].ID;
    } 
}

void GetTable(int * Path , int *Table)
{
    int i;
    for(i = 0;i < MAXPLACES && Path[i + 1] != 0 ; i++)
    {
        Table[i] = FindRoads(Path[i],Path[i+1]);
    }
}

void PrintPath(int * Path , int * Table , int is2)
{   
    int i;
    if(is2 == 0);
    cout << "系统为您规划的路线为：";
    for(int i = 0; Path[i+1] != 0; i++)
    {
        cout << "向" << PrintDirection(Table[i]) << "走" <<Roads[Findid(Table[i])].Length<< "米, 到达";
        PrintPlace(Path[i+1]);
        cout << endl << "预计用时" << (Roads[Findid(Table[i])].Length*Roads[Findid(Table[i])].Cro/10/HUMANSPD) <<"分" << endl;
    }
    cout << "需要的总时间为： " << GetTimeM(Table) << endl;
}

int Findid(int id)
{
    int i;
    for(i = 0; i < MAXROADS;i++)
    {
        if(Roads[i].ID == id)
        return i;
    }
}

int GetTimeM(int * Table)
{
    int i;
    int time = 0;
    for(i = 0 ; Table[i]!=0 ; i ++)
    {
        time += (Roads[Findid(Table[i])].Length*Roads[Findid(Table[i])].Cro/10/HUMANSPD);
    }
    return time;
}

int CanBus(int Ntime,int Ptime)
{
    int i;
    for(i = 0;i < 5;i++)
    {
        if((BusClock[i]-(Ntime + Ptime)) <= (BUSST - BUST + WAIT))
        return i;
    }
    if (i == 5)
    return -1;
}

void Guide(int *Table,int isbike)
{
    cout << "开始导航" << endl;

    int i = 0;
    int stopkey = 0;
    for(i = 0;Table[i] != 0; i++)
    {
        int nextflag = 0;
        Now->ID = 60;
        Now->Name = "用户";
        Now->Road[0] = 198;
        Now->Road[1] = 199;
        TempRoad->EndA = Roads[Findid(Table[i])].EndA;
        TempRoad->EndB = 60;
        TempRoad->ID = 198;
        TempRoad->Dir = ((Roads[Findid(Table[i])].Dir+4)%8+1*((Roads[Findid(Table[i])].Dir+4)/8));
        TempRoad->Length = 0;
        TempRoad->Bik = Roads[Findid(Table[i])].Bik;
        TempRoad->Cro = Roads[Findid(Table[i])].Cro;
        (TempRoad+1)->EndA = 60;
        (TempRoad+1)->EndB = Roads[Findid(Table[i])].EndB;
        (TempRoad+1)->ID = 199;
        (TempRoad+1)->Length = Roads[Findid(Table[i])].Length;
        (TempRoad+1)->Dir = Roads[Findid(Table[i])].Dir;
        (TempRoad+1)->Bik = Roads[Findid(Table[i])].Bik;
        (TempRoad+1)->Cro = Roads[Findid(Table[i])].Cro;
        string logger;
        logger = "用户当前位于：" + Places[Roads[Findid(Table[i])].EndA-1].Name;
        writeFile(logger);
        cout << "您当前位于：" << Places[Roads[Findid(Table[i])].EndA-1].Name << endl;
        cout << "向 " << PrintDirection(Table[i]) << " 前进 " << (TempRoad+1)->Length << " 米" << "到达 " << Places[Roads[Findid(Table[i])].EndB-1].Name << endl;
        while(1)
        {
            Timer.Run();
            stopkey ++;
            TempRoad->Length += (HUMANSPD * (1 + 2 * isbike) / TempRoad->Cro * 10);
            (TempRoad+1)->Length -= (HUMANSPD * (1 + 2 * isbike) / TempRoad->Cro * 10);
            if((TempRoad+1)->Length <= 0)
            {
                (TempRoad+1)->Length = 0;
                TempRoad->Length = Roads[Findid(Table[i])].Length;
            }
            if((TempRoad+1)->Length > 0)
            {
                cout << "您当前位置到下个目标地点 " << Places[Roads[Findid(Table[i])].EndB-1].Name << " 的距离为 " << (TempRoad+1)->Length << "米。" << endl;
                cout << "预计 " << ((TempRoad+1)->Length / (HUMANSPD * (1 + 2 * isbike) / TempRoad->Cro * 10)) << " 分钟后到达下一个目标点。" << endl; 
            }
            else
            {
                cout << "您已到达目标地点 " << Places[Roads[Findid(Table[i])].EndB-1].Name  << endl;
                if(Table[i + 1] != 0)
                cout << "下个目标地点为 " << Places[Roads[Findid(Table[i + 1])].EndB-1].Name << endl;
                nextflag = 1;
            }
            if(nextflag == 1)
            {
                break;
            }
            if(kbhit())
            {
                int a = 0;
                getchar();
                getchar();
                cout << "导航中断" << endl;
                logger = "用户中断了导航";
                writeFile(logger);
                cout << "您需要做什么？（1：查询周围建筑    2: 继续导航）";
                cin >> a;
                int list[MAXNEIGHBOR] = {0};
                if(a == 1)
                {
                    Inquire(60 , list);
                    cout << "附近 " << MAXNEIGHBORLENGTH << " 米的建筑有："; 
                    for(int j = 0;list[j] !=0;j++)
                    {
                        PrintPlace(list[j]);
                        cout << "    ";
                        if(j % 5 == 4)
                        cout << endl;
                    }
                    cout << endl;
                }
                logger = "用户继续导航";
                writeFile(logger);
            }

        }
    }
}

void ShowResult(int s,int * result)
{
    cout << "距离 ";
    PrintPlace(s);
    cout << " " << MAXNEIGHBORLENGTH <<" 米以内的地点有： " << endl;
    for(int i = 0;result[i] != 0;i++)
    {
        PrintPlace(result[i]);
        cout << "      ";
        if((i % 5) == 4)
        cout << endl;
    }
    cout <<endl;
}

void Place :: operator=(const Place &P )
{
    ID = P.ID;
    Name = P.Name;
    for(int i = 0;i < 8 ;i ++)
    {
        Road[i] = P.Road[i];
        Event[i]= P.Event[1];
    }
    
}

void Clock::SetTime(int newH, int newM, int newS) //设置时间
{
    hour = newH;
    minute = newM;
    second = newS;
    second1 = newS;
    string Hour,Minute,Second;
        Hour = to_string(hour);
        Minute = to_string(minute);
        Second = to_string(second1);
        if(Hour.length() == 1)
        Hour = "0" + Hour;
        if(Minute.length() == 1)
        Minute = "0" + Minute;
        if(Second.length() == 1)
        Second = "0" + Second;
    	time1 = Hour + ':' + Minute + ':' + Second;
}

void Clock::ShowTime()        //显示时间，在显示时间前进行判断，如果时间设置不合适，则提示错误
{
    if (hour > 24 || hour<0 || minute>60 || minute<0 || second>60 || second < 0 || second1 > 60 || second1 < 0)
    {
        cout << "Error" << endl;
        exit(0);
    }
    else
    {
    	string Hour,Minute,Second;
        Hour = to_string(hour);
        Minute = to_string(minute);
        Second = to_string(second1);
        if(Hour.length() == 1)
        Hour = "0" + Hour;
        if(Minute.length() == 1)
        Minute = "0" + Minute;
        if(Second.length() == 1)
        Second = "0" + Second;
    	time1 = Hour + ':' + Minute + ':' + Second; 
    	cout << time1;
    }
}

void Clock::Run()  //每运行一次该函数，计时器向前增加一分钟
{
        second += 1;
        second1 +=60;
        if (second1 >= 60)
        {
            second1 -= 60;
            minute += 1;

        }
        if(second >= 60)
        {
        	second -= 60;
		}
        if (minute >= 60)
        {
            minute -= 60;
            hour += 1;
        }
        if (hour >= 24)
        {
            hour -= 24;
        }
        Sleep(1000);
        string Hour,Minute,Second;
        Hour = to_string(hour);
        Minute = to_string(minute);
        Second = to_string(second1);
        if(Hour.length() == 1)
        Hour = "0" + Hour;
        if(Minute.length() == 1)
        Minute = "0" + Minute;
        if(Second.length() == 1)
        Second = "0" + Second;
    	time1 = Hour + ':' + Minute + ':' + Second;
};

int Clock :: getminutes()
{
    return hour*60+minute;
}

void writeFile(string record){ 
    ofstream write;
    write.open("log.txt",ios::app);
    
    write << time1;
    write <<' ';
    write << record << endl;
    write.close();
};


