/****************************************************************************
** Meta object code from reading C++ file 'isoluxdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../isoluxdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'isoluxdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ISOLuxDialog_t {
    QByteArrayData data[9];
    char stringdata0[191];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ISOLuxDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ISOLuxDialog_t qt_meta_stringdata_ISOLuxDialog = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ISOLuxDialog"
QT_MOC_LITERAL(1, 13, 15), // "on_rb2D_toggled"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 15), // "on_rb3D_toggled"
QT_MOC_LITERAL(4, 46, 32), // "on_spinHalfMapWidth_valueChanged"
QT_MOC_LITERAL(5, 79, 28), // "on_spinDistance_valueChanged"
QT_MOC_LITERAL(6, 108, 31), // "on_cmbPlane_currentIndexChanged"
QT_MOC_LITERAL(7, 140, 26), // "on_spinLevels_valueChanged"
QT_MOC_LITERAL(8, 167, 23) // "on_chkGrid_stateChanged"

    },
    "ISOLuxDialog\0on_rb2D_toggled\0\0"
    "on_rb3D_toggled\0on_spinHalfMapWidth_valueChanged\0"
    "on_spinDistance_valueChanged\0"
    "on_cmbPlane_currentIndexChanged\0"
    "on_spinLevels_valueChanged\0"
    "on_chkGrid_stateChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ISOLuxDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    1,   51,    2, 0x0a /* Public */,
       5,    1,   54,    2, 0x0a /* Public */,
       6,    1,   57,    2, 0x0a /* Public */,
       7,    1,   60,    2, 0x0a /* Public */,
       8,    1,   63,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void ISOLuxDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ISOLuxDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_rb2D_toggled(); break;
        case 1: _t->on_rb3D_toggled(); break;
        case 2: _t->on_spinHalfMapWidth_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_spinDistance_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_cmbPlane_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_spinLevels_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_chkGrid_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ISOLuxDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_ISOLuxDialog.data,
    qt_meta_data_ISOLuxDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ISOLuxDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ISOLuxDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ISOLuxDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ISOLuxDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
