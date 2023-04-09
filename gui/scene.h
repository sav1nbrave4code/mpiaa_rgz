#ifndef RGZ_SAVIN_SCENE_H
#define RGZ_SAVIN_SCENE_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVector>
#include <QRectF>
#include <QPaintEvent>
#include <QString>
#include <QMouseEvent>
#include <QPoint>

#include "../model/graph/graph.h"

class Scene : public QWidget
{
    Q_OBJECT

public:
    struct Vertex
    {
        int number {};
        int x      {};
        int y      {};
    };

    struct Wall
    {
        Wall(Vertex v1, Vertex v2);

        Vertex v1;
        Vertex v2;
    };

    [[nodiscard]] auto intersect(double a, double b, double c, double d) const noexcept -> bool;
    [[nodiscard]] auto intersect(Vertex a, Vertex b, Vertex c, Vertex d) const noexcept -> bool;
    [[nodiscard]] auto area(Vertex a, Vertex b, Vertex c) const noexcept -> double;

public:
    explicit Scene(QWidget *parent = nullptr);
    ~Scene() override = default;

protected:
    auto paintEvent(QPaintEvent* event) -> void override;
    auto mousePressEvent(QMouseEvent* event) -> void override;

private slots:
    auto onRand() -> void;
    auto onImport() -> void;
    auto onExport() -> void;

private:
    auto clearPath() -> void;
    auto updateSceneComponents() -> void;

private:
    const QVector<QString> colors {"white", "blue", "green", "cyan", "yellow"};

private:
    QPushButton*   rand_button   {};
    QPushButton*   import_button {};
    QPushButton*   export_button {};
    QSpinBox*      count_spinbox {};
    QVector<QRect> rectangles    {};
    QPoint         start_pos     {};
    QPoint         finish_pos    {};
    bool           start_set     {false};
    bool           finish_set    {false};

    QVector<QPoint>  path             {};
    QVector<Vertex>  vertices         {};
    QVector<Wall>    walls            {};
    QVector<Wall>    diagonals        {};
    Graph            scene_graph      {};
    Graph            visibility_graph {};
};

#endif //RGZ_SAVIN_SCENE_H
