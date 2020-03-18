#include "background.h"
#include <qmath.h>
#include <QDebug>

Background::Background()
    : m_count(0)
{
    m_data.resize(8 * 4);
    add(QVector3D((GLfloat)0.0,(GLfloat)0.0,(GLfloat)0.0),
        QVector3D((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0),
        QVector2D((GLfloat)0.0,(GLfloat)0.0)
        );

    add(QVector3D((GLfloat)240,(GLfloat)0.0,(GLfloat)0.0),
        QVector3D((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0),
        QVector2D((GLfloat)1.0,(GLfloat)0.0)
        );

    add(QVector3D((GLfloat)240,(GLfloat)240,(GLfloat)0.0),
        QVector3D((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0),
        QVector2D((GLfloat)1.0,(GLfloat)1.0)
        );

    add(QVector3D((GLfloat)0.0,(GLfloat)240,(GLfloat)0.0),
        QVector3D((GLfloat)0.0,(GLfloat)0.0,(GLfloat)1.0),
        QVector2D((GLfloat)0.0,(GLfloat)1.0)
        );

}

void Background::add(const QVector3D &v, const QVector3D &n,const QVector2D &c)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    *p++ = c.x();
    *p++ = c.y();

    m_count += 8;
}
