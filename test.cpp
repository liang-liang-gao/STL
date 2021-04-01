#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <queue>
#include <numeric>
#include <thread>
#include <mutex>
#include <set>
#include <map>
using namespace std;

// // class Student
// // {
// // 	private:
// // 		static int number;
// // 	public:
// // 		Student()
// // 		{
// // 			number++;
// // 			show("Student");
// // 		}
// // 		Student(const Student&)
// // 		{
// // 			number++;
// // 			show("Student");
// // 		}
// //         Student(Student&&)
// // 		{
// // 			number++;
// // 			show("Student");
// // 		}
// // 		~Student()
// // 		{
// // 			number--;
// // 			show("Student");
// // 		}
// // 		static void show(const char* str = NULL)//指向常量的指针
// // 		{
// // 			if(str)
// // 			{
// // 				cout << str << ":";
// // 			}
// // 			cout << "number=" << number <<endl;
// // 		}
// // };
 
// // int Student::number = 0;//静态数据成员赋值
 
// // Student f(Student x)
// // {
// // 	x.show("x inside f()");
// // 	return x;
// // }
 
// // int test(int &a)
// // {
// //     return a+1;
// // }
// // int main()
// // {
// //     int a=2;
// //     (++a)++;
// //     cout<<a<<endl;
// //     //test(2);
// //     // int a=2;
// //     // const int b=a;
// //     // b=4;
// //     // int c=b;
// // 	// Student h1;
// // 	// Student h2 = f(h1);
// // 	// Student::show("after call f()");
// // 	return 0;
// // }

class A
{
public :
	virtual void f(){}
};
class B : public A
{};
void fun (A* pa)
{
	// dynamic_cast会先检查是否能转换成功，能成功则转换，不能则返回
	B* pb1 = static_cast<B*>(pa);
	B* pb2 = dynamic_cast<B*>(pa);
	cout<<"pb1:" <<pb1<< endl;
	cout<<"pb2:" <<pb2<< endl;
}
class C:public A{

};


//冒泡 O(n^2) 稳定
void bubble(int *a, int n) {
    if(n == 1) return ;
    bool flag = false;
    for(int i = 0; i < n-1; i++) {
        if(!flag) {
            flag = true;//默认有序
            for(int j = 1; j < n-i; j++) {
                if(a[j-1] > a[j]) {
                    swap(a[j-1], a[j]);
                    flag = false;
                }
            } 
        }
        else break;
    }
}

//选择 O(n^2) 不稳定
void select(int *a, int n) {
    if(n == 1) return;
    for(int i=0;i<n-1;i++) {//选n-1趟即可
        //记录最大值的下标，将最大的值和a[n-i-1]交换
        int min_flag=i;
        int minm=a[i];
        for(int j=i+1;j<n;j++) {
            if(a[j] < minm) {
                minm = a[j];
                min_flag = j;
            }
        }
        swap(a[min_flag], a[i]);//这一步导致不稳定
    }
}

//插入排序 O(n^2) 稳定
void insert_sort(int *a, int n) {
    if(n == 1) return;
    for(int i = 1; i < n; i++) {//每次选择一个数
        int j = i-1;
        int temp = a[i];//待插入的数字！！！必须保存下来
        while(j >= 0 && temp < a[j]) {//这里必须使用temp保存结果，不能用a[i]，一定注意小错误
            a[j+1] = a[j];
            j--;
        }
        a[j+1] = temp;//注意这里是a[j+1]
    }
}

//希尔排序 O(n^1.3)  不稳定
void shell(int *a, int n) {
    int gap = n/2;//gap初始设为n/2
    while(gap) {//修改gap重新计算
        for(int w=0; w<gap; w++) {//遍历每一组
            for(int i=w+gap; i<n; i+=gap) {
                int j=i-gap;
                int temp=a[i];
                while(j>=0 && temp < a[j]) {
                    a[j+gap]=a[j];
                    j-=gap;
                }
                a[j+gap]=temp;
            }
        }
        gap/=2;
    }
}


//改进的希尔排序(只是写法改进)  O(n^1.3) 不稳定
void shell_better(int *a, int n) {
    int gap = n/2;//gap初始设为n/2
    while(gap) {//修改gap重新计算
        for(int i=gap; i<n; i++) {
            int j=i-gap;
            int temp=a[i];
            while(j>=0 && temp < a[j]) {
                a[j+gap]=a[j];
                j-=gap;
            }
            a[j+gap]=temp;
        }
        gap/=2;
    }
}

void merge(int *a, int first, int mid, int last, int *ans) {
    int k = 0;
    int start1 = first, end1 = mid;
    int start2 = mid+1, end2 = last;
    while(start1 <= end1 && start2 <= end2) {
        if(a[start1] < a[start2]) {
            ans[k++] = a[start1];
            start1++;
        }
        else {
            ans[k++] = a[start2];
            start2++;
        }
    }
    if(start1 > end1) {
        while(start2 <= end2) {
            ans[k++] = a[start2++];
        }
    }
    else {
        while(start1 <= end1) {
            ans[k++] = a[start1++];
        }
    }
    for(int i = 0; i<k ; i++) {
        a[first+i] = ans[i];//！！！很容易出错
    }
}

void apart(int *a, int first, int last, int *ans) {
    if(first < last) {
        int mid = (first+last)>>1;
        apart(a, first, mid, ans);
        apart(a, mid+1, last, ans);
        merge(a, first, mid, last, ans);//合并两个有序数列
    }
    else ans[first]=a[first];
}
//归并排序 O(NlogN) 稳定
void mergeSort(int a[], int n) {
    if(n == 1) return;
    int mid = n/2;
    int first = 0;
    int last = n-1;
    int *ans = new int[n];
    apart(a, first, mid, ans);
    apart(a, mid+1, last, ans);
    merge(a, first, mid, last, ans);
    //for(int i = 0; i < n; i++) a[i] = ans[i];
}

//分治
int quick_sort(int *a, int l, int r) {
    int left = l, right = r;
    int temp = a[left];
    while(left < right) {
        while(left < right && a[right] >= temp) {
            right--;
        }
        if(left < right) {
            a[left] = a[right];
            left++;
        } 
        while(left < right && a[left] <= temp) {
            left++;
        }
        if(left < right) {
            a[right] = a[left];
            right--;
        } 
    }
    if(left == right) {
        a[left] = temp;
    }
    return left;
}

//快速排序 O(NlogN) 不稳定
void quick(int *a, int l, int r) {
    int temp = quick_sort(a, l, r);
    if(l < temp-1) quick(a, l, temp-1);
    if(r > temp+1) quick(a, temp+1, r);
}

template<class T>
struct MyIter {
    typedef T value_tt;
    T* ptr;
    MyIter(T* p=0):ptr(p) {}
    T& operator*() {return *ptr;}
};


class abpro {
    public:
    virtual void op()=0;
};
class Apro: public abpro
{
    public:
    void op() {
        cout<<"Apro"<<endl;
    }
};
class Bpro: public abpro
{
    public:
    void op() {
        cout<<"Bpro"<<endl;
    }
};
class Cpro:public abpro
{
    public:
    void op() {
        cout<<"Cpro"<<endl;
    }
};

class abpro_new {
    public:
    virtual void op()=0;
};

class new_pro1 : public abpro_new
{
    public:
    void op() {
        cout<<"new_pro1"<<endl;
    }
};  

class new_pro2 : public abpro_new
{
    public:
    void op() {
        cout<<"new_pro2"<<endl;
    }
};  

class new_pro3 : public abpro_new
{
    public:
    void op() {
        cout<<"new_pro3"<<endl;
    }
};  
class abfac{
    public:
    virtual abpro* create()=0; 
    virtual abpro_new* create_new()=0;
};
class Afac: public abfac
{
    public:
    abpro* create() {//生产新的产品需要修改代码
        return new Apro(); 
    } 
    abpro_new* create_new() {
        return new new_pro1;
    }
};

class Bfac: public abfac
{
    public:
    abpro* create() {//生产新的产品需要修改代码
        return new Bpro(); 
    } 
    abpro_new* create_new() {
        return new new_pro2;
    }    
};
//扩展：
class Cfac:public abfac
{
    public:
    abpro* create() {
        return new Cpro();
    }
    abpro_new* create_new() {
        return new new_pro3;
    }
};
mutex m;
class single
{
    private:
    int value;
    static single* ptr;
    single() {
        value = 0;
    }
    public:
    static single* get() {
        if(ptr == nullptr) {
            m.lock();
            if(ptr == nullptr) {
                ptr = new single();
            }
            m.unlock();
        }
        return ptr;
    }
    int& get_value() {
        return value;
    }
};
single* single:: ptr = nullptr;
//观察者模式处理铃声
class ring//铃声类，包含设定铃声，获取此时状态
{
    private:
    bool sound;
    public:
    ring(bool _sound = true):sound(_sound) {}

    void setSound(bool _sound) {
        sound = _sound;
    }
    bool getSound() {
        return sound;
    }
};

class Observer
{
    public:
    virtual void hearbell(bool sound) {};
};

class teacher:public Observer
{
    public:
    void hearbell(bool sound) {
        if(sound) {
            cout<<"老师要上课了"<<endl;
        }
        else {
            cout<<"老师要下课了"<<endl;
        }
    }
};

class children:public Observer
{
    public:
    void hearbell(bool sound) {
        if(sound) {
            cout<<"学生要上课了"<<endl;
        }
        else {
            cout<<"学生要下课了"<<endl;
        }
    }
};
class bellEvent//事件源类，包含添加观察者，删除观察者，以及触发函数
{
    private:
    set<Observer*> listener;
    public:
    void Add(Observer* _listener) {
        listener.insert(_listener);
    }
    void Delete(Observer* _listener) {
        listener.erase(_listener);
    }
    void ringChange(bool sound) {
        string type = sound ? "上课铃" : "下课铃" ;
        cout<<type<<"响"<<endl;
        //ring event = new ring(sound);
        notify(sound);
    }
    void notify(bool sound) {
        for(auto it=listener.begin();it!=listener.end();it++) {
            (*it)->hearbell(sound);
        }
    }
};

// template<class I>
// typename I::value_tt func(I ite) {return *ite; }
int main ()
{
    int temp;
    cin>>temp;
    // teacher t1;
    // children c1;
    bellEvent *bell = new bellEvent();
    bell->Add(new teacher());
    bell->Add(new children());
    bell->ringChange(1);
    cout<<"----------------"<<endl;
    bell->ringChange(0);
    // single *a = single::get();
    // a->get_value() = 2; 
    // cout<<a->get_value()<<endl;


    // abfac* fac = new Cfac();
    // abpro* pro = fac->create();
    // abpro_new* pro_new = fac->create_new();
    // pro->op();
    // pro_new->op();
//     stack<int, deque<int>> b;
//     priority_queue<int, vector<int>, greater<int>> q;//小根堆
//     q.push(5);
//     q.push(4);
//     cout<<q.top()<<endl;
//     list<int> a;
//     //int a[5]={0};
//     //cout<<accumulate(a)<<endl;
//     int arr[]={10,20,30,40,50};  
// vector<int> va(&arr[0],&arr[5]);  
// int sum=accumulate(arr,arr+5, 45);  //sum = 150 
    // vector<int> b(4, 5);
    // deque<int> w(20,9);  
    // auto pp = w.begin();
    // cout<<*pp<<endl;
    // //cout<<*(pp.c)<<endl;
    // auto p = b.begin();
    // b.insert(p, 2);
    // cout<<*p<<endl;
    // p++;
    // cout<<*p<<endl;
    // MyIter<int> ite(new int(8));
    // cout<<func(ite)<<endl;
    // int *m=(int *)malloc(sizeof(int));
    // int *a=new int(0);
    // new(a) int(2);
    // int a[10]={0};
    // for(int i=0;i<10;i++) cin>>a[i];
    // //mergeSort(a,10);
    // quick(a,0,9);
    // for(int i=0;i<10;i++) cout<<a[i]<<endl;
    // double left=1.0;
    // double right=2;
    // double mid;
    // int maxm=20;
    // while(maxm--) {
    //     mid = (left+right)/2;
    //     if((mid*mid) > 2.0) right=mid;
    //     else left=mid;
    // }
    // cout<<mid<<endl;
    //printf("%12.10f\n", mid);
    // double a=5.4; 
    // int b=(int&)a;
    // cout<<b<<endl;
	// A a;
	// B b;
    // C c;
	// fun(&a);
	// fun(&b);
	return 0;
}
// class A
// {
// public :
// 	 A (int a)
// 	{
// 		cout<<"A(int a)" <<endl;
// 	}
// 	A(const A& a)
// 	{
// 		cout<<"A(const A& a)" <<endl;
// 	}

// private :
// 	int _a ;
// };
// class B
// {

// }
// int main ()
// {
//     vector<vector<int>> v(10, vector<int>(5, 0));
//     make_pair<int ,int>(2,4)
//     return 0;

//     // int *a = new int[5];
//     // cout<<sizeof(a)<<endl;
// 	// A a1 (1);
// 	// // 隐式转换-> A tmp(1); A a2(tmp);
// 	// A a2 = 1;
//}
// #include<queue>
// #include<vector>
// #include<queue>
// #include<pair>
// using namespace std;


// class Solution {
// public:
    
//     int dir[4][2]={{0,1}, {1,0}, {0,-1}, {-1,0}};
//     queue<pair<int, int>> q;
    
//     int bfs(int i, int j, int m, int n, vector<vector<int>>& v, vector<vector<char>>& grid) {
//         q.push({i, j});
//         v[i][j] = 1;
//         while(!q.empty()) {
//             pair<int ,int> temp = q.front();
//             q.pop();
//             for(int r=0; r<4; r++) {
//                 int x = i+dir[r][0];
//                 int y = j+dir[r][1];
//                 if((x>=0) && (x<m) && (y>=0) && (y<n) && grid[x][y] && !v[x][y]) {
//                     q.push({x,y});
//                     v[x][y] = 1;
//                 }
//             }
//         }
//     }
//     int numIslands(vector<vector<char>>& grid) {
//         if(grid.empty()) return 0;
//         int n = grid[0].size();
//         int m = grid.size();
//         vector<vector<int>> v(m, vector<int>(n, 0));
        
//         int i,j;
//         // for(i=0; i<m; i++) {
//         //     for(j=0; j<n; j++) {
//         //         if(grid[i][j] == 1)
//         //             break;
//         //     }
//         // }
        
//         // if(i == m) return 0;
//         // q.push((make_pair<int,int>(i, j)));
//         // v[i][j] = 1;
//         int ans=0;
//         for(i=0; i<m; i++) {
//             for(j=0; j<n; j++) {
//                 if(grid[i][j] && !v[i][j]) {
//                     bfs(i, j, m, n, v, grid);
//                     ans++;
//                 }
//             }
//         }
//         return ans;
//     }
// };

// int main() {
//     vector<vector<char>> grid;
    
// }