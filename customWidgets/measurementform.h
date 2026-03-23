#ifndef MEASUREMENTFORM_H
#define MEASUREMENTFORM_H

#include <QDialog>

namespace Ui {
class MeasurementForm;
}

class MeasurementForm : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurementForm(QWidget *parent = nullptr);
    inline void setStartPoint(const QPointF& point)  { mStartPos = point; };
    inline void setEndPoint  (const QPointF& point)  { mEndPos   = point; };
    inline std::pair<QPointF, QPointF> getPoints() const { return {mStartPos, mEndPos}; }
    void setDistance(double distance);
    bool isGeodesicalMode() const;
    bool isCartesianMode () const;
    std::string getEllipsoid() const;
    ~MeasurementForm();
signals:
    void ellipsoidChanged(const std::string& ellipsName);
    void modeChanged();
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_radioButtonCartesian_clicked();

    void on_radioButtonGeodesical_clicked();

    void on_pushButtonExit_clicked();

private:
    Ui::MeasurementForm *ui;
    QPointF mStartPos;
    QPointF mEndPos;
};

#endif // MEASUREMENTFORM_H
