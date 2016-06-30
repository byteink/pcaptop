#include "ui.h"

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <iostream>
#include "net_types.h"
#include "syn_stat.h"
#include "flow_stat.h"

using namespace std;

FlowStat f1(60, 2);    // 1分钟
FlowStat f5(300, 10);   // 5分钟
FlowStat f10(600, 20);   // 10分钟

SynStat s1(60, 2);
SynStat s5(300, 10);
SynStat s10(600, 20);

static pthread_t ui_thread;

void initUI(const char* interface) {
    // init ncurses
    //initscr();
    //cbreak();
    //keypad(stdscr, TRUE);
    //noecho();

    // init ui update thread
    void *updateUIRoutine(void *arg);
    int ret = pthread_create(&ui_thread, NULL, updateUIRoutine, NULL);
    if (ret != 0) {
        perror("call pthread_create failed");
        endwin();
        exit(-1);
    }
}

void exitUI() {
    // end ncurses mode
    endwin();
}

void addData(const DataPoint& dp) {
    //std::cout << dp << std::endl;
    time_t now = getCurrentSeconds();

    f1.addData(dp, now);
    f5.addData(dp, now);
    f10.addData(dp, now);

    s1.addData(dp, now);
    s5.addData(dp, now);
    s10.addData(dp, now);
}

void *updateUIRoutine(void *arg) {
    while (true) {
        sleep(1);

        std::vector<SynStat::Result> syn_results;
        s1.getResults(10, syn_results);
        //for (int i = 0; i < syn_results.size(); ++i) {
            //cout << syn_results[i].ip << "\t" << syn_results[i].nums_of_syn << endl;
        //}


        std::vector<FlowStat::Result> flow_results;
        f1.getResults(10,  FlowStat::SORT_BY_TOTOAL, flow_results);
        for (int i = 0; i < flow_results.size(); ++i) {
            FlowStat::Result& r = flow_results[i];
            std::cout << r.addr.ip << ":" << r.addr.port << "\t" << r.flow.in + r.flow.out << std::endl;
        }

        cout << "**********************" << endl << endl;
    }
}


