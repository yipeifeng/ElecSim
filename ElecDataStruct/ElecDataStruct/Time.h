#ifndef ElecPkg_Time_h
#define ElecPkg_Time_h

namespace ElecData {

typedef long int TimeInterval;
struct Time {
    static const int s2ns = 1000000000;
    long int s;
    int ns;

    Time() {
        s = 0;
        ns = 0;
    }

    Time(int t2) {
        s = t2/s2ns;
        ns = t2%s2ns;

    }

    // operator int () {
    //     return ns;
    // }

    Time& operator+=(const Time& t2) {
        ns += t2.ns;
        s += t2.s;
        if (ns >= s2ns) {
            s += 1;
            ns -= s2ns;
        }
        return *this;
    }

    Time operator+(const Time& t2) const {
        Time t;
        t.s = s+t2.s;
        t.ns = ns + t2.ns;
        return t;
    }

    Time operator+(const Time& t2) {
        Time t;
        t.s = s+t2.s;
        t.ns = ns + t2.ns;
        return t;
    }

    Time operator+(int t2) {
        Time t;
        //t.s = s+t2.s;
        t.ns = ns + t2;
        while(t.ns > s2ns)
        {
            t.ns -= s2ns;
            t.s += 1;
        }
        return t;
    }
    TimeInterval operator-(const Time& t2) {
        // Time t;
        // t.s = s-t2.s;
        // t.ns = ns - t2.ns;

        return (long int)(ns - t2.ns) + (s-t2.s)*s2ns;
    }

//  TimeInterval operator-(const Time& t2) {
//      Time t;
//      t.s = s-t2.s;
//      t.ns = ns - t2.ns;
//
//      if (t.ns < 0) {
//          t.ns += s2ns;
//          t.s -= 1;
//      }
//
//      if (t.s == 0) {
//          return t.ns;
//      } 
//
//      throw exception();
//  }

    Time& operator++() {
        return operator+=(1);

    }

    bool operator<(const Time& t2) const {
        if (s < t2.s) {
            return true;
        } else if (s == t2.s) {
            return (ns < t2.ns);
        } else {
            return false;
        }

    }
};

}


#endif
