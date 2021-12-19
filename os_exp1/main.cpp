#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include "mysemop.h"

#define STICKFLAG 10

#define STICKNUM 5

#define PERSONNUM 5

int chops[STICKNUM];

//拿起筷子是临界区，包含：报告情况、拿起筷子
int chop_take_locks[STICKNUM];

//记录筷子拿起的根数
int chop_take_record[STICKNUM];

int less_than_4_mutex;

//报告是临界资源
int report_mutex;

using std::cout;
using std::endl;

std::string generate_report() {
    std::string report = " [";
    for (int i = 0; i < STICKNUM; i++) {
        int rest = semctl(chop_take_record[i], 0, GETVAL, 0);
        int token_num = STICKFLAG - rest;
        std::stringstream ss;
        if (i < STICKNUM - 1) {
            ss << token_num << ", ";
        } else {
            ss << token_num << "]";
        }
        report += ss.str();
    }

    return report;
}


void take_stick(int for_stick_index) {
    Psem(chops[for_stick_index]);
}

void put_stick(int for_stick_index) {
    Vsem(chops[for_stick_index]);
}

void record_stick_token(int chop_index) {
    Psem(chop_take_record[chop_index]);
}

void record_stick_put(int chop_index) {
    Vsem(chop_take_record[chop_index]);
}

void report(int for_p_index, const char *report_str, int chop_index) {
    std::string report = generate_report();
    printf("进程:%d --> %s(%d) %s\n", for_p_index, report_str, chop_index, report.c_str());
}

void lock_stick(int for_chop_index) {
    Psem(chop_take_locks[for_chop_index]);
}

void unlock_stick(int for_chop_index) {
    Vsem(chop_take_locks[for_chop_index]);
}

void report_before(const char *op, int for_p_index, const char *report_str, int chop_index) {
    Psem(report_mutex);
    if (strcmp(op, "take") == 0) {
        record_stick_token(chop_index);
    } else {
        record_stick_put(chop_index);
    }
    report(for_p_index, report_str, chop_index);
    Vsem(report_mutex);
}

void process_take(const char *direction, int for_p_index) {
    int chop_index_2be_token;

    const char *report_str = nullptr;

    if (strcmp(direction, "right") == 0) {
        chop_index_2be_token = (for_p_index + 1) % STICKNUM;
        report_str = "⬆右边筷子";
    } else {
        chop_index_2be_token = for_p_index;
        report_str = "⬆左边筷子";
    }

    lock_stick(chop_index_2be_token);

    report_before("take", for_p_index, report_str, chop_index_2be_token);
    take_stick(chop_index_2be_token);

    unlock_stick(chop_index_2be_token);
}


void process_put(const char *direction, int for_p_index) {
    int chop_index_2be_token;

    const char *report_str = nullptr;

    if (strcmp(direction, "right") == 0) {
        chop_index_2be_token = (for_p_index + 1) % STICKNUM;
        report_str = "⬇右边筷子";
    } else {
        chop_index_2be_token = for_p_index;
        report_str = "⬇左边筷子";
    }

    //lock_stick(chop_index_2be_token);

    report_before("put", for_p_index, report_str, chop_index_2be_token);

    put_stick(chop_index_2be_token);

    //unlock_stick(chop_index_2be_token);
}

int p_index = 0;

void init_mutex() {
    less_than_4_mutex = CreateSem(4);
    report_mutex = CreateSem(1);
    for (int i = 0; i < STICKNUM; i++) {
        chops[i] = CreateSem(1);
        chop_take_locks[i] = CreateSem(1);
        chop_take_record[i] = CreateSem(STICKFLAG);
    }
}

bool is_child(int flag){
    return flag == 0;
}

void span_process() {
    int count = 0;
    for (int i = 0; i < PERSONNUM - 1; i++) {
        int re = fork();
        count++;
        if (is_child(re)) {
            p_index = count;
            break;
        }
    }
}

void init() {
    init_mutex();
    span_process();
}

void normal() {
    while (1) {
        cout << "进程:" << p_index << " --> " << "think" << endl;

        process_take("left", p_index);
        process_take("right", p_index);

        cout << "进程:" << p_index << " --> " << "eat" << endl;

        process_put("left", p_index);
        process_put("right", p_index);
    }
}

void odd_even_number() {
    while (1) {
        cout << "进程:" << p_index << " --> " << "think" << endl;

        if (p_index % 2 == 0) {
            process_take("right", p_index);
            process_take("left", p_index);
        } else {
            process_take("left", p_index);
            process_take("right", p_index);
        }

        cout << "进程:" << p_index << " --> " << "eat" << endl;

        process_put("left", p_index);
        process_put("right", p_index);
    }
}

void less_than_4() {
    while (1) {

        cout << "进程:" << p_index << " --> " << "think" << endl;

        Psem(less_than_4_mutex);
        process_take("left", p_index);
        process_take("right", p_index);

        cout << "进程:" << p_index << " --> " << "eat" << endl;

        process_put("left", p_index);
        process_put("right", p_index);
        Vsem(less_than_4_mutex);
    }
}

int main() {
    init();
    less_than_4();
}