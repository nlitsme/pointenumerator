#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cmath>

struct point { 
    // simple 2-d point object, supports point addition, subtraction
    // scalar multiplication, division
    // distance and norm calculation.
    // and stream output.
    // default construct or from two integers.

    point() : x(0), y(0) { }
    point(int x, int y) : x(x), y(y) { }

    int x;
    int y;

    point operator+(const point& rhs) const { return point(x+rhs.x, y+rhs.y); }
    point operator-(const point& rhs) const { return point(x-rhs.x, y-rhs.y); }
    point operator*(int rhs) const { return point(x*rhs, y*rhs); }
    point operator/(int rhs) const { return point(x/rhs, y/rhs); }

    bool operator==(const point& rhs) const { return x==rhs.x and y==rhs.y; }

    double dist(const point& other) const
    {
        return (*this - other).norm();
    }
    double norm() const
    {
        return sqrt(x*x + y*y);
    }
    friend std::ostream& operator<<(std::ostream& os, const point& pt)
    {
        return os << '(' << pt.x << ',' << pt.y << ')';
    }
    std::string str() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }
};

// integer math helper functions
int intsqrt(int n)
{
    return std::sqrt((double)n);
}
int intsqr(int n)
{
    return n*n;
}

// template for returning maximum of any number of arguments.
template<typename T>
T nmax(T x) { return x; }

template<typename T, typename... Args>
T nmax(T x, const Args&... args)
{
    return std::max(x, nmax(args...));
}

struct enumbase {
    // base class for all point enumerators
    //
    // provide iterator interface

    struct iter : std::iterator<std::forward_iterator_tag, point> {
        int _n;
        enumbase *_sp;
        iter(int n, enumbase *sp)
            : _n(n), _sp(sp)
        {
        }
        point operator*() const
        {
            return _sp->getpos(_n);
        }
        iter& operator++()
        {
            ++_n;
            return *this;
        }
        bool operator!=(const iter& rhs)
        {
            return _n!=rhs._n;
        }
    };
    int _w;
    enumbase(int w)
        : _w(w)
    {
    }

    int width() const { return _w; }

    iter begin() { return iter(0, this); }
    iter end() { return iter(maxiter(), this); }

    point getpos(int n)
    {
        return n2pt(n);
    }

    virtual int maxiter() = 0;
    virtual point n2pt(unsigned n) = 0;
    virtual int pt2n(const point& p) = 0;
};
struct spiralenum : enumbase {
    // enumerate points in the entire x,y plane starting at the origin.
    // counter clockwise in  square shape.
    // distance between consecutive points is 1 for all
    // but the first and last point.
    // which are: sqrt(2)*(w-1) apart.
    //
    //      4<--3<--2   .
    //      |       ^   .
    //      v       |   .
    //      5   0-->1  10
    //      |           ^
    //      v           |
    //      6-->7-->8-->9
    //
    spiralenum(int w)
        : enumbase(w)
    {
    }

    virtual int maxiter()
    {
        return intsqr((_w-1)*2+1);
    }
    point n2pt(unsigned n)
    {
        if (n==0)
            return point(0,0);

        int s= (intsqrt(n)-1)/2;
        int r= n-intsqr(s*2+1);
        s++;

        int q= r/(2*s);  // quadrant
        int m= r%(2*s);
        switch(q) {
            case 0: return point(s, m-s+1);
            case 1: return point(s-1-m, s);
            case 2: return point(-s, s-1-m);
            case 3: return point(m-s+1, -s);
        }
        throw "invalid q";
    }
    int pt2n(const point& p)
    {
        int s= nmax(p.x, -p.x, p.y, -p.y);
        int q, m;
        if (p.x==s && p.y > -s) { q=0; m=p.y+s-1; }
        else if (p.y==s)  { q=1; m=s-1-p.x; }
        else if (p.x==-s) { q=2; m=s-1-p.y; }
        else              { q=3; m=p.x+s-1; }

        int r= 2*s*q+m;
        s--;

        return r+intsqr(s*2+1);
    }
};

struct zigzagenum : enumbase {
    // enumerate points in the quadrant where 0 <= x,y < w
    // zigzag in a square shape.
    // distance between consecutive points is 1 for all
    // but the first and last point.
    // which are either (even w) 'w-1', or (odd w) sqrt(2)*(w-1) apart.
    //
    //   8 .......
    //   ^
    //   |
    //   7<--6<--5<--4
    //               ^
    //               |
    //   0-->1-->2-->3
    //
    zigzagenum(int w)
        : enumbase(w)
    {
    }

    virtual int maxiter()
    {
        return intsqr(_w);
    }
    point n2pt(unsigned n)
    {
        if (_w==0)
            return point(0,0);
        int y = n/_w;
        int x = n%_w;
        if (y%2)
            return point(_w-1-x, y);
        else
            return point(x, y);
    }
    int pt2n(const point& p)
    {
        int x = p.x;
        int y = p.y;
        if (y%2)
            x = _w-1-x;
        return y*_w+x;
    }
};

struct triangleenum : enumbase {
    // enumerate points in the quadrant where x,y >= 0
    // zigzag in a triangle shape.
    // distance between consecutive points is either 1, or sqrt(2)
    // or (for the first and last point): w-1
    //
    //  3
    //  ^\
    //  | \
    //  2  \4  .
    //   \   \  .
    //    \   \  \
    // 0-->1   5->6
    //
    triangleenum(int w)
        : enumbase(w)
    {
    }
    virtual int maxiter()
    {
        return _w*(_w+1)/2;
    }
    point n2pt(unsigned n)
    {
        int i = (-1+intsqrt(1+8*n))/2;
        int j = n - i*(i+1)/2;

        if (i%2)
            j = i - j;

        return point(j, i-j);
    }
    int pt2n(const point& p)
    {
        int i = p.x + p.y;
        int j = p.x;

        if (i%2)
            j = i - j;

        return i*(i+1)/2 + j;
    }
};

struct diamondenum : enumbase {
    // enumerate points in the entire plane,
    // clockwise, in diamond shape.
    // distance between consecutive points is either 1, or sqrt(2)
    // or (for the first and last point): w-1
    //
    //       4->5
    //      /    \
    //     /      \
    //    3  0->1  6
    //     \   /  .
    //      \ /  .
    //       2
    //
    diamondenum(int w)
        : enumbase(w)
    {
    }
    virtual int maxiter()
    {
        return 2*_w*(_w-1)+1;
    }

// 'diamond'
//             40*41
//          39 24*25 42
//       38 23 12*13 26 43
//    37 22 11  4 *5 14 27 44
// 36 21 10  3  0 *1  6 15 28 45
//    35 20  9  2  7 16 29
//       34 19  8 17 30
//          33 18 31
//             32
//
// '*' == 1   -2*x   +2*x^2  : 1 5 13 25 41
// x-axis:    -1*x   +2*x^2  : 1 6 15 28 45


    point n2pt(unsigned n)
    {
        if (n==0)
            return point(0,0);
        int i = (1 + intsqrt(2*n-1))/2;
        int j = n - (2*intsqr(i)-2*i+1);

        int q = j/i;  // quadrant
        int m = j%i;

        switch (q)
        {
            case 0: return point(m+1, i-1-m);  // x+y = i
            case 1: return point(i-1-m, -m-1); // x-y = i
            case 2: return point(-m-1, m-i+1); //-x-y = i
            case 3: return point(m-i+1, m+1);  //-x+y = i
        }
        throw "invalid q";
    }
    int pt2n(const point& p)
    {
        int q, m, i;
        if (p.x>0 && p.y>=0) { q=0; m=p.x-1; i = p.x+p.y; }
        else if (p.x>0 && p.y<0) { q=1; m=-p.y-1; i = p.x-p.y; }
        else if (p.x<=0 && p.y<0) { q=2; m=-p.x-1; i = -p.x-p.y; }
        else if (p.x<=0 && p.y>=0) { q=3; m=p.y-1; i = -p.x+p.y; }
        else
            throw "invalid q";

        int j = q*i+m;

        return j + (2*intsqr(i)-2*i+1);
    }
};

// todo: blockiter: step distance is always 1
//
//    ..12
// 4 5 611
// 3 2 710
// 0 1 8 9
//
//
// todo: circleiter: enumerate points in increasing distance from origin.
    
struct Grid {
    // 2-dimensional grid
    int _w;
    int _w2;
    std::vector<int> v;
    Grid(int w)
        : _w(w), _w2(2*w+1), v(_w2*_w2, -1) 
    {
    }
    int &at(int i, int j)
    {
        i += _w;
        j += _w;

        int n = i*_w2+j;
        if (n<0 || n>=v.size())
            throw "invalid index";

        return v[ n ];
    }
};
template<typename ENUMERATOR>
void test(ENUMERATOR points)
{
    int w = points.width();

    std::cout << "points to ints\n";
    // decode all x,y combinations to integer
    for (int y=w ; y>=-w ; y--) {
        for (int x=-w ; x<=w ; x++)
            std::cout << std::setw(4) << points.pt2n(point(x,y));
        std::cout << '\n';
    }
    std::cout << '\n';


    // decode all ints to x,y combination
    std::vector<point> pts;
    for (int i=0 ; i<points.maxiter() ; i++)
        pts.push_back( points.n2pt(i) );

    // use generator to generate all points
    std::vector<point> pts_gen;
    for (auto p : points )
        pts_gen.push_back(p);

    // these two lists should be equal
    if (pts != pts_gen)
        std::cout << "WARNING: iterated and generated list don't match\n";

    // calculate delta's: depending on enumerator, this should be either:
    //    - only 1.0
    //    - 1.0 and sqrt(2)
    std::map<double, int> delta_stats;
    if (!pts.empty()) {
        point q = pts.back();
        for (auto p : pts) {
            delta_stats[ p.dist(q) ]++;
            q = p;
        }
    }
    std::cout << "consecutive point differences\n";
    for (auto kv : delta_stats)
        std::cout << std::setw(4) << kv.second << " : " << kv.first << '\n';


    // test pt2n(n2pt(i)) == i
    for (int i=0 ; i<pts.size() ; i++)
        if (points.pt2n(pts[i]) != i)
            std::cout << "WARNING: " << i << "->" << pts[i] << "->" << points.pt2n(pts[i]) << std::endl;

    // test point generator

    Grid grid(w);

    int i = 0;
    for (auto p : pts ) {
        grid.at(p.y,p.x) = i;
        i++;
    }

    std::cout << "generated grid\n";
    // output grid generated by generator
    for (int y = w ; y >= -w ; y--) {
        for (int x = -w ; x <= w ; x++)
            std::cout << std::setw(3) << grid.at(y, x);
        std::cout << '\n';
    }
    std::cout << '\n';

}

int main(int,char**)
{
    for (int w=0 ; w<9 ; w++) {
        std::cout << "---- zigzag:" << w << " ----\n";
        test(zigzagenum(w));

        std::cout << "---- spiral:" << w << " ----\n";
        test(spiralenum(w));

        std::cout << "---- triangle:" << w << " ----\n";
        test(triangleenum(w));

        std::cout << "---- diamond:" << w << " ----\n";
        test(diamondenum(w));
    }
}
