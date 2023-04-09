#include "scene.h"

#include <QDebug>
#include <QPainter>
#include <QBrush>
#include <QRandomGenerator>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "../model/graph/shortest_path.h"

auto Scene::intersect(double a, double b, double c, double d) const noexcept -> bool
{
    if (a > b)
    {
        std::swap(a, b);
    }

    if (c > d)
    {
        std::swap(c, d);
    }

    return std::max(a, c) <= std::min(b, d);
}

auto Scene::intersect(const Vertex a, const Vertex b, const Vertex c, const Vertex d) const noexcept -> bool
{
    return intersect(a.x, b.x, c.x, d.x) &&
           intersect(a.y, b.y, c.y, d.y) &&
           area(a, b, c) * area (a, b, d) < 0 &&
           area(c, d, a) * area(c, d, b) < 0;
}

auto Scene::area(Vertex a, Vertex b, Vertex c) const noexcept -> double
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

Scene::Wall::Wall(Scene::Vertex v1, Scene::Vertex v2):
    v1{v1},
    v2{v2}
{}


Scene::Scene(QWidget *parent):
    QWidget{parent},
    rand_button{new QPushButton{this}},
    import_button{new QPushButton{this}},
    export_button{new QPushButton{this}},
    count_spinbox{new QSpinBox{this}}
{
    connect(rand_button, &QPushButton::clicked, this, &Scene::onRand);
    connect(export_button, &QPushButton::clicked, this, &Scene::onExport);
    connect(import_button, &QPushButton::clicked, this, &Scene::onImport);

    count_spinbox->setGeometry(1100, 470, 100, 50);
    count_spinbox->setMinimum(1);
    count_spinbox->setMaximum(50);

    rand_button->setGeometry(1100, 520, 100, 50);
    import_button->setGeometry(1100, 570, 100, 50);
    export_button->setGeometry(1100, 620, 100, 50);
    rand_button->setText("Rand");
    import_button->setText("Import");
    export_button->setText("Export");

    setStyleSheet("background-color: rgb(50, 50, 50);"
                  "color: rgb(240, 240, 240);");
    resize(1280, 720);
    setFixedSize(size());
}

auto Scene::clearPath() -> void
{
    if (finish_set)
    {
        vertices.erase(vertices.cend() - 1);
    }

    if (start_set)
    {
        vertices.erase(vertices.cend() - 1);
    }

    start_set        = false;
    finish_set       = false;
    start_pos        = {0, 0};
    finish_pos       = {0, 0};
    visibility_graph = scene_graph;

    path.clear();
    repaint();
}

auto Scene::updateSceneComponents() -> void
{
    scene_graph.clear();
    visibility_graph.clear();
    vertices.clear();

    clearPath();

    for (auto i {0}; i < rectangles.size(); ++i)
    {
        scene_graph.add_edge(4 * i + 0, 4 * i + 1, rectangles[i].width());
        scene_graph.add_edge(4 * i + 1, 4 * i + 2, rectangles[i].height());
        scene_graph.add_edge(4 * i + 2, 4 * i + 3, rectangles[i].width());
        scene_graph.add_edge(4 * i + 3, 4 * i + 0, rectangles[i].height());

        vertices.emplace_back(Vertex{4 * i + 0,
                                     rectangles[i].x(), rectangles[i].y()});
        vertices.emplace_back(Vertex{4 * i + 1,
                                      rectangles[i].x() + rectangles[i].width(), rectangles[i].y()});
        vertices.emplace_back(Vertex{4 * i + 2,
                                      rectangles[i].x() + rectangles[i].width(), rectangles[i].y() + rectangles[i].height()});
        vertices.emplace_back(Vertex{4 * i + 3,
                                     rectangles[i].x(), rectangles[i].y() + rectangles[i].height()});

        walls.emplace_back(Wall{{4 * i + 0, rectangles[i].x(), rectangles[i].y()},
                                {4 * i + 1, rectangles[i].x() + rectangles[i].width(), rectangles[i].y()}});
        walls.emplace_back(Wall{{4 * i + 1, rectangles[i].x() + rectangles[i].width(), rectangles[i].y()},
                                {4 * i + 2, rectangles[i].x() + rectangles[i].width(), rectangles[i].y() + rectangles[i].height()}});
        walls.emplace_back(Wall{{4 * i + 2, rectangles[i].x() + rectangles[i].width(), rectangles[i].y() + rectangles[i].height()},
                                {4 * i + 3, rectangles[i].x(), rectangles[i].y() + rectangles[i].height()}});
        walls.emplace_back(Wall{{4 * i + 3, rectangles[i].x(), rectangles[i].y() + rectangles[i].height()},
                                {4 * i + 0, rectangles[i].x(), rectangles[i].y()}});

        diagonals.emplace_back(Wall{{4 * i + 0, rectangles[i].x(), rectangles[i].y()},
                                    {4 * i + 2, rectangles[i].x() + rectangles[i].width(), rectangles[i].y() + rectangles[i].height()}});
        diagonals.emplace_back(Wall{{4 * i + 1, rectangles[i].x() + rectangles[i].width(), rectangles[i].y()},
                                    {4 * i + 3, rectangles[i].x(), rectangles[i].y() + rectangles[i].height()}});
    }

    visibility_graph = scene_graph;
}

auto Scene::onRand() -> void
{
    rectangles.clear();

    clearPath();

    for (auto i {0}; i < count_spinbox->value(); ++i)
    {
        auto x {QRandomGenerator::global()->bounded(1080)};
        auto y {QRandomGenerator::global()->bounded(520)};
        auto width {QRandomGenerator::global()->bounded(200)};
        auto height {QRandomGenerator::global()->bounded(200)};
        auto intersection {false};

        if (width < 20)
        {
            width += 20;
        }

        if (height < 20)
        {
            height += 20;
        }

        for (const auto& rect : rectangles)
        {
            if (rect.intersects(QRect{x, y, width, height}))
            {
                intersection = true;

                break;
            }
        }

        if (intersection)
        {
            continue;
        }

        rectangles.push_back(QRect{x, y, width, height});
    }

    updateSceneComponents();
    repaint();
}

auto Scene::onImport() -> void
{
    QFile jsonFile   {QFileDialog::getOpenFileName(this, tr("Choose config"),
                                                         "/home", tr("*.json"))};

    jsonFile.open(QFile::ReadOnly | QIODevice::Text);

    QByteArray      data       {jsonFile.readAll()};
    QJsonParseError parseError {};
    QJsonDocument   config     {QJsonDocument::fromJson(data, &parseError)};
    QJsonArray      rectsArray {config.array()};

    jsonFile.close();

    if (parseError.error != QJsonParseError::NoError)
    {
        QMessageBox::warning(this, "Warning", "Error while parse json: " + parseError.errorString());

        return;
    }

    if (rectsArray.size() != 1 && !rectsArray[0].isArray())
    {
        QMessageBox::warning(this, "Warning", "No array were presented in configuration");

        return;
    }

    rectangles.clear();

    //  Bug in 6.4.2 qt version with reading "pure" json array like [], without {[]}
    rectsArray = rectsArray[0].toArray();

    for (const auto& objectRef : rectsArray)
    {
        rectangles.emplace_back(QRect{objectRef.toObject().value("x").toInt(),
                                      objectRef.toObject().value("y").toInt(),
                                      objectRef.toObject().value("width").toInt(),
                                      objectRef.toObject().value("height").toInt()});
    }

    for (const auto& current_rect : rectangles)
    {
        for (const auto& rect : rectangles)
        {
            if (current_rect.intersects(rect) && current_rect != rect)
            {
                QMessageBox::warning(this, "Warning", "Some rects intersected in configuration");

                return;
            }
        }
    }

    updateSceneComponents();
    repaint();
}

auto Scene::onExport() -> void
{
    QJsonDocument config     {};
    QJsonArray    rectsArray {};
    QFile         jsonFile   {"config.json"};
    QString       dir        {QFileDialog::getExistingDirectory(this,
                              tr("Choose directory"), "/home",
                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)};

    jsonFile.setFileName(dir + "/" + jsonFile.fileName());

    for (const auto& rect : rectangles)
    {
        QJsonObject rectObject;

        rectObject.insert("x", rect.x());
        rectObject.insert("y", rect.y());
        rectObject.insert("width", rect.width());
        rectObject.insert("height", rect.height());

        rectsArray.push_back(rectObject);
    }

    config.setArray(rectsArray);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(config.toJson());
}

auto Scene::paintEvent(QPaintEvent* event) -> void
{
    QPainter painter {this};

    for (auto i {0}; i < rectangles.size(); ++i)
    {
        QColor color {colors[i % colors.size()]};

        painter.setBrush(QBrush{color});
        painter.setPen(QPen{"black"});
        painter.drawRect(rectangles[i]);
    }

    painter.setBrush(QBrush{"red"});
    painter.setPen(QPen{"red"});

    if (start_set)
    {
        painter.drawEllipse(start_pos, 5, 5);
    }

    if (finish_set)
    {
        painter.drawEllipse(finish_pos, 5, 5);
    }

    for (int i {0}; i < path.size() - 1; ++i)
    {
        painter.drawLine(path[i], path[i + 1]);
    }

    event->accept();
}

auto Scene::mousePressEvent(QMouseEvent* event) -> void
{
    if (event->button() == Qt::MouseButton::RightButton)
    {
        clearPath();

        return;
    }

    if (!start_set)
    {
        start_pos = event->pos();
        start_set = true;

        visibility_graph.add_vertex(static_cast<int>(vertices.size()));
        vertices.push_back(Vertex{static_cast<int>(vertices.size()), start_pos.x(), start_pos.y()});
        repaint();

        return;
    }

    if (!finish_set)
    {
        finish_pos = event->pos();
        finish_set = true;

        visibility_graph.add_vertex(static_cast<int>(vertices.size()));
        vertices.push_back(Vertex{static_cast<int>(vertices.size()), finish_pos.x(), finish_pos.y()});

        for (const auto& start: visibility_graph.get_vertices())
        {
            for (const auto& end: visibility_graph.get_vertices())
            {
                if (start == end || visibility_graph.has_edge(start, end))
                {
                    continue;
                }

                auto intersection {false};

                for (auto wall : walls)
                {
                    if (intersect(vertices[start], vertices[end], wall.v1, wall.v2))
                    {
                        intersection = true;
                        break;
                    }
                }

                if (intersection)
                {
                    continue;
                }

                for (auto diagonal : diagonals)
                {
                    if (intersect(vertices[start], vertices[end], diagonal.v1, diagonal.v2))
                    {
                        intersection = true;
                        break;
                    }
                }

                if (intersection)
                {
                    continue;
                }

                visibility_graph.add_edge(start, end,
                                     std::sqrt(std::pow(vertices[start].x - vertices[end].x, 2) +
                                                      std::pow(vertices[start].y - vertices[end].y, 2)));
            }
        }

        auto result {shortest_path(visibility_graph,
                                   static_cast<int>(vertices.size() - 1),
                                   static_cast<int>(vertices.size() - 2))};

        for (const auto& vertex : result)
        {
            path.emplace_back(QPoint{vertices[vertex].x,vertices[vertex].y});
        }
    }

    repaint();
}

