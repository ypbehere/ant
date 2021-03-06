#include <QDebug>
#include "city.h"
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <QTimer>
#include <QTextStream>
#include <QFile>
#include <QString>
using namespace std;
QString DATA_DIR = "/home/ypbehere/Documents/srtp/ant1/Ant/data/data.txt";

CCity::CCity()
{
    QFile data(DATA_DIR);
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cout << "Open failed." << endl;
    }

    QTextStream dataStream(&data);
    int storeCount = 0;
    while(!dataStream.atEnd())
    {
        dataStream >> _store[storeCount]._pos._x >> _store[storeCount]._pos._y;
        storeCount++;
    }
    _storeNum = storeCount - 1;

    for (int i = 0; i < _driverNum; i++) {
        _driver[i]._pos._x = _store[0].x();
        _driver[i]._pos._y = _store[0].y();
        _driver[i].tempPos = 0;
    }

    data.close();
}

double CCity::store2StoreDis(int s1, int s2) {
    if (s1 != s2) {
        double xDis = _store[s1].x() - _store[s2].x();
        double yDis = _store[s1].y() - _store[s2].y();
        return sqrt(pow(xDis, 2) + pow(yDis, 2));
    }
    else return 0.001;
}

double CCity::store2StoreEta(int s1, int s2) {
    return 1 / store2StoreDis(s1, s2);
}

void CCity::start() {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fresh()));
    timer->start(1000);
}

void CCity::fresh() {
    emit needDraw();

    Ant tempAnt(10, 1);
    vector<int> tmp = tempAnt.dealWithData();

    int i = 0;
    while (tmp[i] != _driver[0].tempPos) i++;
    int front = i == 0 ? tmp[_storeNum - 1] : tmp[i-1];
    int next = i == _storeNum - 1 ? tmp[0] : tmp[i+1];
    double frontDis = store2StoreDis(_driver[0].tempPos, front);
    double nextDis = store2StoreDis(_driver[0].tempPos, next);
    int finalDicision = frontDis > nextDis ? next : front;

    _driver[0]._pos._x = _store[finalDicision].x();
    _driver[0]._pos._y = _store[finalDicision].y();
    qDebug() << _driver[0].tempPos;
    clearStore(_driver[0].tempPos);
    _storeNum--;
    _driver[0].tempPos = finalDicision > _driver[0].tempPos ? finalDicision - 1 : finalDicision;
}

void CCity::clearStore(int i) {
    if (i == _storeNum) return;
    while (i + 1 < _storeNum) {
        _store[i]._pos._x = _store[i+1].x();
        _store[i]._pos._y = _store[i+1].y();
        i++;
    }
}
