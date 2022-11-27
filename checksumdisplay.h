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
    enum class ChecksumState {
        Incomplete,
        Good,
        Bad,
    };

    explicit ChecksumDisplay(QWidget *parent = nullptr);
    ~ChecksumDisplay();

    void setFilename( QString filename );
    void setExpected( QString expected );
    ChecksumState checksumState() const;

public slots:
    void checksumCalculationCompleted( bool success, QString calculated );

signals:
    void checksumStateChanged();

private:
    Ui::ChecksumDisplay *ui;
    ChecksumState m_state;
};

#endif // CHECKSUMDISPLAY_H
