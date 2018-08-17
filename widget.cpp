#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    widget_init();
}

void Widget::slot_save()
{
    str_file_name = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                 QDir::homePath() + "/untitled.png",
                                                 tr("*.png;;*.jpg;;*.jpeg;;*.bmp"));
    if(str_file_name.isEmpty() || str_file_name.isNull())
    {
        return;
    }
    image.save(str_file_name);
}

void Widget::set_color()
{
    color = QColorDialog::getColor();
    pen.setColor(color);
    brush.setColor(color);
    if(draw_eraser)
    {
        draw_eraser = false;
        btn_rectangle->setEnabled(true);
        btn_elipse->setEnabled(true);
        if(!pen.color().isValid())
        {
           pen.setColor(Qt::black);
           brush.setColor(pen.color());
        }
    }
}

void Widget::slot_fill_with_color()
{
    is_filled_with_color = true;
}

void Widget::slot_aliasing()
{
    if(aliasing->checkState() == Qt::Checked)
        is_aliasing_on = true;
    else
        is_aliasing_on = false;
}

void Widget::set_size(int a)
{
    pen.setWidth(a);
}

void Widget::slot_rectangle()
{
    draw_rectangle = true;
}

void Widget::slot_elipse()
{
    draw_elipse = true;
}

void Widget::slot_eraser()
{
    if(draw_elipse || draw_rectangle)
    {
        draw_elipse = false;
        draw_rectangle = false;
    }
    pen.setColor(QColor(Qt::white));
    brush.setColor(pen.color());
    btn_rectangle->setEnabled(false);
    btn_elipse->setEnabled(false);
    draw_eraser = true;
}

void Widget::slot_open()
{
    str_file_name = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                 QDir::homePath(),
                                                 tr("*.png;;*.jpg;;*.jpeg;;*.bmp"));
    if(str_file_name.isEmpty() || str_file_name.isNull())
    {
        return;
    }
    QImage image2;
    image2.load(str_file_name);
    image = image2.scaled(800,400, Qt::KeepAspectRatio);
    lbl->setPixmap(QPixmap::fromImage(image));
}

void Widget::slot_resize_canvas()
{
    QDialog *dialog = new QDialog(this);
    dialog->setMaximumSize(QSize(280, 100));
    dialog->setMinimumSize(QSize(280, 100));
    QFormLayout form(dialog);
    QLineEdit * lineEdit1 = new QLineEdit();
    QLineEdit * lineEdit2 = new QLineEdit();

    lineEdit1->setValidator(new QIntValidator(20, 1366, this));
    lineEdit2->setValidator(new QIntValidator(20, 768, this));

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                        | QDialogButtonBox::Cancel, dialog);
    QString width = QString("Width: ");
    QString height = QString("Height: ");

    form.addRow(width, lineEdit1);
    form.addRow(height, lineEdit2);
    form.addRow(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    lineEdit1->setFocus(Qt::OtherFocusReason);

    int result = dialog->exec();
    if(result == QDialog::Accepted)
    {
        QSize canvas = QSize(lineEdit1->text().toInt(), lineEdit2->text().toInt());
        image = QImage(image.scaled(canvas, Qt::KeepAspectRatio));
        lbl->setPixmap(QPixmap::fromImage(image));
    }
}

void Widget::paintEvent(QPaintEvent *)
{
    if(is_pressed)
    {
        if(draw_rectangle)
        {
            if(begin != end)
            {
                QPainter painter;
                painter.begin(&image);
                if(is_aliasing_on)
                    painter.setRenderHint(QPainter::Antialiasing);
                painter.setPen(pen);
                int w = (end.x() - begin.x());
                int h = (end.y() - begin.y());
                painter.drawRect(begin.x(),begin.y(),w,h);
                painter.end();
                is_pressed = false;
                draw_rectangle = false;
                is_something_modified = true;
                begin.setX(0);
                begin.setY(0);
                end=begin;
                lbl->setPixmap(QPixmap::fromImage(image));
                return;
            }
        }
        if(draw_elipse)
        {
            if(begin != end)
            {
                QPainter painter;
                painter.begin(&image);
                if(is_aliasing_on)
                    painter.setRenderHint(QPainter::Antialiasing);
                painter.setPen(pen);
                int w = (end.x() - begin.x());
                int h = (end.y() - begin.y());
                painter.drawEllipse(begin.x(),begin.y(),w,h);
                painter.end();
                is_pressed = false;
                draw_elipse = false;
                is_something_modified = true;
                begin.setX(0);
                begin.setY(0);
                end=begin;
                lbl->setPixmap(QPixmap::fromImage(image));
                return;
            }
        }

        QPainter painter;
        painter.begin(&image);
        if(is_aliasing_on)
            painter.setRenderHint(QPainter::Antialiasing);
        if (list.size() < 2) return;
        painter.setPen(pen);
        QList<QPoint>::const_iterator it = list.begin();
        QPoint start = *it;
        it++;
        while(it != list.end())
        {
            QPoint end = *it;
            painter.drawLine(start,end);
            start = end;
            it++;
        }
        is_something_modified = true;
        painter.end();
        lbl->setPixmap(QPixmap::fromImage(image));
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(!is_pressed)
    {
        return;
    }

    if(draw_rectangle)
    {
        this->update();
        return;
    }

    if(draw_elipse)
    {
        this->update();
        return;
    }
    if(is_filled_with_color)
    {
        is_filled_with_color = false;
        return;
    }
    list.push_back(event->pos());
    this->update();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
 if (event->button() == Qt::LeftButton)
    {
        if(draw_rectangle)
        {
            QToolTip::showText(event->globalPos(),"Keep Button pressed and"
                                                  " Move Cursor to"
                                                  " the next position",
                                                  this
                                                  );
            is_pressed = true;
            begin=event->pos();
            end=begin;
            return;
        }
        if(draw_elipse)
        {
            QToolTip::showText(event->globalPos(),"Keep Button pressed and"
                                                  " Move Cursor to"
                                                  " the next position",
                                                  this
                                                  );
            is_pressed = true;
            begin=event->pos();
            end=begin;
            return;
        }
        if(is_filled_with_color) {
            QToolTip::showText(event->globalPos(),"Keep Button pressed and"
                                                  " Move Cursor to"
                                                  " the next position",
                                                  this
                                                  );
            is_pressed = true;
            begin = event->pos();
            fill_with_color(&image, begin, image.pixelColor(begin), color);
            end=begin;
            lbl->setPixmap(QPixmap::fromImage(image));
            return;
        }
        is_pressed = true;
        list.push_back(event->pos());
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(draw_rectangle)
        {
            end = event->pos();
            this->update();
            return;
        }
        if(draw_elipse)
        {
            end = event->pos();
            this->update();
            return;
        }
        if(is_filled_with_color)
        {
            end = event->pos();
            this->update();
            return;
        }
        is_pressed = false;
        list.push_back(event->pos());
        this->update();
        list.clear();
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    if(is_something_modified)
    {
        QMessageBox messageBox(this);
        messageBox.setText("Do you want to save changes before close app?");
        messageBox.setWindowTitle("Exit");
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int button_pressed = messageBox.exec();
        if(button_pressed == QMessageBox::Yes){
            slot_save();
            event->accept();
        } else if(button_pressed == QMessageBox::No)
            event->accept();
    }
    else {
        event->accept();
    }
}

void Widget::fill_with_color(QImage *currentImage, QPoint point, QColor oldColor, QColor newColor)
{
    if(currentImage->pixelColor(point) == newColor)
        return;

    if (point.y() < 0 || point.y() > currentImage->height() - 1 || point.x() < 0 || point.x() > currentImage->width() - 1)
        return;

    QVector<QPoint>* stack = new QVector<QPoint>();
    stack->push_back(point);

    while (stack->size() > 0)
    {
        QPoint p = stack->back();
        stack->pop_back();
        int x = p.x();
        int y = p.y();
        if (y < 0 || y > currentImage->height() - 1 || x < 0 || x > currentImage->width() - 1)
            continue;

        if (currentImage->pixelColor(x, y) == oldColor)
        {
            currentImage->setPixelColor(x, y, newColor);
            stack->push_back(QPoint(x + 1, y));
            stack->push_back(QPoint(x - 1, y));
            stack->push_back(QPoint(x, y + 1));
            stack->push_back(QPoint(x, y - 1));
        }
    }
}

void Widget::widget_init()
{
    lbl = new QLabel;

    ///--Layouts and sizes of widgets------------
    resize(800, 400);
    setWindowTitle("My Paint");
    setWindowIcon(QIcon(":/pics/img/paint-palette.png"));

    lbl->setFrameShape(QFrame::Panel);
    lbl->setFrameShadow(QFrame::Plain);
    lbl->setLineWidth(1);
    h_layout = new QHBoxLayout;
    h_layout->addWidget(lbl, 0, Qt::AlignTop | Qt::AlignLeft);

    toolbar_init();

    ///--Signals & Slots--------------------------------
    connect(btn_save,SIGNAL(clicked(bool)),SLOT(slot_save()));
    connect(btn_open,SIGNAL(clicked(bool)),SLOT(slot_open()));
    connect(btn_color,SIGNAL(clicked(bool)),SLOT(set_color()));
    connect(btn_fill_with_color, SIGNAL(clicked(bool)),SLOT(slot_fill_with_color()));
    connect(btn_resize_canvas, SIGNAL(clicked(bool)),SLOT(slot_resize_canvas()));
    connect(btn_eraser,SIGNAL(clicked(bool)),SLOT(slot_eraser()));
    connect(btn_rectangle,SIGNAL(clicked(bool)),SLOT(slot_rectangle()));
    connect(btn_elipse,SIGNAL(clicked(bool)),SLOT(slot_elipse()));
    connect(aliasing, SIGNAL(stateChanged(int)),SLOT(slot_aliasing()));
    connect(slider,SIGNAL(valueChanged(int)),SLOT(set_size(int)));

    ///--Tools for paint-------------------------
    image = QImage(800,400,QImage::Format_RGB32);
    image.fill(Qt::white);
    lbl->setPixmap(QPixmap::fromImage(image));
    color = Qt::black;
    brush.setColor(color);
    pen.setColor(color);

    ///--Flags for elipse & rectangle----------------------------
    draw_rectangle = false;
    draw_elipse = false;
    draw_eraser = false;
    is_filled_with_color = false;
    is_aliasing_on = false;
    is_something_modified = false;
}

void Widget::toolbar_init()
{
    ///--Slider-----------------------
    slider = new QSlider(Qt::Horizontal);
    slider->setToolTip("Pen width ");
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTracking(true);

    slider->setMaximumHeight(20);
    slider->setMaximumWidth(120);
    slider->setMinimum(1);
    slider->setMaximum(100);

    ///--Buttons-----------------------
    btn_save = new QPushButton;
    btn_save->setToolTip("Save");
    btn_save->setIcon(QIcon(":/pics/img/save-file.png"));
    btn_save->setIconSize(QSize(60,20));

    btn_open = new QPushButton;
    btn_open->setToolTip("Open");
    btn_open->setIcon(QIcon(":/pics/img/open-folder.png"));
    btn_open->setIconSize(QSize(60,20));

    btn_color = new QPushButton;
    btn_color->setToolTip("Set pen&color");
    btn_color->setIcon(QIcon(":/pics/img/color.png"));
    btn_color->setIconSize(QSize(60,20));

    btn_fill_with_color = new QPushButton;
    btn_fill_with_color->setToolTip("Fill with color");
    btn_fill_with_color->setIcon(QIcon(":/pics/img/paint-bucket.png"));
    btn_fill_with_color->setIconSize(QSize(60,20));

    btn_resize_canvas = new QPushButton;
    btn_resize_canvas->setToolTip("Resize canvas");
    btn_resize_canvas->setIcon(QIcon(":/pics/img/resize-canvas.png"));
    btn_resize_canvas->setIconSize(QSize(60,20));

    btn_rectangle = new QPushButton;
    btn_rectangle->setToolTip("Select Rectangle");
    btn_rectangle->setIcon(QIcon(":/pics/img/rectangle.png"));
    btn_rectangle->setIconSize(QSize(60,20));

    btn_elipse = new QPushButton;
    btn_elipse->setToolTip("Select Elipse");
    btn_elipse->setIcon(QIcon(":/pics/img/ellipse.png"));
    btn_elipse->setIconSize(QSize(60,20));

    btn_eraser = new QPushButton;
    btn_eraser->setToolTip("Select Eraser");
    btn_eraser->setIcon(QIcon(":/pics/img/eraser.png"));
    btn_eraser->setIconSize(QSize(60,20));

    btn_elipse->setFixedSize(80,30);
    btn_rectangle->setFixedSize(80,30);
    btn_save->setFixedSize(80,30);
    btn_open->setFixedSize(80,30);
    btn_color->setFixedSize(80,30);
    btn_eraser->setFixedSize(80,30);
    btn_fill_with_color->setFixedSize(80,30);

    btn_elipse->setFlat(true);
    btn_rectangle->setFlat(true);
    btn_save->setFlat(true);
    btn_open->setFlat(true);
    btn_color->setFlat(true);
    btn_eraser->setFlat(true);
    btn_fill_with_color->setFlat(true);
    btn_resize_canvas->setFlat(true);

    QToolBar *qbar = new QToolBar(this);
    qbar->setFixedWidth(110);
    qbar->setAutoFillBackground(false);
    qbar->setOrientation(Qt::Vertical);
    qbar->setGeometry(this->width() - 120, 0, 120, this->height());
    qbar->setMovable(true);
    qbar->setFloatable(true);
    qbar->setAllowedAreas(Qt::RightToolBarArea);

    QVBoxLayout *v_btn_layout = new QVBoxLayout;
    v_btn_layout->addWidget(btn_save, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(btn_open, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(btn_color, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(btn_fill_with_color, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(btn_eraser, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(btn_resize_canvas, 0, Qt::AlignCenter);

    aliasing = new QCheckBox("Aliasing");
    QListWidget *geom_objs_list = new QListWidget;
    QListWidgetItem *ellipse = new QListWidgetItem(geom_objs_list);
    QListWidgetItem *rectangle = new QListWidgetItem(geom_objs_list);
    geom_objs_list->setFixedHeight(60);
    geom_objs_list->setPalette(palette().color(QPalette::Window));
    ellipse->setSizeHint(btn_elipse->minimumSizeHint());
    rectangle->setSizeHint(btn_rectangle->minimumSizeHint());
    geom_objs_list->addItem(ellipse);
    geom_objs_list->addItem(rectangle);
    geom_objs_list->setItemWidget(ellipse, btn_elipse);
    geom_objs_list->setItemWidget(rectangle, btn_rectangle);

    v_btn_layout->addWidget(geom_objs_list, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(aliasing, 0, Qt::AlignCenter);
    v_btn_layout->addWidget(slider, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(v_btn_layout);
    qbar->addWidget(widget);
    h_layout->addWidget(qbar);
    h_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(h_layout);
}

Widget::~Widget()
{
    delete ui;
}
