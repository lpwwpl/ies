/****************************************************************************
** Meta object code from reading C++ file 'IESglwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../IESglwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IESglwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ColorBar_t {
    QByteArrayData data[1];
    char stringdata0[9];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ColorBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ColorBar_t qt_meta_stringdata_ColorBar = {
    {
QT_MOC_LITERAL(0, 0, 8) // "ColorBar"

    },
    "ColorBar"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ColorBar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ColorBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ColorBar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ColorBar.data,
    qt_meta_data_ColorBar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ColorBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ColorBar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ColorBar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ColorBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_PointCloudViewer_t {
    QByteArrayData data[11];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PointCloudViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PointCloudViewer_t qt_meta_stringdata_PointCloudViewer = {
    {
QT_MOC_LITERAL(0, 0, 16), // "PointCloudViewer"
QT_MOC_LITERAL(1, 17, 16), // "pointSizeChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 4), // "size"
QT_MOC_LITERAL(4, 40, 22), // "xyPlanePositionChanged"
QT_MOC_LITERAL(5, 63, 8), // "position"
QT_MOC_LITERAL(6, 72, 22), // "yzPlanePositionChanged"
QT_MOC_LITERAL(7, 95, 22), // "xzPlanePositionChanged"
QT_MOC_LITERAL(8, 118, 20), // "planePositionChanged"
QT_MOC_LITERAL(9, 139, 9), // "PlaneType"
QT_MOC_LITERAL(10, 149, 4) // "type"

    },
    "PointCloudViewer\0pointSizeChanged\0\0"
    "size\0xyPlanePositionChanged\0position\0"
    "yzPlanePositionChanged\0xzPlanePositionChanged\0"
    "planePositionChanged\0PlaneType\0type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PointCloudViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,
       7,    1,   48,    2, 0x06 /* Public */,
       8,    2,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    5,
    QMetaType::Void, QMetaType::Float,    5,
    QMetaType::Void, QMetaType::Float,    5,
    QMetaType::Void, 0x80000000 | 9, QMetaType::Float,   10,    5,

       0        // eod
};

void PointCloudViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PointCloudViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pointSizeChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->xyPlanePositionChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->yzPlanePositionChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: _t->xzPlanePositionChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: _t->planePositionChanged((*reinterpret_cast< PlaneType(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PointCloudViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PointCloudViewer::pointSizeChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PointCloudViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PointCloudViewer::xyPlanePositionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PointCloudViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PointCloudViewer::yzPlanePositionChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PointCloudViewer::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PointCloudViewer::xzPlanePositionChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PointCloudViewer::*)(PlaneType , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PointCloudViewer::planePositionChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PointCloudViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_PointCloudViewer.data,
    qt_meta_data_PointCloudViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PointCloudViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PointCloudViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PointCloudViewer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions"))
        return static_cast< QOpenGLFunctions*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int PointCloudViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void PointCloudViewer::pointSizeChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PointCloudViewer::xyPlanePositionChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PointCloudViewer::yzPlanePositionChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PointCloudViewer::xzPlanePositionChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PointCloudViewer::planePositionChanged(PlaneType _t1, float _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
struct qt_meta_stringdata_PlaneDataPlotter_t {
    QByteArrayData data[1];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlaneDataPlotter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlaneDataPlotter_t qt_meta_stringdata_PlaneDataPlotter = {
    {
QT_MOC_LITERAL(0, 0, 16) // "PlaneDataPlotter"

    },
    "PlaneDataPlotter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaneDataPlotter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void PlaneDataPlotter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject PlaneDataPlotter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PlaneDataPlotter.data,
    qt_meta_data_PlaneDataPlotter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlaneDataPlotter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaneDataPlotter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlaneDataPlotter.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlaneDataPlotter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
