#include "code/dialogs/chart-dialog.h"
#include "ui_chart-dialog.h"

#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLineSeries>
#include <QPushButton>
#include <QSpinBox>
#include <QValueAxis>

ChartDialog::ChartDialog(QChart* chart, QWidget* parent)
    : QDialog(parent), ui(new Ui::ChartDialog), chart_(chart) {
    ui->setupUi(this);
    ui->titleEdit->setText(chart_->title());
    if (auto* axis = getAxis(Qt::Horizontal))
        ui->xAxisLabelEdit->setText(axis->titleText());
    if (auto* axis = getAxis(Qt::Vertical))
        ui->yAxisLabelEdit->setText(axis->titleText());

    buildSeriesEditors();

    connect(ui->applyButton, &QPushButton::clicked, this, &ChartDialog::applyChanges);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ChartDialog::reject);
    connect(this, &QDialog::rejected, this, &ChartDialog::restoreChart);
}

ChartDialog::~ChartDialog() {
    delete ui;
}

QAbstractAxis* ChartDialog::getAxis(Qt::Orientation orientation) const {
    const auto axes = chart_->axes(orientation);
    return axes.isEmpty() ? nullptr : axes.first();
}

Qt::PenStyle ChartDialog::penStyleFromIndex(int index) {
    switch (index) {
    case 1:
        return Qt::DashLine;
    case 2:
        return Qt::DotLine;
    case 3:
        return Qt::DashDotLine;
    case 4:
        return Qt::DashDotDotLine;
    default:
        return Qt::SolidLine;
    }
}

int ChartDialog::indexFromPenStyle(Qt::PenStyle style) {
    switch (style) {
    case Qt::DashLine:
        return 1;
    case Qt::DotLine:
        return 2;
    case Qt::DashDotLine:
        return 3;
    case Qt::DashDotDotLine:
        return 4;
    default:
        return 0;
    }
}

void ChartDialog::buildSeriesEditors() {
    auto* layout = new QGridLayout(ui->seriesHost);
    layout->addWidget(new QLabel(tr("Название")), 0, 0);
    layout->addWidget(new QLabel(tr("Цвет")), 0, 1);
    layout->addWidget(new QLabel(tr("Толщина")), 0, 2);
    layout->addWidget(new QLabel(tr("Стиль")), 0, 3);

    int index = 0;
    for (auto* series : chart_->series()) {
        auto* lineSeries = qobject_cast<QLineSeries*>(series);
        if (!lineSeries || lineSeries->name() == QLatin1String("hor-line") ||
            lineSeries->name() == QLatin1String("ver-line"))
            continue;

        auto* nameEdit = new QLineEdit(lineSeries->name());
        auto* colorButton = new QPushButton;
        auto* widthSpin = new QSpinBox;
        auto* styleCombo = new QComboBox;

        colorButton->setObjectName(QStringLiteral("seriesColorButton"));
        colorButton->setAutoFillBackground(true);
        {
            QPalette pal = colorButton->palette();
            pal.setColor(QPalette::Button, lineSeries->pen().color());
            colorButton->setPalette(pal);
        }
        widthSpin->setRange(1, 10);
        widthSpin->setValue(lineSeries->pen().width());
        styleCombo->addItems({tr("Сплошная"), tr("Штриховая"), tr("Точечная"), tr("Штрих-точка"), tr("Штрих-точка-точка")});
        styleCombo->setCurrentIndex(indexFromPenStyle(lineSeries->pen().style()));

        connect(colorButton, &QPushButton::clicked, this, [this, index] { changeSeriesColor(index); });
        connect(widthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, index] { changeSeriesStyle(index); });
        connect(styleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this, index] { changeSeriesStyle(index); });

        ++index;
        layout->addWidget(nameEdit, index, 0);
        layout->addWidget(colorButton, index, 1);
        layout->addWidget(widthSpin, index, 2);
        layout->addWidget(styleCombo, index, 3);

        seriesNameEdits_.append(nameEdit);
        colorButtons_.append(colorButton);
        widthSpinBoxes_.append(widthSpin);
        styleComboBoxes_.append(styleCombo);
        lineSeriesPointers_.append(lineSeries);
        initPens_.append(lineSeries->pen());
    }
    currentPens_ = initPens_;
}

void ChartDialog::changeSeriesColor(int index) {
    const QColor newColor = QColorDialog::getColor(lineSeriesPointers_[index]->pen().color(), this);
    if (!newColor.isValid())
        return;
    currentPens_[index].setColor(newColor);
    lineSeriesPointers_[index]->setPen(currentPens_[index]);
    QPalette pal = colorButtons_[index]->palette();
    pal.setColor(QPalette::Button, newColor);
    colorButtons_[index]->setPalette(pal);
}

void ChartDialog::changeSeriesStyle(int index) {
    currentPens_[index].setWidth(widthSpinBoxes_[index]->value());
    currentPens_[index].setStyle(penStyleFromIndex(styleComboBoxes_[index]->currentIndex()));
    lineSeriesPointers_[index]->setPen(currentPens_[index]);
}

void ChartDialog::applyChanges() {
    chart_->setTitle(ui->titleEdit->text());
    if (auto* axis = getAxis(Qt::Horizontal))
        axis->setTitleText(ui->xAxisLabelEdit->text());
    if (auto* axis = getAxis(Qt::Vertical))
        axis->setTitleText(ui->yAxisLabelEdit->text());
    for (int i = 0; i < lineSeriesPointers_.size(); ++i) {
        lineSeriesPointers_[i]->setName(seriesNameEdits_[i]->text());
        lineSeriesPointers_[i]->setPen(currentPens_[i]);
    }
    accept();
}

void ChartDialog::restoreChart() {
    for (int i = 0; i < lineSeriesPointers_.size(); ++i)
        lineSeriesPointers_[i]->setPen(initPens_[i]);
}
