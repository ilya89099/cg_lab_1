#include "graphicwidget.h"

GraphicWidget::GraphicWidget(QWidget* parent)
    : QWidget(parent)
{

}


void GraphicWidget::SetParameters(QHash<QString, double> in_params) {
    QString param_prefix = "param_";
    params.n = in_params[param_prefix + "n"];
    params.a = in_params[param_prefix + "a"];
    params.B = in_params[param_prefix + "B"];
    params.r = in_params[param_prefix + "r"];
    update();
}


void GraphicWidget::PaintAxis(QPainter& p, QPen& pen) {

    pen.setColor(Qt::black);
    p.setPen(pen);

    p.drawLine(QLine((-width() - params.x_t) * (1 / params.scale), 0, (width() - params.x_t) * (1 / params.scale), 0));
    p.drawLine(QLine(0, (-height() - params.y_t) * (1 / params.scale), 0, (height() - params.y_t) * (1 / params.scale)));

    int horizontal_points = width() / (2 * params.scale);
    int closest_ten_power = std::floor(std::log10(horizontal_points * (3.0/4.0)));
    double center_x = -params.x_t / params.scale;
    double center_y = -params.y_t / params.scale;

    int start_x = center_x / (std::pow(10, closest_ten_power));
    int start_y = center_y / (std::pow(10, closest_ten_power));

    QFont label_font = p.font();
    label_font.setPixelSize(std::max(15.0 / params.scale, 1.0));
    p.setFont(label_font);
    for (int i = 0; i <= 15; ++i) {
        int center = start_x * std::pow(10, closest_ten_power);
        int delta = i * std::pow(10, closest_ten_power);
        p.drawText(QPoint(center + delta, 0), QString::number(center + delta));
        p.drawText(QPoint(center - delta, 0), QString::number(center - delta));
    }
    for (int i = 0; i <= 15; ++i) {
        int center = start_y * std::pow(10, closest_ten_power);
        int delta = i * std::pow(10, closest_ten_power);
        p.drawText(QPoint(0, center + delta), QString::number(center + delta));
        p.drawText(QPoint(0, center - delta), QString::number(center - delta));
    }
}

void GraphicWidget::paintEvent(QPaintEvent* e) {
    QPainter ptr(this);
    ptr.setRenderHint(ptr.Antialiasing);
    QPen pen;
    pen.setCosmetic(true);
    pen.setWidth(2);
    ptr.setPen(pen);
    int x_c = width() / 2;
    int y_c = height() / 2;



    QMatrix4x4 transform_matrix = QTransform{params.scale,    0,  0,
                                             0,   params.scale,   0,
                                             x_c + params.x_t, y_c + params.y_t,1};

    ptr.setTransform(transform_matrix.toTransform());
    PaintAxis(ptr, pen);

    pen.setColor(Qt::blue);
    ptr.setPen(pen);
    double teta = qDegreesToRadians(double(params.r));

    transform_matrix *= QMatrix4x4(QTransform({cos(teta),   sin(teta),  0,
                                               -sin(teta),   cos(teta),  0,
                                               0,            0,          1}));
    ptr.setTransform(transform_matrix.toTransform());

    for (int i = 0; i < 4; ++i) {
        QVector4D col = QMatrix4x4(ptr.transform()).column(i);
        for (int j = 0; j < 4; ++j) {
            std::cout << col[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;

    QVector<QPointF> graphic = appr.GetPoints(params.n, params.a, params.B);
    ptr.drawPolyline(graphic.data(), graphic.size());

}


void GraphicWidget::wheelEvent(QWheelEvent *e) {
    double wheel_scale = e->angleDelta().y() / 500.0;
    double min_scale = 0.01, max_scale = 20.0;
    params.scale = std::min(std::max(min_scale, params.scale + wheel_scale), max_scale);
    update();
}

void GraphicWidget::mouseMoveEvent(QMouseEvent* e) {
    if (!mouse_pressed) {
        return;
    }
    params.x_t += (e->x() - prev_x);
    params.y_t += (e->y() - prev_y);
    prev_x = e->x();
    prev_y = e->y();
    update();
}

void GraphicWidget::mousePressEvent(QMouseEvent* e) {
    mouse_pressed = true;
    prev_x = e->x();
    prev_y = e->y();
}

void GraphicWidget::mouseReleaseEvent(QMouseEvent* e) {
    mouse_pressed = false;
}

