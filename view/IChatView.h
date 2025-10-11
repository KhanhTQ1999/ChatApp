#ifndef ICHATVIEW_H
#define ICHATVIEW_H

class IChatView
{
public:
    virtual ~IChatView() = default;
    virtual void run() = 0;
    virtual void stop() = 0;
};

#endif // ICHATVIEW_H