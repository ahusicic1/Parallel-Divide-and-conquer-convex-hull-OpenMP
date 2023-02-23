#include <stack>
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <chrono>
#include<vector>
#include <algorithm>
#include <iterator>
#include <execution>
using namespace std;
using namespace std::chrono;

#define ll long long int
#define mp make_pair
#define pb push_back
#define SIZE 5000000
constexpr int num_test = 10;

struct Point
{
    ll x, y;
};
Point p0;
//ll size = SIZE;
vector<ll> points_x(SIZE);
vector<ll> points_y(SIZE);

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(ll px, ll py, ll qx, ll qy, ll rx, ll ry)
{
    ll val = (qy - py) * (rx - qx) -
        (qx - px) * (ry - qy);

    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

//u pocetnoj verziji serijske implementacije koristena je struct Point
//ali se ovdje umjesto toga koriste nizovi x i y koordinata
//obzirom da je za stvarnu sliku ubrzanja potrebno
//porediti optimalnu serijsku izvedbu sa paralelnom
auto convexHull_serial(vector<ll> points_x, vector<ll> points_y, ll n)
{
    std::vector<Point> hull;
    ll min = 0;
  /*  for (ll i = 1;i < n;i++) {
        if ((points_x[i] < points_x[min])) {
            // min_y=points[i].y;
            min = i;
        }
    }*/
   //std::executuin::seq je defaultni nacin izvodjenja
   //pobrinuti se da je n stvarni broj elemenata (a ne samo alocirani jer ce biti nule)
   
    auto rezultat = std::min_element(points_x.begin(), points_x.end()); 
    min = std::distance(points_x.begin(), rezultat);  //jer je min zapravo indeks
    ll p = min;
    ll q;
    Point trenutna_tacka;
    do {
        q = (p + 1) % SIZE;
        trenutna_tacka.x = points_x[p];
        trenutna_tacka.y = points_y[p];
        hull.pb(trenutna_tacka);
        for (ll i = 0;i < SIZE;i++) {
            if (orientation(points_x[p], points_y[p], points_x[i], points_y[i], points_x[q], points_y[q]) == 2) {
                q = i;
              
            }
        }
        p = q;
    } while (p != min);
    return hull;
}
auto  convexHull(const vector<ll>& points_x, const vector<ll>& points_y, ll n, int chunk)
{
    std::vector<Point> hull;
    ll min_y = points_y[0], min = 0;

    // #pragma omp parallel
    // #pragma omp for
 /*
#pragma omp parallel for schedule(static,chunk)
    for (ll i = 1;i < SIZE;i++) {
        if ((points_x[i] < points_x[min])) {
            // min_y=points[i].y;
            min = i; //reduction
        }
    }*/

    //ne zaustavlja se izvršenje ako se koriste linije ispod iz nekog razloga
    //odnosno q nikad ne postane = min (pokusavali smo i sa std::execution::par)
    //auto rezultat = std::min_element(std::execution::par, points_x.begin(), points_x.end());
    //min = std::distance(points_x.begin(), rezultat);  //jer je min zapravo indeks*/
    auto min_point = std::min_element(std::execution::par_unseq, points_x.begin(), points_x.end());
    min = std::distance(points_x.begin(), min_point);

   
    ll p = min;
    ll q;
   

    Point trenutna_tacka;
    do {
        q = (p + 1) % n;
        trenutna_tacka.x = points_x[p];
        trenutna_tacka.y = points_y[p];
        hull.pb(trenutna_tacka);
    
//#pragma omp parallel for schedule(static,chunk)
        for (ll i = 0;i < SIZE; i++) {
            if (orientation(points_x[p], points_y[p], points_x[i], points_y[i], points_x[q], points_y[q]) == 2) {
                q = i; //reduction, obrnuta petlja
                //obrnuta petlja ne moze jer se vrijednsoti
                //p i q iterativno popravljaju - ne možemo unaprijed znati za koje æe 
                //taèke funkcija orientation biti pozvana u zadnjem prolazu
            }
        }
        
        p = q;
    } while (p != min);
    return hull;
}
int main()
{
    // int n;
    // cin>>n;
   // Point points[n];

    long start_time, end_time;
    double  time_overhead;
    long start_time1, end_time1;
    double  time_overhead1;
    ll n = 100000;
    srand(time(NULL));
    for (int i = 0;i < SIZE;i++) {
        points_x[i] = rand() % n;
        points_y[i] = rand() % n;
    }
  /*  points_x[0] = 7;
    points_y[0] = 7;
    points_x[1] = 7;
    points_y[1] = -7;
    points_x[2] = -7;
    points_y[2] = -7;
    points_x[3] = -7;
    points_y[3] = 7;
    points_x[4] = 9;
    points_y[4] = 0;
    points_x[5] = -9;
    points_y[5] = 0;
    points_x[6] = 0;
    points_y[6] = 9;
    points_x[7] = 0;
    points_y[7] = -9;*/
    // Point points[] = {{0, 3}, {2, 2}, {1, 1}, {2, 1}, 
    //                   {3, 0}, {0, 0}, {3, 3}}; 
    // int n = sizeof(points)/sizeof(points[0]); 
    printf("set no of threads\n");
    int xx, chunk;
    cin >> xx;
    omp_set_num_threads(xx);
    printf("enter chunk size\n");
    cin >> chunk;
   
   // vector<Point> ch_parallel= convexHull(points_x, points_y, n, chunk);

   // vector< duration<double>> test_times(num_test);
    //for (int i = 0; i < num_test;++i)
    {
       
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        vector<Point> ch_parallel = convexHull(points_x, points_y, SIZE, chunk);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        cout << "Elapsed time: " << time_span.count() << " seconds.";

       // test_times.push_back(time_span);
    }


  /*  cout << ch_parallel.size() << endl;
    for (ll i = 0;i < ch_parallel.size();i++) {
        cout << ch_parallel[i].x << " " << ch_parallel[i].y << endl;
    }*/

  /*  high_resolution_clock::time_point t1_serial = high_resolution_clock::now();
    vector<Point> ch_serial(512);
    auto pocetak = std::begin(points_x);
    auto kraj = std::end(points_x);
    ch_serial = convexHull_serial(points_x, points_y, n);
    high_resolution_clock::time_point t2_serial = high_resolution_clock::now();
    duration<double> time_span_serial = duration_cast<duration<double>>(t2_serial - t1_serial);
    cout << "Elapsed time: " << time_span_serial.count() << " seconds.";
    cout << ch_serial.size() << endl;
    cout << "n je " << n;
    int j = 1;
    for (ll i = 0;i < ch_serial.size();i++) {
        cout << ch_serial[i].x << " " << ch_serial[i].y << endl;
        cout << "j = " << j++ << endl;
    }*/
    return 0;
}

/*
int main_novi()
{
    // int n;
    // cin>>n;
   // Point points[n];

    long start_time, end_time;
    double  time_overhead;
    long start_time1, end_time1;
    double  time_overhead1;
    for (int i = 0;i < n;i++) {
        points_x[i] = rand() % n;
        points_y[i] = rand() % n;
    }
    /*  points_x[0] = 7;
      points_y[0] = 7;
      points_x[1] = 7;
      points_y[1] = -7;
      points_x[2] = -7;
      points_y[2] = -7;
      points_x[3] = -7;
      points_y[3] = 7;
      points_x[4] = 9;
      points_y[4] = 0;
      points_x[5] = -9;
      points_y[5] = 0;
      points_x[6] = 0;
      points_y[6] = 9;
      points_x[7] = 0;
      points_y[7] = -9;*/
      // Point points[] = {{0, 3}, {2, 2}, {1, 1}, {2, 1}, 
      //                   {3, 0}, {0, 0}, {3, 3}}; 
      // int n = sizeof(points)/sizeof(points[0]); 
   /* printf("set no of threads\n");
    int xx, chunk;
    cin >> xx;
    omp_set_num_threads(xx);
    printf("enter chunk size\n");
    cin >> chunk;

    vector<Point> ch_parallel = convexHull(points_x, points_y, n, chunk);

    vector< duration<double>> test_times(num_test);
    for (int i = 0; i < num_test;++i)
    {

        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        vector<Point> ch_parallel = convexHull(points_x, points_y, n, chunk);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        test_times.push_back(time_span);
    }


    cout << "Elapsed time: " << time_span.count() << " seconds.";
    /*  cout << ch_parallel.size() << endl;
      for (ll i = 0;i < ch_parallel.size();i++) {
          cout << ch_parallel[i].x << " " << ch_parallel[i].y << endl;
      }*/

      /*  high_resolution_clock::time_point t1_serial = high_resolution_clock::now();
        vector<Point> ch_serial(512);
        auto pocetak = std::begin(points_x);
        auto kraj = std::end(points_x);
        ch_serial = convexHull_serial(points_x, points_y, n);
        high_resolution_clock::time_point t2_serial = high_resolution_clock::now();
        duration<double> time_span_serial = duration_cast<duration<double>>(t2_serial - t1_serial);
        cout << "Elapsed time: " << time_span_serial.count() << " seconds.";
        cout << ch_serial.size() << endl;
        cout << "n je " << n;
        int j = 1;
        for (ll i = 0;i < ch_serial.size();i++) {
            cout << ch_serial[i].x << " " << ch_serial[i].y << endl;
            cout << "j = " << j++ << endl;
        }*/
  //  return 0;
//}*/