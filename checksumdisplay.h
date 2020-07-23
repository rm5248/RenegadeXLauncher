#ifndef CHECKSUMDISPLAY_H
#define CHECKSUMDISPLAY_H

#include <QWidget>

namespace Ui {
class ChecksumDisplay;
}

class ChecksumDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit ChecksumDisplay(QWidget *parent = nullptr);
    ~ChecksumDisplay();

private:
    Ui::ChecksumDisplay *ui;
};

#endif // CHECKSUMDISPLAY_H
