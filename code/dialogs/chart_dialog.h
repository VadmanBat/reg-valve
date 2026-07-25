#pragma once

#include <QChart>
#include <QDialog>
#include <QPen>
#include <QVector>

class QComboBox;
class QLineEdit;
class QLineSeries;
class QPushButton;
class QSpinBox;

namespace Ui {
class ChartDialog;
}

class ChartDialog : public QDialog {
    Q_OBJECT
public:
    explicit ChartDialog(QChart* chart, QWidget* parent = nullptr);
    ~ChartDialog() override;

private slots:
    void applyChanges();
    void restoreChart();

private:
    void buildSeriesEditors();
    void changeSeriesColor(int index);
    void changeSeriesStyle(int index);
    [[nodiscard]] QAbstractAxis* getAxis(Qt::Orientation orientation) const;
    static Qt::PenStyle penStyleFromIndex(int index);
    static int indexFromPenStyle(Qt::PenStyle style);

    Ui::ChartDialog* ui;
    QChart* chart_;
    QVector<QPen> initPens_;
    QVector<QPen> currentPens_;
    QVector<QLineEdit*> seriesNameEdits_;
    QVector<QPushButton*> colorButtons_;
    QVector<QSpinBox*> widthSpinBoxes_;
    QVector<QComboBox*> styleComboBoxes_;
    QVector<QLineSeries*> lineSeriesPointers_;
};
