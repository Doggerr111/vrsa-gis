#ifndef MEASUREMENTFORM_H
#define MEASUREMENTFORM_H

#include <QDialog>

namespace Ui {
class MeasurementForm;
}

/**
 * @brief Диалоговое окно для отображения результатов измерений
 *
 * Показывает расстояние между двумя точками, измеренное линейкой.
 * Поддерживает два режима расчёта:
 * - на плоскости (евклидово расстояние)
 * - геодезический (с учётом кривизны Земли)
 *
 * Позволяет выбирать эллипсоид для геодезических расчётов (WGS84, ГСК-2011).
 *
 * @see vrsa::tools::RulerTool
 */
class MeasurementForm : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurementForm (QWidget *parent = nullptr);

    inline void setStartPoint(const QPointF& point)  { mStartPos = point; };
    inline void setEndPoint  (const QPointF& point)  { mEndPos   = point; };
    void setDistance         (double distance);
    inline std::pair<QPointF, QPointF> getPoints() const { return {mStartPos, mEndPos}; }
    std::string getEllipsoid() const;
    bool isGeodesicalMode()    const;
    bool isCartesianMode ()    const;
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
