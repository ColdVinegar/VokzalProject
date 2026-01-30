#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QPushButton>
#include <QDialogButtonBox>
#include "addingdialog.h"
#include "deldialog.h"
#include "core/station.h"
#include "core/discount.h"
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_confirmAdd_clicked();
    void on_editDataButton_clicked();
    void on_confirmDel_clicked();
    void on_ticketByPassButton_clicked();
    void on_passesByTariffButton_clicked();
    void on_openBDButton_clicked();
    void on_saveBDButton_clicked();

    void onAddingDialogAccepted(int mode, QString selfindex, const QVariantMap& data);
    void onEditDialogAccepted(int mode, QString selfindex, const QVariantMap& data);

    void onDelDialogAccepted(int mode, int selectedIndex);

    void on_checkBoxAutosave_checkStateChanged(const Qt::CheckState &arg1);

    void on_restoreDataButtons_clicked(QAbstractButton *button);

    void on_totalRevenueButton_clicked();

private:
    Ui::MainWindow *ui;
    Station station;
    DiscountManager discountManager;
    bool autoMode;
    bool wasSaved;

    // Модели для таблиц
    QStandardItemModel *tariffsModel;
    QStandardItemModel *discountsModel;
    QStandardItemModel *passengersModel;
    QStandardItemModel *ticketsModel;

    QSortFilterProxyModel* tariffsProxyModel;
    QSortFilterProxyModel* discountsProxyModel;
    QSortFilterProxyModel* passesProxyModel;
    QSortFilterProxyModel* ticketsProxyModel;

    // Методы инициализации
    void setupModels();
    void setupDiscountManager();
    void loadSampleData();

    // Обновление таблиц
    void refreshTariffsTable();
    void refreshDiscountsTable();
    void refreshPassengersTable();
    void refreshTicketsTable();
    void refreshAllTables();

    // Вспомогательные методы
    void showStatusMessage(const QString& message, int timeout = 3000);
    bool validatePassport(const QString& passportStr, int& passport) const;
    bool validatePrice(const QString& priceStr, float& price) const;
    bool validatePerc(float perc) const;


    // Получение данных из таблиц
    QString getSelectedTariffName() const;
    int getSelectedPassengerPassport() const;
    int getSelectedTicketIndex() const;

    // Методы для работы с данными
    bool addPassenger(const QVariantMap& data);
    bool addTariff(const QVariantMap& data);
    bool addTicket(const QVariantMap& data);
    bool addDiscount(const QVariantMap& data);

    bool deletePassenger(int passport);
    bool deleteTariff(const QString& name);
    bool deleteTicket(int index);
    bool deleteDiscount(const QString& name);

    bool askToSave(const QString& message, bool noCancel);
};

#endif // MAINWINDOW_H
