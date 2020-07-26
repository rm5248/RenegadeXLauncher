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

    void setFilename( QString filename );
    void setExpected( QString expected );

public slots:
    void checksumCalculationCompleted( bool success, QString calculated );

private:
    Ui::ChecksumDisplay *ui;
};

#endif // CHECKSUMDISPLAY_H
