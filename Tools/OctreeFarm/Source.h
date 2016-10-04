#pragma once
#include <QObject>
#include <QColor>
#include <QSharedPointer>
#include <QVector>
#include <json/json.hpp>

struct Node;

using json = nlohmann::json;

class Source : public QObject {
    Q_OBJECT

public:
    explicit Source(QObject* parent = 0);
    void create(const QString& string = QString());
    QString serialize();
    QSharedPointer<QVector<uint32_t>> binary();
    bool deleteNode(const QVector<QSharedPointer<Node>>& selection);

private:
    json root;
    uint32_t defaultColor = 0xFF4681FF;
};
