#ifndef TIMER
#define TIMER
#include <QTimer>

class Timer:public QTimer
{
public:
    Timer();
    ~Timer();

public slots:
    void execute();
};

#endif // TIMER

