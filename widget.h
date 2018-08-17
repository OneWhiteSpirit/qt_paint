#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QSlider>
#include <QToolBar>
#include <QCheckBox>
#include <QMessageBox>
#include <QColor>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QDialog>
#include <QToolTip>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QList>
#include <QVector>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QLineEdit>
#include <QIntValidator>
#include <QListWidget>
#include <QListWidgetItem>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

private:
    QImage image;

    QLabel *lbl;
    QHBoxLayout *h_layout;
    QSlider *slider;
    QPushButton *btn_save;
    QPushButton *btn_open;
    QPushButton *btn_color;
    QPushButton *btn_rectangle;
    QPushButton *btn_elipse;
    QPushButton *btn_eraser;
    QPushButton *btn_fill_with_color;
    QPushButton *btn_resize_canvas;
    QCheckBox *aliasing;

    QColor color;
    QBrush brush;
    QPen pen;
    QList<QPoint> list;

    QString str_file_name;

    ///For rectangle
    QPoint begin;
    QPoint end;

    ///Flags-----------
    bool is_pressed;
    bool draw_rectangle;
    bool draw_elipse;
    bool draw_eraser;
    bool is_filled_with_color;
    bool is_aliasing_on;
    bool is_something_modified;

    void fill_with_color(QImage*, QPoint, QColor, QColor);
    void widget_init();
    void toolbar_init();

    Ui::Widget *ui;

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void slot_save();
    void set_color();
    void slot_fill_with_color();
    void slot_aliasing();
    void set_size(int);
    void slot_rectangle();
    void slot_elipse();
    void slot_eraser();
    void slot_open();
    void slot_resize_canvas();

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void closeEvent(QCloseEvent *) override;
};

#endif // WIDGET_H
