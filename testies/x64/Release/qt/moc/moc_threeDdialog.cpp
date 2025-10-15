/****************************************************************************
** Meta object code from reading C++ file 'threeDdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../threeDdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'threeDdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_IESPointCloudWidget_t {
    QByteArrayData data[6];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IESPointCloudWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IESPointCloudWidget_t qt_meta_stringdata_IESPointCloudWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "IESPointCloudWidget"
QT_MOC_LITERAL(1, 20, 17), // "updateIESDataMesh"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 20), // "updateIESDataShading"
QT_MOC_LITERAL(4, 60, 18), // "updateIESDataColor"
QT_MOC_LITERAL(5, 79, 18) // "updateIESDataShape"

    },
    "IESPointCloudWidget\0updateIESDataMesh\0"
    "\0updateIESDataShading\0updateIESDataColor\0"
    "updateIESDataShape"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IESPointCloudWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       3,    1,   37,    2, 0x0a /* Public */,
       4,    1,   40,    2, 0x0a /* Public */,
       5,    1,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void IESPointCloudWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<IESPointCloudWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateIESDataMesh((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->updateIESDataShading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->updateIESDataColor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->updateIESDataShape((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject IESPointCloudWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QVTKOpenGLNativeWidget::staticMetaObject>(),
    qt_meta_stringdata_IESPointCloudWidget.data,
    qt_meta_data_IESPointCloudWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *IESPointCloudWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IESPointCloudWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IESPointCloudWidget.stringdata0))
        return static_cast<void*>(this);
    return QVTKOpenGLNativeWidget::qt_metacast(_clname);
}

int IESPointCloudWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QVTKOpenGLNativeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_ThreeDDialog_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ThreeDDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ThreeDDialog_t qt_meta_stringdata_ThreeDDialog = {
    {
QT_MOC_LITERAL(0, 0, 12) // "ThreeDDialog"

    },
    "ThreeDDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThreeDDialog[] = {

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

void ThreeDDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ThreeDDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_ThreeDDialog.data,
    qt_meta_data_ThreeDDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ThreeDDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ThreeDDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ThreeDDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ThreeDDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
