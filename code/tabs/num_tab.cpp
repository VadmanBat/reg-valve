#include "code/tabs/num_tab.h"
#include "ui_num_tab.h"

#include "code/dialogs/mod_par_dialog.h"
#include "code/util/tf_builder.hpp"

#include "numina/classes/control/transfer-function/response-lab.h"

#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

NumTab::NumTab(QWidget* parent) : QWidget(parent), ui(new Ui::NumTab) {
    ui->setupUi(this);
    installCustomWidgets();
    setupMetrics();

    charts_ = new ResponseChartBank(this);
    ui->chartsLayout->addWidget(charts_);
    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 1);

    chartsMenu_ = new QMenu(this);
    connect(chartsMenu_, &QMenu::aboutToShow, this, [this] { charts_->populateMenu(chartsMenu_); });

    auto* chartsBtn = new QToolButton(this);
    chartsBtn->setObjectName(QStringLiteral("chartsButton"));
    chartsBtn->setText(QStringLiteral("◫"));
    chartsBtn->setToolTip(tr("Отображаемые графики"));
    chartsBtn->setPopupMode(QToolButton::InstantPopup);
    chartsBtn->setMenu(chartsMenu_);
    chartsBtn->setFixedSize(36, 36);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->settingsButton), chartsBtn);

    form_->bindNameLabel(ui->nameLabel);
    form_->setTransferFunction(&currentTf_);

    connect(ui->settingsButton, &QPushButton::clicked, this, &NumTab::openSettings);
    connect(ui->addButton, &QPushButton::clicked, this, &NumTab::addTransferFunction);
    connect(ui->replaceButton, &QPushButton::clicked, this, &NumTab::replaceTransferFunction);
    connect(ui->clearButton, &QPushButton::clicked, this, &NumTab::clearCharts);
}

NumTab::~NumTab() {
    delete ui;
}

void NumTab::installCustomWidgets() {
    form_ = new TranFuncForm(6, 6, QStringLiteral("W(p) = "), ui->formHost);
    auto* formLayout = new QVBoxLayout(ui->formHost);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addWidget(form_);

    metrics_ = new RegulationWidget(3, 2, ui->metricsHost);
    auto* metricsLayout = new QVBoxLayout(ui->metricsHost);
    metricsLayout->setContentsMargins(0, 0, 0, 0);
    metricsLayout->addWidget(metrics_);
}

void NumTab::setupMetrics() {
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"),
        QStringLiteral("ω<sub>n</sub>:"),
        QStringLiteral("t<sub>н</sub>:"),
        QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("ζ:"),
        QStringLiteral("h<sub>уст</sub>:"),
    });
    metrics_->setPrecisions({2, 4, 2, 4, 4, 2});
    metrics_->setColors({{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}});
}

void NumTab::showError(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка ввода"), message);
}

void NumTab::openSettings() {
    ModParDialog dialog(modelParam_, this);
    if (dialog.exec() == QDialog::Accepted)
        modelParam_ = dialog.data();
}

void NumTab::openChartsMenu() {
    if (chartsMenu_)
        chartsMenu_->popup(QCursor::pos());
}

void NumTab::addTransferFunction() {
    auto num = form_->numerator();
    auto den = form_->denominator();
    if (!tf_builder::validInput(num, den)) {
        if (den.empty())
            showError(tr("Знаменатель НЕ может быть равен нулю!"));
        else if (den.size() == 1)
            showError(tr("Порядок знаменателя НЕ может быть меньше первого!"));
        else
            showError(tr("Порядок числителя НЕ может быть больше порядка знаменателя!"));
        return;
    }

    try {
        currentTf_ = tf_builder::plant(std::move(num), std::move(den), form_->hasDelay() ? form_->delayTime() : 0.0,
                                       modelParam_.approxOrder);
        form_->setTransferFunction(&currentTf_);

        charts_->appendFromTf(currentTf_, modelParam_, form_->linkName());

        numina::ResponseLab lab(currentTf_);
        const auto q = lab.evaluate();
        if (q.is_settled) {
            metrics_->updateValues({q.settling_time, q.natural_frequency, q.rise_time, q.cut_frequency,
                                    q.damping_ratio, q.steady_state});
        } else {
            metrics_->updateValues({});
        }
    } catch (const std::exception& ex) {
        showError(QString::fromUtf8(ex.what()));
    }
}

void NumTab::replaceTransferFunction() {
    if (charts_->empty()) {
        addTransferFunction();
        return;
    }
    auto num = form_->numerator();
    auto den = form_->denominator();
    if (!tf_builder::validInput(num, den)) {
        showError(tr("Некорректная передаточная функция"));
        return;
    }
    try {
        currentTf_ = tf_builder::plant(std::move(num), std::move(den), form_->hasDelay() ? form_->delayTime() : 0.0,
                                       modelParam_.approxOrder);
        form_->setTransferFunction(&currentTf_);
        charts_->replaceLastFromTf(currentTf_, modelParam_, form_->linkName());

        numina::ResponseLab lab(currentTf_);
        const auto q = lab.evaluate();
        if (q.is_settled) {
            metrics_->updateValues({q.settling_time, q.natural_frequency, q.rise_time, q.cut_frequency,
                                    q.damping_ratio, q.steady_state});
        } else {
            metrics_->updateValues({});
        }
    } catch (const std::exception& ex) {
        showError(QString::fromUtf8(ex.what()));
    }
}

void NumTab::clearCharts() {
    charts_->clearAll();
    metrics_->updateValues({});
}
