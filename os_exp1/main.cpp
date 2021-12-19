#include <iostream>
#include <unistd.h>
#include <cstring>
#include "mysemop.h"

int chops[5];

int chop_lock[5];

int if_chops_token[5];


using std::cout;
using std::endl;

void report() {
    std::string report = " [";
    for(int i = 0; i < 4;i++) {
        if(semctl(if_chops_token[i], 0, GETVAL, 0) <= 0) {
            report+= "竞争, ";
        } else if(semctl(if_chops_token[i], 0, GETVAL, 0) == 1) {
            report+= "占用, ";
        } else {
            report+= "可用, ";
        }
    }

    if(semctl(if_chops_token[4], 0, GETVAL, 0) <=0) {
        report+= "竞争]";
    } else if(semctl(if_chops_token[4], 0, GETVAL, 0) == 1) {
        report+= "占用]";
    } else {
        report+= "可用]";
    }

    printf("进程:%d --> %s(%d) %s\n",for_p_index, op, chop_index, report.c_str());
}


void process_doing(const char *what, int for_p_index) {
    cout << "进程:" << for_p_index << " --> " << what << "）" << endl;
}


void process_take_left(int for_p_index) {
    int chop_index = for_p_index;
    cout << "进程:" << for_p_index << " --> " << "拿起左边筷子（" << chop_index << "）" << endl;
    if_chops_token[chop_index]++;
    Psem(chops[chop_index]);
}

void process_take_right(int for_p_index) {
    int chop_index = (for_p_index + 1) % 5;
    cout << "进程:" << for_p_index << " --> " << "拿起右边筷子（" << chop_index << "）" << endl;
    if_chops_token[chop_index]++;
    Psem(chops[chop_index]);
}

void process_take(const char *direction, int for_p_index) {
    int chop_index;

    const char* op = nullptr;

    if (strcmp(direction, "right") == 0) {
        chop_index = (for_p_index + 1) % 5;
        op = "拿起右边筷子";
    } else {
        chop_index = for_p_index;
        op = "拿起左边筷子";
    }


    Psem(chops[chop_index]);

    Psem(chop_lock[chop_index]);

    //cout << "进程:" << for_p_index << " --> " << op << "（" << chop_index << "）" << endl;

    Psem(if_chops_token[chop_index]);



    std::string report = " [";
    for(int i = 0; i < 4;i++) {
        if(semctl(if_chops_token[i], 0, GETVAL, 0) <= 0) {
            report+= "竞争, ";
        } else if(semctl(if_chops_token[i], 0, GETVAL, 0) == 1) {
            report+= "占用, ";
        } else {
            report+= "可用, ";
        }
    }

    if(semctl(if_chops_token[4], 0, GETVAL, 0) <=0) {
        report+= "竞争]";
    } else if(semctl(if_chops_token[4], 0, GETVAL, 0) == 1) {
        report+= "占用]";
    } else {
        report+= "可用]";
    }

    //cout << "进程:" << for_p_index << " --> " << op << "（" << chop_index << "）" << report  << endl;
    printf("进程:%d --> %s(%d) %s\n",for_p_index, op, chop_index, report.c_str());

    Vsem(chop_lock[chop_index]);
}

void process_put(const char *direction, int for_p_index) {
    int chop_index;

    const char* op = nullptr;

    if (strcmp(direction, "right") == 0) {
        chop_index = (for_p_index + 1) % 5;
        op = "放下右边筷子";
    } else {
        chop_index = for_p_index;
        op = "放下左边筷子";
    }

    Psem(chop_lock[chop_index]);


    //cout << "进程:" << for_p_index << " --> " << op << "（" << chop_index << "）" << endl;

    Vsem(chops[chop_index]);


    Vsem(if_chops_token[chop_index]);

    report();

    //cout << "进程:" << for_p_index << " --> " << op << "（" << chop_index << "）" << report  << endl;



    Vsem(chop_lock[chop_index]);
}
//
//void process_put_left(int for_p_index) {
//    cout << "进程:" << for_p_index << " --> " << "放下左边筷子（" << for_p_index << "）" << endl;
//    if_chops_token[for_p_index]--;
//    Vsem(chops[for_p_index]);
//}
//
//void process_put_right(int for_p_index) {
//    int chop_index = (for_p_index + 1) % 5;
//    cout << "进程:" << for_p_index << " --> " << "放下右边筷子（" << chop_index << "）" << endl;
//    if_chops_token[chop_index]--;
//    Vsem(chops[chop_index]);
//}


int main() {
    //std::cout << "Hello, World!" << std::endl;

    for (int i = 0; i < 5; i++) {
        chops[i] = CreateSem(1);
        chop_lock[i] = CreateSem(1);
        if_chops_token[i] = CreateSem(2);
    }

    int p_index = 0;

    int count = 0;
    for (int i = 0; i < 4; i++) {
        int re = fork();
        count++;
        if (re == 0) {
            p_index = count;
            break;
        }

    }
    //
    ////cout << getpid() << endl;
    //cout << p_index << endl;



//while(1){}


    //while (1) {
    //
    //    cout << "进程:" << p_index << " --> " << "think" << endl;
    //
    //    if(p_index % 2 == 0) {
    //        process_take("left", p_index);
    //        process_take("right", p_index);
    //    } else {
    //        process_take("right", p_index);
    //        process_take("left", p_index);
    //    }
    //
    //
    //
    //    //process_take_left(p_index);
    //    //process_take_right(p_index);
    //
    //    cout << "进程:" << p_index << " --> " << "eat" << endl;
    //
    //    process_put("left", p_index);
    //    process_put("right", p_index);
    //}


    while (1) {

        cout << "进程:" << p_index << " --> " << "think" << endl;

        process_take("left", p_index);
        process_take("right", p_index);
        //process_take_left(p_index);
        //process_take_right(p_index);

        cout << "进程:" << p_index << " --> " << "eat" << endl;

        process_put("left", p_index);
        process_put("right", p_index);
    }

    while (1);


    return 0;
}
